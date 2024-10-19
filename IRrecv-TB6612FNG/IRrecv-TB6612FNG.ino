#include <IRremote.h>

const int RECV_PIN = 11;  // Pin a cui è collegato il ricevitore IR
IRrecv irrecv(RECV_PIN);
decode_results results;

// Codici IR
unsigned long CODE_FORWARD = 0xF5E60C23;  // Camminare avanti
unsigned long CODE_SOUND = 0x1DE889B0;    // Emettere suono
unsigned long CODE_STOP = 0x3021E99;      // Stop

// Pin per il TB6612FNG
const int AIN1 = 2;  // Pin per AIN1
const int AIN2 = 3;  // Pin per AIN2
const int PWMA = 4;  // Pin PWM per il controllo della velocità
const int STBY = 5;  // Pin per attivare/disattivare il motore (standby)

// Pin per il LED
const int LED_PIN = 6;  // Pin del LED

void setup() {
  Serial.begin(9600);  // Inizializza la comunicazione seriale
  irrecv.enableIRIn(); // Abilita il ricevitore IR

  // Imposta i pin del motor driver e del LED come uscite
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(LED_PIN, OUTPUT);  // Imposta il pin del LED come uscita

  // All'inizio, assicuriamoci che il motore sia fermo e il LED sia spento
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(STBY, LOW);  // Standby attivo
  digitalWrite(LED_PIN, LOW);  // Spegni il LED all'inizio
}

void loop() {
  if (irrecv.decode(&results)) {
    unsigned long receivedCode = results.value;

    // Stampa il codice ricevuto per il debug
    Serial.print("Codice ricevuto: ");
    Serial.println(receivedCode, HEX);

    // Esegui l'azione corrispondente al codice ricevuto
    if (receivedCode == CODE_FORWARD) {
      Serial.println("Camminare avanti");
      digitalWrite(AIN1, HIGH);  // Attiva AIN1 per la direzione avanti
      digitalWrite(AIN2, LOW);   // Disattiva AIN2
      digitalWrite(STBY, HIGH);  // Esci dalla modalità standby per far funzionare il motore
      analogWrite(PWMA, 255);    // Imposta la velocità massima
      digitalWrite(LED_PIN, HIGH); // Accendi il LED quando la locomotiva cammina
    } 
    else if (receivedCode == CODE_SOUND) {
      Serial.println("Emettere suono");
      // Aggiungi qui il suono o altra azione
    } 
    else if (receivedCode == CODE_STOP) {
      Serial.println("Stop");
      digitalWrite(AIN1, LOW);   // Disattiva AIN1
      digitalWrite(AIN2, LOW);   // Disattiva AIN2
      analogWrite(PWMA, 0);      // Imposta la velocità del motore a 0
      digitalWrite(STBY, LOW);   // Riattiva la modalità standby per fermare il motore
      digitalWrite(LED_PIN, LOW); // Spegni il LED quando la locomotiva si ferma
    }

    // Riprendi la ricezione del prossimo segnale
    irrecv.resume();  
  }
}
