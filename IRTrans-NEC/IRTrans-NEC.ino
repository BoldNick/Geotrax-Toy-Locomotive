#include <IRremote.h>

const byte butonPin = 2; //button input
const byte IRPin = 1; // LED output

void setup()
{
   // connect button to ground and input
   pinMode(butonPin, INPUT_PULLUP);

   IrSender.begin(IRPin);
}

void loop()
{
   static bool lastButtonState = HIGH;
   bool buttonState = digitalRead(butonPin);
   if (buttonState != lastButtonState)
   {
      if (buttonState == LOW)
      {
         IrSender.sendNEC(0xED12BF40, 0x12, 0);
      }
      lastButtonState = buttonState;
   }
}