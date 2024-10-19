/*
 * Light Weight NEC Infrared decoder using interrupt instead of timer.
 *
 * 04_06_2019 v0_05 namespace model
 *
 */

#include "IrNec.h"


void setup() {
  Serial.begin(9600) ;
  Serial.println();
  Serial.println(F("IR v0_01")) ;

  // Set to the pin number of an external interrupt pin and connect the IR receiver to it.
  // Uno/Nano etc. : pin2 or pin3
  // ATTINY841 : pin1 (PB1)
  // ESP8266 : Any GPIO pin apart from GPIO16
  // see attachInterrupt() for other examples and more details
  
  nsIrNec::begin(11) ;  // MUST BE EXTERNAL INTERRUPT PIN 
}

void loop() {

  nsIrNec::loop() ;   // check for new data

  if ( nsIrNec::dataOut != 0 ) {
    Serial.println();

    // print raw data
    for ( uint8_t i = 0 ; i < 32 ; i++ )   {
      Serial.print( bitRead( nsIrNec::dataRaw, i ) ) ;
      if ( (i+1) % 8 == 0  ) Serial.print(' ' ) ;
    }
    Serial.println();

    // print interpreted data then reset it
    Serial.println(nsIrNec::dataOut,HEX) ;
    Serial.println();
    nsIrNec::dataOut = 0 ; //clear
  }
}

