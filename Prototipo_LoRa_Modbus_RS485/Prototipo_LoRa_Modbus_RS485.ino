#include <SPI.h>
#include <lmic.h>
#include <hal/hal.h>
#include <Vector.h>
#include <SoftwareSerial.h>

/////////////////////////Configuracao de comunicacao Modbus em RS485 /////////////////////////
#define SSerialRX        4  //Serial Rx pin
#define SSerialTX        5  //Serial Tx pin
#define SSerialTxControl 3   //RS485 Direction control
#define RS485Transmit    HIGH
#define RS485Receive     LOW
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // Mapeia RX, TX para o Serial do conversor

Vector<byte> bytes2send;
uint8_t data[22];

byte msgs[9][8] = {
  {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}, //Registrador 0x0000 Total KWh
  {0x01, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xCB}, //Registrador 0x0001 Total kwh
  //{0x01, 0x03, 0x00, 0x08, 0x00, 0x02, 0x45, 0xC9}, //Registrador 0x0008 Export kWh
  //{0x01, 0x03, 0x00, 0x09, 0x00, 0x02, 0x14, 0x09}, //Registrador 0x0009 Export kWh
  //{0x01, 0x03, 0x00, 0x0A, 0x00, 0x02, 0xE4, 0x09}, //Registrador 0x000A Import KWh
  //{0x01, 0x03, 0x00, 0x0B, 0x00, 0x02, 0xB5, 0xC9}, //Registrador 0x000B Import KWh
  {0x01, 0x03, 0x00, 0x0C, 0x00, 0x01, 0x44, 0x09}, //Registrador 0x000C Voltage V
  {0x01, 0x03, 0x00, 0x0D, 0x00, 0x01, 0x15, 0xC9}, //Registrador 0x000D Current A
  {0x01, 0x03, 0x00, 0x0E, 0x00, 0x01, 0xE5, 0xC9}, //Registrador 0x000E Active Power kW
  {0x01, 0x03, 0x00, 0x0F, 0x00, 0x01, 0xB4, 0x09}, //Registrador 0x000F Reactive Power kvar
  {0x01, 0x03, 0x00, 0x10, 0x00, 0x01, 0x85, 0xCF}, //Registrador 0x0010 Power Factor
  {0x01, 0x03, 0x00, 0x11, 0x00, 0x01, 0xD4, 0x0F}, //Registrador 0x0011 Frequency Hz
  {0x01, 0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xCE}  //Registrador 0x0015 ID + Baud rate(01~04)
};

