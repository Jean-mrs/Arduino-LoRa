#include <SoftwareSerial.h>
#define SSerialRX        10  //Serial Rx pin
#define SSerialTX        11  //Serial Tx pin

#define SSerialTxControl 3   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // Mapeia RX, TX para o Serial do conversor

byte byteReceived[11];
byte byteReceived2[11];
//byte msg1[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg2[8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xCB}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg3[8] = {0x01, 0x03, 0x00, 0x08, 0x00, 0x02, 0x45, 0xC9}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg4[8] = {0x01, 0x03, 0x00, 0x09, 0x00, 0x02, 0x14, 0x09}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg5[8] = {0x01, 0x03, 0x00, 0x0A, 0x00, 0x02, 0xE4, 0x09}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
//byte msg6[8] = {0x01, 0x03, 0x00, 0x0B, 0x00, 0x02, 0xB5, 0xC9}; //meterID, functionCode, registerAddr(2 bytes), dataNumber(2bytes), checkCode(2bytes)
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
  if (RS485Serial.isListening()) {
    Serial.println("Enviando request...");
    digitalWrite(SSerialTxControl, RS485Transmit);  // Habilita RS485 para Transmitir
    delay(500);
    RS485Serial.write(msg5, 8); // Escreve mensagens a enviar
    Serial.print("Mensagem enviada: ");

    for (int i = 0; i < sizeof(msg5); i++) { //Visualizacao de mensagem hexadecimal enviada no monitor Serial
      Serial.print("0x");
      Serial.print(msg5[i], HEX);
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
  
  Serial.print("Mensagem recebida: ");
  int n = sizeof(byteReceived);
  for (int i = 0; i < n; i++) {
    byteReceived[i] = RS485Serial.read();    // Visualizacao de mensagem hexadecimal recebida no monitor Serial
    Serial.print("0x");
    Serial.print(byteReceived[i], HEX);
    Serial.print(" ");
  }
  //////////////////////////////////////////////////////////////

  Serial.println();
  Serial.println("Enviando request...");
  digitalWrite(SSerialTxControl, RS485Transmit);  // Habilita RS485 para Transmitir
  delay(500);
  RS485Serial.write(msg6, 8); // Escreve mensagens a enviar
  Serial.print("Mensagem enviada: ");

  for (int i = 0; i < sizeof(msg6); i++) {
    Serial.print("0x");
    Serial.print(msg6[i], HEX);
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
  //Serial.print("Tensão: ");
  Serial.print(value_4bytes / 100.0);
  Serial.print("(Kwh)");


  /*Serial.println();
  long value_2bytes;
  value_2bytes = ((long)(byteReceived[3]) << 8)
          + ((long)(byteReceived[4]));
  Serial.print("Tensão: ");
  Serial.print(value_2bytes / 10.0);
  Serial.print("(V)");
  */
  
  Serial.println();
  Serial.println("#######################################################");
  delay(5000);

}
