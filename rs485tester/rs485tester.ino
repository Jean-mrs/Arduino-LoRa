#include <SoftwareSerial.h>
#define SSerialRX        10  //Serial Rx pin
#define SSerialTX        11  //Serial Tx pin

#define SSerialTxControl 3   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

byte byteReceived[8];
byte msg[8] = {0x01, 0x03, 0x00, 0x0C, 0x00, 0x01, 0x44, 0x09}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)

void setup() {

  Serial.begin(9600);
  RS485Serial.begin(9600);

  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, RS485Transmit);  // Init as Transceiver
}


void loop() {
  if (RS485Serial.isListening()) {
    Serial.println("Enviando request...");
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 to Transmit
    delay(500);
    RS485Serial.write(msg, 8);
    Serial.print("Mensagem enviada: ");

    for (int i = 0; i < sizeof(msg); i++) {
      Serial.print("0x");
      Serial.print(msg[i], HEX);
      Serial.print(" ");
    }

    Serial.println();

  }

  digitalWrite(SSerialTxControl, RS485Receive);  // Enable RS485 to receive
  Serial.print("Aguardando a resposta");
  while (!RS485Serial.available()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Mensagem recebida: ");
  int n = sizeof(byteReceived);
  for (int i = 0; i < n; i++) {
    byteReceived[i] = RS485Serial.read();    // Read received byte
    Serial.print("0x");
    Serial.print(byteReceived[i], HEX);
    Serial.print(" ");
  }


  Serial.println();
  long value;
  value = ((long)(byteReceived[3]) << 8)
          + ((long)(byteReceived[4]));
  Serial.print("Tensão: ");
  Serial.print(value / 10.0);
  Serial.print("(V)");

  Serial.println();
  Serial.println("#######################################################");
  delay(5000);

}
