// **** INCLUDES *****
#include "LowPower.h"


// Use pin 2 as wake up pin
const int wakeUpPin = 2;
volatile int waterSignalCount = 0;

void wakeUp()
{
    // Just a handler for the pin interrupt.
    waterSignalCount ++;
    Serial.println("wakeUp");
    Serial.flush();
}

void setup()
{
    Serial.begin(57600);
    // Configure wake up pin as input.
    // This will consumes few uA of current.
    pinMode(wakeUpPin, INPUT);   
    // Allow wake up pin to trigger interrupt on low.
}

void loop() 
{
    attachInterrupt(0, wakeUp, RISING);

    // Enter power down state with ADC and BOD module disabled.
    // Wake up when wake up pin is low.
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    //delay (1000);
    
    // Disable external pin interrupt on wake up pin.
    detachInterrupt(0);
    
    // Do something here
    // Example: Read sensor, data logging, data transmission.

 
    Serial.print("waterLevel: "); 
    Serial.println(waterSignalCount);\
    Serial.flush();
}
