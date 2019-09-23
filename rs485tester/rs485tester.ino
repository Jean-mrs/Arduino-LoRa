#include <Vector.h>

#include <SoftwareSerial.h>
#define SSerialRX        4  //Serial Rx pin
#define SSerialTX        5  //Serial Tx pin

#define SSerialTxControl 3   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // Mapeia RX, TX para o Serial do conversor

byte longbyteReceived[9];
byte byteReceived[9];
byte byteReceived2[9];
Vector<byte> bytes2send;
//byte resquest2send[121];

byte msgs[13][8] = {
  {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}, //Registrador 0x0000 Total KWh
  {0x01, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xCB}, //Registrador 0x0001 Total kwh
  {0x01, 0x03, 0x00, 0x08, 0x00, 0x02, 0x45, 0xC9}, //Registrador 0x0008 Export kWh
  {0x01, 0x03, 0x00, 0x09, 0x00, 0x02, 0x14, 0x09}, //Registrador 0x0009 Export kWh
  {0x01, 0x03, 0x00, 0x0A, 0x00, 0x02, 0xE4, 0x09}, //Registrador 0x000A Import KWh
  {0x01, 0x03, 0x00, 0x0B, 0x00, 0x02, 0xB5, 0xC9}, //Registrador 0x000B Import KWh
  {0x01, 0x03, 0x00, 0x0C, 0x00, 0x01, 0x44, 0x09}, //Registrador 0x000C Voltage V
  {0x01, 0x03, 0x00, 0x0D, 0x00, 0x01, 0x15, 0xC9}, //Registrador 0x000D Current A
  {0x01, 0x03, 0x00, 0x0E, 0x00, 0x01, 0xE5, 0xC9}, //Registrador 0x000E Active Power kW
  {0x01, 0x03, 0x00, 0x0F, 0x00, 0x01, 0xB4, 0x09}, //Registrador 0x000F Reactive Power kvar
  {0x01, 0x03, 0x00, 0x10, 0x00, 0x01, 0x85, 0xCF}, //Registrador 0x0010 Power Factor
  {0x01, 0x03, 0x00, 0x11, 0x00, 0x01, 0xD4, 0x0F}, //Registrador 0x0011 Frequency Hz
  {0x01, 0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xCE}  //Registrador 0x0015 ID + Baud rate(01~04)
};

//byte msg1[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
byte msg2[8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xCB}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg3[8] = {0x01, 0x03, 0x00, 0x08, 0x00, 0x02, 0x45, 0xC9}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
byte msg4[8] = {0x01, 0x03, 0x00, 0x09, 0x00, 0x02, 0x14, 0x09}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg5[8] = {0x01, 0x03, 0x00, 0x0A, 0x00, 0x02, 0xE4, 0x09}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
byte msg6[8] = {0x01, 0x03, 0x00, 0x0B, 0x00, 0x02, 0xB5, 0xC9}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg7[8] = {0x01, 0x03, 0x00, 0x0C, 0x00, 0x01, 0x44, 0x09}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg8[8] = {0x01, 0x03, 0x00, 0x0D, 0x00, 0x01, 0x15, 0xC9}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg9[8] = {0x01, 0x03, 0x00, 0x0E, 0x00, 0x01, 0xE5, 0xC9}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg10[8] = {0x01, 0x03, 0x00, 0x0F, 0x00, 0x01, 0xB4, 0x09}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg11[8] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x01, 0x85, 0xCF}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg12[8] = {0x01, 0x03, 0x00, 0x11, 0x00, 0x01, 0xD4, 0x0F}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg13[8] = {0x01, 0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xCE}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)


void setup() {

  Serial.begin(9600); // Banda monitor Serial
  RS485Serial.begin(9600); //Banda do Smart Meter

  pinMode(SSerialTxControl, OUTPUT); // Habilita pino de controle de RX/TX
  digitalWrite(SSerialTxControl, RS485Transmit);  // Inicia com modo de transmissão ativado
}

