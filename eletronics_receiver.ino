#include <SPI.h>
#include <LoRa.h>
#include <HardwareSerial.h>

//#define DEBUG

const int SERIAL_BAUD_RATE = 115200;
#define COM 1
HardwareSerial SerialUSB(COM);

// Definicacao de constantes
const int csPin = 5;      // Chip Select ( Slave Select do protocolo SPI ) do modulo Lora
const int resetPin = 2;    // Reset do modulo LoRa
const int irqPin = 4;      // Pino DI0

byte localAddress = 0xBA;   // Endereco deste dispositivo LoRa
byte altimeterAddress = 0xBB; // Endereço do LoRa do altímetro
byte sateliteAddress = 0xBC;  // Endereço do LoRa do satélite
//byte msgCount = 0;         // Contador de mensagens enviadas

//long lastSendTime = 0; // TimeStamp da ultima mensagem enviada
//int interval = 5000;   // Intervalo em ms no envio das mensagens ( inicial 5s)

//A FAZER!!!!!!
//ver o que que ta dando de errado no SerialUSB.begin(SERIAL_BAUD_RATE)
//transformar os dados recebidos do LoRa em uma array (e adaptar o codigo pra isso)
//verificar se a jsonParser() funciona ou se vamos realmente precisar do ArduinoJson
//testar a comunicação serial
//atualizar a formatação do json


void jsonParser(int altimeterData)
// Recebe um vetor com os valores do altímetro e retorna uma String no formato json com eles
{
  String jsonString = "{";
  jsonString += "\"Altitude\":" + String(altimeterData[0]) + ",";
  jsonString += "\"Latitude\":" + String(altimeterData[1]) + ",";
  jsonString += "\"Longitude\":" + String(altimeterData[2]) + ",";
  jsonString += "\"Principal Paraquedas Estabilizador\":" + String(altimeterData[3]) + ",";
  jsonString += "\"Redundancia Paraquedas Estabilizador\":" + String(altimeterData[4]) + ",";
  jsonString += "\"Comercial Paraquedas Estabilizador\":" + String(altimeterData[5]) + ",";
  jsonString += "\"Principal Paraquedas Principal\":" + String(altimeterData[6]) + ",";
  jsonString += "\"Comercial Paraquedas Principal\":" + String(altimeterData[7]) + ",";
  jsonString += "\"Acelerometro\":{\"x\":" + String(altimeterData[8]) + ",\"y\":" + String(altimeterData[9]) + ",\"z\":" + String(altimeterData[10]) + "},";
  jsonString += "\"Giroscopio\":{\"x\":" + String(altimeterData[11]) + ",\"y\":" + String(altimeterData[12]) + ",\"z\":" + String(altimeterData[13]) + "},";
  jsonString += "\"RSSI\":" + String(altimeterData[14]) + ",";  
  jsonString += "}";

  return jsonString;
}


void eletroicsReceiver(int packetSize)
{
  if (packetSize == 0)
    return; // Se nenhuma mesnagem foi recebida , retorna nada
  // Leu um pacote , vamos decodificar ?
  int recipient = LoRa.read();       // Endereco de quem ta recebendo
  byte sender = LoRa.read();         // Endereco do remetente
  byte incomingMsgId = LoRa.read();  // Mensagem
  byte incomingLength = LoRa.read(); // Tamanho da mensagem
  String incoming = "";
  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }
  if (incomingLength != incoming.length())
  {
    // check length for error
    Serial.println(" erro !: o tamanho da mensagem nao condiz com o conteudo !");
    return;
  }
  // se a mensagem não estiver endereçada para este dispositivo ,
  if (recipient != localAddress && recipient != 0xFF)
  {
    #ifdef DEBUG:
      Serial.println("Mensagem não endereçada para este LoRa.");
    #endif
    return;
  }
  // Caso a mensagem seja para este dispositivo e veio do altímetro,
  // imprime os detalhes
  if (sender == altimeterAddress)
  {
    #ifdef DEBUG:
      Serial.println(" Recebido do dispositivo : 0x" + String(sender, HEX));
      Serial.println(" Enviado para : 0x" + String(recipient, HEX));
      Serial.println("ID da mensagem : " + String(incomingMsgId));
      Serial.println(" Tamanho da mensagem : " + String(incomingLength));
      Serial.println(" Mensagem : " + incoming);
      Serial.println(" RSSI : " + String(LoRa.packetRssi()));
      Serial.println(" Snr : " + String(LoRa.packetSnr()));
      Serial.println();
    #endif
    SerialUSB.print(jsonParser(incoming);
  }
}



void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG:
    Serial.begin(115200);
    Serial.println("Projeto LoRa Receiver");
  #endif
  LoRa.setPins(csPin, resetPin, irqPin);
  if(!LoRa.begin(915E6))
  {
    #ifdef DEBUG:
      Serial.println("Erro ao iniciar o módulo LoRa!");
    #endif
  }
  if(!SerialUSB.begin(SERIAL_BAUD_RATE))
  {
    #ifdef DEBUG:
      Serial.println("Erro ao estabelecer conexão Serial");
    #endif
  } 
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation
  eletroicsReceiver(LoRa.parsePacket());
}
