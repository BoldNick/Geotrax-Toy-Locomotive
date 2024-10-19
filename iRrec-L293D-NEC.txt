#define DECODE_NEC      // Enable decoding of NEC protocol
#include <IRremote.hpp> // Include InfraRed library
#include <toneAC.h>

// Pins
constexpr uint8_t RECV_PIN {7};      // Pin for IR receiver
constexpr uint8_t LED_PIN {8};       // Pin for LED
constexpr uint8_t BUZZER_PIN {6};    // Pin for Buzzer

// IR Codes
constexpr uint16_t CODE_FORWARD = 0x45;  // NEC IR Code for Move Forward
constexpr uint16_t CODE_STOP = 0x46;     // NEC IR Code for Stop
constexpr uint16_t CODE_SOUND = 0x47;    // NEC IR Code for Sound
constexpr uint16_t CODE_BACKWARD = 0x44; // NEC IR Code for Move Backward

// Pins for the L293D Motor Driver
constexpr uint8_t IN1 {3};  // Pin for IN1 (spin)
constexpr uint8_t IN2 {4};  // Pin for IN2 (spin)
constexpr uint8_t ENA {5};  // Pin for ENA (speed)


uint16_t irReceive() {
  uint16_t received{0};
  
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      // We have an unknown protocol here, print more info
      IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    if (IrReceiver.decodedIRData.protocol == NEC) {
      received = IrReceiver.decodedIRData.command;
      Serial.print("Command: 0x");
      Serial.println(received, HEX);
    }
    IrReceiver.resume();
  }
  return received;
}

void setup()
{
  Serial.begin(9600);

  pinMode (LED_PIN, OUTPUT); 
  pinMode(BUZZER_PIN, OUTPUT);
    // Set up the motor Pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

    // Initialize all the outputs turned off
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);            // Motor Stop
  digitalWrite(LED_PIN, LOW);     // LED turned off
  digitalWrite(BUZZER_PIN, LOW);  // Buzzer turned off

  // Start IR receiver on the specified pin
  IrReceiver.begin(RECV_PIN);
  Serial.print(F("Ready to receive IR signals at pin "));
  Serial.println(RECV_PIN);
}

void loop()
{
 //  if (irReceive() == CODE_FORWARD) { digitalWrite(LED_PIN,!digitalRead(LED_PIN)); }

   switch (irReceive()) {
    case CODE_FORWARD:
      Serial.println("Move Forward");
      digitalWrite(IN1, HIGH);  // Move forward
      digitalWrite(IN2, LOW);   // IN2 deactivated
      analogWrite(ENA, 128);     // Medium speed (max = 255)
      digitalWrite(LED_PIN, HIGH); // Turn on LED
      break;

    case CODE_STOP:
      Serial.println("Stop");
      digitalWrite(IN1, LOW);   // Stop motor
      digitalWrite(IN2, LOW);   // Stop motor
      analogWrite(ENA, 0);      // Turn off motor
      digitalWrite(LED_PIN, LOW); // Turn off LED
      break;

    case CODE_BACKWARD:
      Serial.println("Move Backward");
      digitalWrite(IN1, LOW);   // Stop motor
      digitalWrite(IN2, HIGH);   // Stop motor
      analogWrite(ENA, 128);      // Turn off motor
      digitalWrite(LED_PIN, HIGH); // Turn off LED
      break;


    case CODE_SOUND:
      Serial.println("Sound");
      toneAC(2000, 255);  // Emit a 1000 Hz sound ToneAC uses PINs 9 and 10. They cannot be modified
      delay(500);              // Sound for 500 milliseconds
      noToneAC();      // Stop sound
      break;

    default:
//      Serial.print("Unrecognized Code: 0x");  // Debugging message
      delay(200);  // 200 ms delay to prevent continuous looping
      //IrReceiver.resume();  // Prepare to receive the next signal 
      break;
    }
}