void loop() {
  for (int i = 0; i < 1; i++) {
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

    if (i < 6) {    //Caso tenha mensagem em mais de um registrador(Data Number 2)
      Serial.print("Mensagem recebida: ");
      int n = sizeof(longbyteReceived);
      for (int k = 0; k < n; k++) {
        longbyteReceived[k] = RS485Serial.read();  // Visualizacao de mensagem hexadecimal recebida no monitor Serial
        Serial.print("0x");
        Serial.print(longbyteReceived[k], HEX);
        Serial.print(" ");
      }
      Serial.println();

//      for (int k = 0; k < 11; k++) {
//        if (k > 2 and k < 9) {
//          bytes2send.PushBack(longbyteReceived[k]); // Adiciona mensagem lida ao array final
//        }
//      }
    }
    else {  //Caso tenha mensagem em apenas um registrador(Data Number 1)
      Serial.print("Mensagem recebida: ");
      int n = sizeof(byteReceived);
      for (int k = 0; k < n; k++) {
        byteReceived[k] = RS485Serial.read();    // Visualizacao de mensagem hexadecimal recebida no monitor Serial
        Serial.print("0x");
        Serial.print(byteReceived[k], HEX);
        Serial.print(" ");
      }
      Serial.println();

//      for (int k = 0; k < 8; k++) {
//        if (k > 2 and k < 6) {
//          bytes2send.PushBack(byteReceived[k]); // Adiciona mensagem lida ao array final
//        }
//      }

    }
    Serial.println();
    Serial.println("#######################################################");
  }

//  for (int k = 0; k < 11; k++) { // Printar mensagem final
//    Serial.print("0x");
//    Serial.print(bytes2send[k], HEX);
//    Serial.print(" ");
//  }

//////////////////////////////////////////////////////////////

  Serial.println();
  Serial.println("Enviando request...");
  digitalWrite(SSerialTxControl, RS485Transmit);  // Habilita RS485 para Transmitir
  delay(500);
  RS485Serial.write(msg2, 8); // Escreve mensagens a enviar
  Serial.print("Mensagem enviada: ");

  for (int i = 0; i < sizeof(msg2); i++) {
    Serial.print("0x");
    Serial.print(msg2[i], HEX);
    Serial.print(" ");
  }

  Serial.println();



  digitalWrite(SSerialTxControl, RS485Receive);  // Enable RS485 to receive
  Serial.print("Aguardando a resposta");
  while (!RS485Serial.available()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Mensagem recebida: ");
  int n2 = sizeof(byteReceived2);
  for (int i = 0; i < n2; i++) {
    byteReceived2[i] = RS485Serial.read();    // Read received byte
    Serial.print("0x");
    Serial.print(byteReceived2[i], HEX);
    Serial.print(" ");
  }
  ///////////////////////////////////////////////////////////////

  Serial.println();
  long value_4bytes;
  value_4bytes = ((long)(byteReceived2[3]) << 56)
                 + ((long)(byteReceived2[4]) << 48)
                 + ((long)(byteReceived2[5]) << 40)
                 + ((long)(byteReceived2[6]) << 32)
                 + ((long)(byteReceived[3]) << 24)
                 + ((long)(byteReceived[4]) << 16)
                 + ((long)(byteReceived[5]) << 8)
                 + ((long)(byteReceived[6]));
  Serial.print("Tensão: ");
  Serial.print(value_4bytes/100.0 );
  Serial.print("(Kwh)");

while(1){
  
}
}
  /*Serial.println();
    long value_2bytes;
    value_2bytes = ((long)(byteReceived[3]) << 8)
          + ((long)(byteReceived[4]));
    Serial.print("Tensão: ");
    Serial.print(value_2bytes / 10.0);
    Serial.print("(V)");


  Serial.println();
  Serial.println("#######################################################");
  delay(5000);
*/
