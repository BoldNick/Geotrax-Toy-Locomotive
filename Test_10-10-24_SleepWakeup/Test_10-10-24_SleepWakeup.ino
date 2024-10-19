#include <IRremote.hpp>
//#include <toneAC.h>
#include <LowPower.h>
#include <avr/wdt.h>  // Include this library to control the watchdog


// Pins
constexpr uint8_t RECV_PIN {7};      // Pin for IR receiver
constexpr uint8_t LED_PIN {8};       // Pin for LED
constexpr uint8_t BUTTON_PIN {2};    // Pin for wake-up button

// Variables for activity tracking
constexpr uint32_t inactivityTimeout = 10000; // 10 seconds inactivity timeout
unsigned long lastActivityTime = 0;

// Debounce timing for the button interrupt
unsigned long lastInterruptTime = 0;

void wakeUp() {
  unsigned long interruptTime = millis();
  
  // Debounce the interrupt signal
  if (interruptTime - lastInterruptTime > 200) {
    Serial.println("Button pressed, waking up...");
  }
  
  lastInterruptTime = interruptTime;
}

void setup() {
  
  // Disable the watchdog timer at the beginning
  wdt_disable();
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);   
  pinMode(BUTTON_PIN, INPUT_PULLUP);  

  // Initialize IR receiver
  IrReceiver.begin(RECV_PIN);
  
  // Attach interrupt to wake up Arduino when button is pressed (falling edge)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeUp, FALLING);

  lastActivityTime = millis(); 
}

void loop() {
    // Ensure watchdog is still disabled before entering sleep mode
  wdt_disable();
  
  uint16_t receivedCode = 0;

  if (IrReceiver.decode()) {
    receivedCode = IrReceiver.decodedIRData.command;
    Serial.print("Command received: 0x");
    Serial.println(receivedCode, HEX);
    IrReceiver.resume();
    lastActivityTime = millis();
  }

  // If no IR code is received for the timeout, enter sleep mode
  if (millis() - lastActivityTime >= inactivityTimeout) {
    Serial.println(F("Entering Sleep Mode..."));
    delay(100);  // Allow time for the message to be sent

    // Ensure no ongoing actions (motor, sound)
   // noToneAC();
    digitalWrite(LED_PIN, LOW);

    // Disable IR interrupts before sleep
    IrReceiver.disableIRIn();

    noInterrupts();  // Disable all interrupts
      // Inserisci il ciclo infinito qui, per evitare risvegli indesiderati
   // while (1) {
        // Entra in modalità sleep fino a quando non viene premuto il pulsante di risveglio
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
   // }
    interrupts();    // Re-enable interrupts after waking up

    // Reinitialize peripherals after waking up
    Serial.begin(9600);
    IrReceiver.enableIRIn();
    
    Serial.println(F("Waking Up..."));
    lastActivityTime = millis();
  }
}
