

#include <Arduino.h>
#include <IRremote.hpp> // include the library

// Pin a cui è collegato il ricevitore IR
const int RECV_PIN = 11;

// Codici IR
//unsigned long CODE_FORWARD = 0xF5E60C23;  // Codice per camminare avanti
//unsigned long CODE_STOP = 0x3021E99;      // Codice per fermarsi
//unsigned long CODE_SOUND = 0x1DE889B0;    // Codice per emettere suono

unsigned long CODE_FORWARD = 0x45;  // Codice per camminare avanti
unsigned long CODE_STOP = 0x46;      // Codice per fermarsi
unsigned long CODE_SOUND = 0x47;    // Codice per emettere suono


// Pin per il L293D
const int IN1 = 2;  // Pin per IN1 (direzione)
const int IN2 = 3;  // Pin per IN2 (direzione)
const int ENA = 4;  // Pin PWM per la velocità

// Pin per il LED
const int LED_PIN = 6;  // Pin per indicare azioni

// Pin per il cicalino (buzzer)
const int BUZZER_PIN = 7;

void setup() {
  Serial.begin(9600);  // Inizializza la comunicazione seriale per il debug
 IrReceiver.begin(RECV_PIN, true);  // Inizializza il ricevitore IR con feedback LED

  // Imposta i pin del motore e del LED come uscite
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Inizialmente spegni tutto
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);     // Motore fermo
  digitalWrite(LED_PIN, LOW);  // LED spento
}

void loop() {
  // Controlla se è stato ricevuto un segnale IR
  if (IrReceiver.decode()) {
    unsigned long receivedCode = IrReceiver.decodedIRData.decodedRawData;
    unsigned long rawCode = IrReceiver.decodedIRData.decodedRawData;

    // Stampa il codice ricevuto per il debug
    Serial.println(receivedCode, HEX);

    // Controlla quale codice è stato ricevuto e agisci di conseguenza
    if (receivedCode == CODE_FORWARD) {
      Serial.println("Camminare avanti");
      digitalWrite(IN1, HIGH);  // Direzione avanti
      digitalWrite(IN2, LOW);   // IN2 disattivato
      analogWrite(ENA, 24);    // Velocità media (massima = 255)
      digitalWrite(LED_PIN, HIGH); // Accendi il LED

    } else if (receivedCode == CODE_STOP) {
      Serial.println("Fermarsi");
      digitalWrite(IN1, LOW);   // Ferma il motore
      digitalWrite(IN2, LOW);   // Ferma il motore
      analogWrite(ENA, 0);      // Spegni il motore
      digitalWrite(LED_PIN, LOW); // Spegni il LED

    } else if (receivedCode == CODE_SOUND) {
      Serial.println("Emettere suono");
      tone(BUZZER_PIN, 1000);  // Emetti un suono di 1000 Hz
      delay(500);              // Suona per 500 millisecondi
      noTone(BUZZER_PIN);      // Ferma il suono
    } else {
      Serial.println("Codice non riconosciuto");
    }

    // Riprendi la ricezione di nuovi segnali IR
    IrReceiver.resume();
  }
}
