// Planta com twitters
// Arduino Day 2015 Fortaleza - Palestra Internet das Coisas com Arduino
// (c) Copyright 2015 ronivaldo@gmail.com
// Released under Apache License, version 2.0
// https://github.com/ronivaldo/plantaIoT
#include <SPI.h>
#include <Ethernet.h>
#include <Twitter.h>

int buzzerPin = 2;
int sensorAguaPin = A0;

// Ethernet Shield Settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Your Token to Tweet (get it from http://arduino-tweet.appspot.com/)
Twitter twitter("1234567890-ASDFASDFASDFASDFZCVZXCVASDFAFZXCVCVZXVASDF");

int NIVEL_SEM_AGUA = 1;
int NIVEL_MINIMO_AGUA = 2;
int NIVEL_POUCA_AGUA = 3;
int NIVEL_INTERMEDIARIO_AGUA = 4;
int NIVEL_CHEIO_AGUA = 5;

int ultimoNivelAgua = 0;

void setup() {

  delay(1000);
  pinMode(sensorAguaPin, INPUT);     
  pinMode(buzzerPin, OUTPUT);   
  // desliga o buzzer - logica inversa
  digitalWrite(buzzerPin, HIGH);

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  // or you can use DHCP for autoomatic IP address configuration.
  // Ethernet.begin(mac);
  Serial.begin(9600);
  
}

void loop() {
  
  int valorSensorAgua = analogRead(sensorAguaPin);
  int nivelAgua = map(valorSensorAgua, 0, 1023, 0, 4);
  Serial.print("Nivel de Água [0..4]=");
  Serial.println(nivelAgua);
  
  if (nivelAgua < NIVEL_SEM_AGUA) {
    postToTwitter("@ronivaldops Estou morrendo de sede!!");    
    beep();
    delay(20000);
  } else if (nivelAgua < NIVEL_MINIMO_AGUA) {
    postToTwitter("@ronivaldops Preciso de Água Urgente!");    
    delay(21000);
  } else if (nivelAgua < NIVEL_POUCA_AGUA) {
    postToTwitter("@ronivaldops Que tal passar para me regar e trocar uma ideia?");    
    delay(22000);
  } else if (nivelAgua < NIVEL_INTERMEDIARIO_AGUA) {
    if (nivelAgua != ultimoNivelAgua) {
      postToTwitter("@ronivaldops Pode apreciar minha beleza, estou satisfeita.");    
    }  
    delay(23000);
  // NIVEL_CHEIO_AGUA
  } else {
    if (nivelAgua != ultimoNivelAgua) {
      postToTwitter("@ronivaldops Que maravilha! Acabei de ser regada! Choveu ou você veio me visitar?");    
    }  
    delay(24000);
  }  
  
  // guarda o ultimo nivel de agua para evitar
  ultimoNivelAgua = nivelAgua;
}

void postToTwitter(String mensagem) {
  // Message to post
  char msg[141] = "";
  
  // adiciona millis para eveitar mensagem duplicada no twitter
  String mensagemParaEnviar = mensagem + " " + String(millis());
  mensagemParaEnviar.toCharArray(msg, sizeof(msg));

  Serial.println("connecting ..");
  Serial.println(mensagemParaEnviar);
  if (twitter.post(msg)) {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
}  

// toca um beep
void beep() {
  digitalWrite(buzzerPin, LOW);
  delay(1000);              
  digitalWrite(buzzerPin, HIGH);
  delay(1000);              
}
