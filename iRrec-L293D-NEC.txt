#define DECODE_NEC      // Enable decoding of NEC protocol
#include <IRremote.hpp> // Include InfraRed library
#include <LowPower.h>   // Include LowPower library for sleep functionality
#include <avr/wdt.h>    // Include this library to control the watchdog

// Pins
constexpr uint8_t RECV_PIN {7};      // Pin for IR receiver
constexpr uint8_t LED_PIN {8};       // Pin for LED
constexpr uint8_t BUTTON_PIN {2};    // Pin for wake-up button
constexpr uint8_t BUZZER_PIN {9};    // Pin for Sound

// IR Codes
constexpr uint16_t CODE_FORWARD = 0x45;  // NEC IR Code for Move Forward
constexpr uint16_t CODE_STOP = 0x46;     // NEC IR Code for Stop
constexpr uint16_t CODE_SOUND = 0x47;    // NEC IR Code for Sound
constexpr uint16_t CODE_BACKWARD = 0x44; // NEC IR Code for Move Backward

// Pins for the L293D Motor Driver
constexpr uint8_t IN1 {3};  // Pin for IN1 (spin)
constexpr uint8_t IN2 {4};  // Pin for IN2 (spin)
constexpr uint8_t ENA {5};  // Pin for ENA (speed)

// Variables to manage "stand-by" functionality
constexpr uint32_t inactivityTimeout = 10000; // 10 seconds inactivity timeout
unsigned long lastActivityTime = 0;           // Keeps track of the last time an IR code was received

// Debounce timing for the button interrupt
unsigned long lastInterruptTime = 0;

void wakeUp() {            // Interrupt Service Routine (ISR) for waking up from sleep
  //***unsigned long interruptTime = millis();
  // This function will be called automatically by the interrupt (button press)
  // Debounce the interrupt signal
  Serial.println("Inside wakeUp()...");
 // if (interruptTime - lastInterruptTime > 200) {
    Serial.println("Button pressed, waking up...");
 // }
  //***lastInterruptTime = interruptTime;

}

uint16_t irReceive() {
  uint16_t received {0};
  Serial.println("Inside irReceive()...");
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    if (IrReceiver.decodedIRData.protocol == NEC) {
      received = IrReceiver.decodedIRData.command;
      Serial.print("Command: 0x");
      Serial.println(received, HEX);
    }
    IrReceiver.resume();  // Prepare to receive the next signal
  }
  return received;
}

// Function to generate sound without using tone() and toneAC() that create problems with interrupts. This one is with analogWrite
void playTone(int frequency, int duration) {
  int period = 1000000 / frequency;          // Calculates the period in milliseconds
  int halfPeriod = period / 2;               // Calculates the half of the period (for a square wave)

  unsigned long startTime = millis();
  
  while (millis() - startTime < duration) {
    analogWrite(BUZZER_PIN, 128);            // Set the duty cycle at 50% (square wave)
    delayMicroseconds(halfPeriod);           // Wait half of the period
    analogWrite(BUZZER_PIN, 0);              // Turn off the PWM signal
    delayMicroseconds(halfPeriod);           // Wait for the second half of the period
  }
}


void setup() {
    // Disable the watchdog timer at the beginning
  Serial.println("Inside setup()... next step: disable watchdog");  
  wdt_disable();
  Serial.println("Inside setup()... next step: serial.begin(9600)");  
  Serial.begin(9600);

  // Pin for the Motor "ON"; Set the Pin as output
  pinMode(LED_PIN, OUTPUT);           // Set up LED pin as output
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Set up the Button Pin as "Pull-up" mode and "internal resistor" mode
  pinMode(BUZZER_PIN, OUTPUT);        // Imposta il pin del buzzer come uscita
 
  // Pins for the Motor Driver L293D: set all the Pins as "outputs"
  pinMode(IN1, OUTPUT);               // Motor driver pin
  pinMode(IN2, OUTPUT);               // Motor driver pin
  pinMode(ENA, OUTPUT);               // Motor speed pin

  // Pin for the wake up Button: set the Pin as "input"
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Set button pin as input with internal pull-up

  // Initialize all the outputs turned off
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);               // Motor Stop
  digitalWrite(LED_PIN, LOW);        // LED turned off

  // Start IR receiver on the specified pin
  Serial.println("Inside setup()... next step: Start IR receiver)");
  IrReceiver.begin(RECV_PIN);
  Serial.print(F("Inside setup()... Ready to receive IR signals at pin "));
  Serial.println(RECV_PIN);

  // Attach interrupt to wake up Arduino when the button is pressed (falling edge)
  Serial.println("Inside setup()... next step: attachInterrupt");
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeUp, FALLING);

  // Initialize the inactivity timer
  lastActivityTime = millis(); 

}

