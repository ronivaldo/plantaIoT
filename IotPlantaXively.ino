// Planta com twitters
// Arduino Day 2015 Fortaleza - Palestra Internet das Coisas com Arduino
// (c) Copyright 2015 ronivaldo@gmail.com
// Released under Apache License, version 2.0
// https://github.com/ronivaldo/plantaIoT
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>

// MAC address for your Ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Your Xively key to let you upload data
char xivelyKey[] = "ASDFA-SDF09AS8DF-0AASDasdçflkajsdfçasd-f0adf";
//your xively feed ID
#define xivelyFeed 321654654
//datastreams
char nivelAguaID[] = "nivel_agua";
char regadorID[] = "regador";

#define CHANNEL_NIVEL_AGUA 0
#define CHANNEL_REGADOR 1
 
// sensorde nivel de agua
#define sensorAguaPin A0
// regador connected pin
#define regadorPin 13

int NIVEL_SEM_AGUA = 1;
int NIVEL_MINIMO_AGUA = 2;
int NIVEL_POUCA_AGUA = 3;
int NIVEL_INTERMEDIARIO_AGUA = 4;
int NIVEL_CHEIO_AGUA = 5;
 
// Define the strings for our datastream IDs
XivelyDatastream datastreams[] = {
  XivelyDatastream(nivelAguaID, strlen(nivelAguaID), DATASTREAM_INT),
  XivelyDatastream(regadorID, strlen(regadorID), DATASTREAM_INT),
};

// Finally, wrap the datastreams into a feed
XivelyFeed feed(xivelyFeed, datastreams, 2 /* number of datastreams */);
 
EthernetClient client;
XivelyClient xivelyclient(client);
 
void setup() {
  // pin setup
  pinMode(sensorAguaPin, INPUT);
  pinMode(regadorPin, OUTPUT);
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  Serial.println("Starting multiple datastream upload to Xively...");
  Serial.println();

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }
}

// liga o regador durante os segundos passados, zero para desligar
void controlaRegador(long segundos) {
  if (segundos > 0) {
    digitalWrite(regadorPin, HIGH);   // liga o regador
    delay(segundos * 1000);           // aguarda o numero de segundos passados
  }  
  digitalWrite(regadorPin, LOW);    // desliga o regador
}  

// liga regador de acordo com o nivel de agua na planta
void processaRegador() {
  // otem dados do feed na nuvem
  int getReturn = xivelyclient.get(feed, xivelyKey);    
  if(getReturn > 0){
    Serial.print("Datastream Nivel de Agua: ");
    Serial.println(feed[CHANNEL_NIVEL_AGUA]);
  } else {
    Serial.println("HTTP Error");
  }  

  // guarda o nivel da agua do feed obtido
  int nivelAgua = feed[CHANNEL_NIVEL_AGUA].getInt();
  
  // calcula quanto tempo o regado deve ficar ligado
  int segundosRegadorDeveLigar = 0;
  if (nivelAgua < NIVEL_SEM_AGUA) {
    // liga regador por 5 segundo
    segundosRegadorDeveLigar = 5;
  } else if (nivelAgua < NIVEL_MINIMO_AGUA) {
    // liga regador por 3 segundo
    segundosRegadorDeveLigar = 3;
  } else if (nivelAgua < NIVEL_POUCA_AGUA) {
    // liga regador por 1 segundo
    segundosRegadorDeveLigar = 1;
  }  
  
  // liga ou desliga o regador de acordo com o nivel da agua
  controlaRegador(segundosRegadorDeveLigar); 

  // guarda o valor a ser salvo na nuvem
  datastreams[CHANNEL_REGADOR].setInt(segundosRegadorDeveLigar);
  
  Serial.print("Datastream Regador: ");
  Serial.println(feed[CHANNEL_REGADOR]);
  // send value to xively
  Serial.println("Regador");
  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  // return message
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  Serial.println("");
}  


// le sensor da planta e salva na nuvem
void processaNivelAgua() {
  int valorSensorAgua = analogRead(sensorAguaPin);
  int nivelAgua = map(valorSensorAgua, 0, 1023, 0, 4);
  Serial.print("Nivel de Água [0..4]=");
  Serial.println(nivelAgua);

  // guarda o valor na nuvem
  datastreams[CHANNEL_NIVEL_AGUA].setInt(nivelAgua);
  
  // send value to xively
  Serial.println("Nivel de Agua");
  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  // return message
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  Serial.println("");
}  


void loop() {
  processaNivelAgua();
  
  processaRegador();
  
  // delay between calls
  delay(15000);
}