void requestModbus() {
  for (int i = 0; i < 9; i++) {
    if (RS485Serial.isListening()) {
      Serial.println("Enviando request...");
      digitalWrite(SSerialTxControl, RS485Transmit);  // Habilita RS485 para Transmitir
      delay(500);
      RS485Serial.write(msgs[i], 8); // Escreve mensagens a enviar
      Serial.print("Mensagem enviada: ");

      for (int j = 0; j < sizeof(msgs[i]); j++) { //Visualizacao de mensagem hexadecimal enviada no monitor Serial
        Serial.print("0x");
        Serial.print(msgs[i][j], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
    digitalWrite(SSerialTxControl, RS485Receive);  // Habilita RS485 para receber mensagem do Smart Meter

    Serial.print("Aguardando a resposta");
    while (!RS485Serial.available()) { //Loop de aguardo ate que seja estabelecida conexao
      Serial.print(".");
      delay(500);
    }
    Serial.println();

    if (i < 2) {    //Caso tenha mensagem em mais de um registrador(Data Number 2)
      Serial.print("Mensagem recebida: ");
      byte longbyteReceived[9];
      int n = sizeof(longbyteReceived);
      for (int k = 0; k < n; k++) {
        longbyteReceived[k] = RS485Serial.read();  // Visualizacao de mensagem hexadecimal recebida no monitor Serial
        Serial.print("0x");
        Serial.print(longbyteReceived[k], HEX);
        Serial.print(" ");
      }
      Serial.println();

      for (int k = 0; k < 9; k++) {
        if (k > 2 and k < 7) {
          bytes2send.PushBack(longbyteReceived[k]); // Adiciona mensagem lida ao array final
        }
      }
    }
    else {  //Caso tenha mensagem em apenas um registrador(Data Number 1)
      byte byteReceived[7];
      Serial.print("Mensagem recebida: ");
      int n = sizeof(byteReceived);
      for (int k = 0; k < n; k++) {
        byteReceived[k] = RS485Serial.read();    // Visualizacao de mensagem hexadecimal recebida no monitor Serial
        Serial.print("0x");
        Serial.print(byteReceived[k], HEX);
        Serial.print(" ");
      }
      Serial.println();

      for (int k = 0; k < 7; k++) {
        if (k > 2 and k < 5) {
          bytes2send.PushBack(byteReceived[k]); // Adiciona mensagem lida ao array final
        }
      }
    }
    Serial.println();
    Serial.println("#######################################################");
  }

  for (int k = 0; k < 22; k++) { // Printar mensagem final
    data[k] = (uint8_t)bytes2send[k];
    Serial.print("0x");
    Serial.print(data[k], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

/////////////////////////Configuracao de comunicacao LoRa /////////////////////////

#ifdef COMPILE_REGRESSION_TEST
# define FILLMEIN 0
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// LoRaWAN NwkSKey, network session key
static const PROGMEM u1_t NWKSKEY[16] = { 0x38, 0xD9, 0x78, 0x96, 0x28, 0x9C, 0x00, 0xC4, 0x2E, 0xFE, 0xF2, 0xC5, 0x49, 0xAB, 0xCD, 0xAC };

// LoRaWAN AppSKey, application session key
static const u1_t PROGMEM APPSKEY[16] = { 0xE9, 0xFF, 0x30, 0xD2, 0xD4, 0x36, 0xB2, 0x3A, 0x3C, 0x98, 0xA4, 0xCF, 0x06, 0xB8, 0xE2, 0x65 };

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
// The library converts the address to network byte order as needed.
static const u4_t DEVADDR = 0x26031FE8 ; // <-- Change this address for every node!

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in arduino-lmic/project_config/lmic_project_config.h,
// otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static uint8_t mydata[] = "Hello, world!";
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

// Pin mapping
// Adapted for Feather M0 per p.10 of [feather]
const lmic_pinmap lmic_pins = {
  .nss = 10,                       // chip select on feather (rf95module) CS
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 9,                       // reset pin
  .dio = {2, 6, 7}, // assumes external jumpers [feather_lora_jumper]
};

void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_RFU1:
      ||     Serial.println(F("EV_RFU1"));
      ||     break;
    */
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }
      // Schedule next transmission
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_SCAN_FOUND:
      ||    Serial.println(F("EV_SCAN_FOUND"));
      ||    break;
    */
    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      break;
    default:
      Serial.print(F("Unknown event: "));
      Serial.println((unsigned) ev);
      break;
  }
}

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, data, sizeof(data), 0);
    Serial.println(F("Packet queued"));
    Serial.println(LMIC.freq);
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
  /////////////////////////Setup Modbus em RS485 /////////////////////////
  Serial.begin(9600); // Banda do monitor Serial
  RS485Serial.begin(9600); //Banda do Smart Meter

  pinMode(SSerialTxControl, OUTPUT); // Habilita pino de controle de RX/TX
  digitalWrite(SSerialTxControl, RS485Transmit);  // Inicia com modo de transmissão ativado

  /////////////////////////Setup envio LoRa /////////////////////////
  while (!Serial); // wait for Serial to be initialized
  delay(100);     // per sample code on RF_95 test
  Serial.println(F("Starting"));

#ifdef VCC_ENABLE
  // For Pinoccio Scout boards
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, HIGH);
  delay(1000);
#endif

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // Set static session parameters. Instead of dynamically establishing a session
  // by joining the network, precomputed session parameters are be provided.
#ifdef PROGMEM
  // On AVR, these values are stored in flash and only copied to RAM
  // once. Copy them to a temporary buffer here, LMIC_setSession will
  // copy them into a buffer of its own again.
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession (0x13, DEVADDR, nwkskey, appskey);
#else
  // If not running an AVR with PROGMEM, just use the arrays directly
  LMIC_setSession (0x13, DEVADDR, NWKSKEY, APPSKEY);
#endif

#if defined(CFG_us915)
  // NA-US channels 0-71 are configured automatically
  // but only one group of 8 should (a subband) should be active
  // TTN recommends the second sub band, 1 in a zero based count.
  // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
  LMIC_selectSubBand(1);
#endif

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7, 14);

  //Comeca a receber mensagem do modbus
  requestModbus();

  // Start job
  do_send(&sendjob);
}


void loop() {
  os_runloop_once();
}