void loop() {

  // Ensure watchdog is still disabled before entering sleep mode
  wdt_disable();

  // Check for IR code and reset activity timer if an IR code is received
  uint16_t receivedCode = 0;

  if (IrReceiver.decode()) {
    receivedCode = IrReceiver.decodedIRData.command;
    Serial.print("Command received: 0x");
    Serial.println(receivedCode, HEX);
    IrReceiver.resume();
    lastActivityTime = millis();
 // Control logic based on received IR code
    switch (receivedCode) {
      case CODE_FORWARD:
        Serial.println("Move Forward");
        digitalWrite(IN1, HIGH);     // Move forward
        digitalWrite(IN2, LOW);      // IN2 deactivated
        analogWrite(ENA, 128);       // Medium speed (max = 255)
        digitalWrite(LED_PIN, HIGH); // Turn on LED
        break;

      case CODE_STOP:
        Serial.println("Stop");
        digitalWrite(IN1, LOW);     // Stop motor
        digitalWrite(IN2, LOW);     // Stop motor
        analogWrite(ENA, 0);        // Turn off motor
        digitalWrite(LED_PIN, LOW); // Turn off LED
        break;

      case CODE_BACKWARD:
        Serial.println("Move Backward");
        digitalWrite(IN1, LOW);      // Stop motor
        digitalWrite(IN2, HIGH);     // Reverse motor
        analogWrite(ENA, 128);       // Medium speed
        digitalWrite(LED_PIN, HIGH); // Turn on LED
        break;

     case CODE_SOUND:
       Serial.println("Sound");
       playTone(2000, 500);  // Riproduci un suono di 2000 Hz per 500 ms
       delay(1000);          // Attendi 1 secondo prima di ripetere
       break;

      default:
        delay(200);  // 200 ms delay to prevent continuous looping
        break;
    }

   }
   
   // If no IR code is received for the timeout, enter sleep mode
   if (millis() - lastActivityTime >= inactivityTimeout) {
    Serial.println(F("Inside loop: Entering Sleep Mode..."));
    delay(100);  // Allow time for the message to be sent

  
    // Disable IR interrupts before sleep
    IrReceiver.disableIRIn();
    // Ensure no ongoing actions (motor, sound)
    Serial.println(F("3 - Stopping Motor, LEDs,..."));
    digitalWrite(LED_PIN, LOW);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);  // Stop motor
    
    // Disable interrupts briefly before sleep
    //***Serial.println(F("4 - Disabling Interrupts..."));
    //***noInterrupts();
  
        // Entra in modalità sleep fino a quando non viene premuto il pulsante di risveglio
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    
    // Re-enable interrupts after waking up
    //***Serial.println(F("A - Waking up, enabling Interrupts..."));
    //***interrupts();    // Re-enable interrupts after waking up
   //*** Serial.println(F("B - Interrupts Enabled..."));
    // Reinitialize peripherals after waking up
    Serial.println(F("C - Waking up, Starting Serial Communications..."));
    Serial.begin(9600);
    IrReceiver.enableIRIn();
    delay(100);  // Short delay for Serial to initialize
    Serial.println(F("D - Waking Up,, enabling IR..."));
    IrReceiver.begin(RECV_PIN); //reinitialize the IRreceiver
    Serial.println(F("E - Waking up, IR Enabled..."));

   
    
    // Reset the activity time after waking up
    lastActivityTime = millis();
  }

  }
