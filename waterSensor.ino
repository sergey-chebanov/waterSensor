// **** INCLUDES *****
#include "LowPower.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"


RF24 radio(9,10);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setupRadio () {

    radio.begin();
    radio.setChannel(68);
    radio.setPALevel(RF24_PA_HIGH);
    radio.setRetries(15,15);
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);

    //dump
    radio.printDetails();
}


bool sendData (unsigned long data) {

    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    printf("Now sending %lu...", data);
    bool ok = radio.write( &data, sizeof(unsigned long) );

    if (ok)
        printf("ok...");
    else
        printf("failed.\n\r");

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
        if (millis() - started_waiting_at > 200 )
            timeout = true;

    // Describe the results
    if ( timeout )
    {
        printf("Failed, response timed out.\n\r");
    }
    else
    {
        // Grab the response, compare, and send to debugging spew
        unsigned long got_time;
        radio.read( &got_time, sizeof(unsigned long) );

        // Spew it
        printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
    }

    return ok;
}


// Use pin 2 as wake up pin
const int wakeUpPin = 2;
volatile unsigned long waterSignalCount = 0;
unsigned long sendedCounter = 0xFFFFFFFF;

void wakeUp()
{
    // Just a handler for the pin interrupt.
    waterSignalCount ++;
    printf("wakeUp: %lu\n", waterSignalCount);

}

void setup()
{
    Serial.begin(57600);
    printf_begin();

    setupRadio ();
    // Configure wake up pin as input.
    // This will consumes few uA of current.
    pinMode(wakeUpPin, INPUT);
    // Allow wake up pin to trigger interrupt on low.
}

void loop()
{
    attachInterrupt(0, wakeUp, RISING);

    printf("Before powerDown\n");

    //deactivating radio
    radio.powerDown();

    // Enter power down state with ADC and BOD module disabled.
    // Wake up when wake up pin is low.
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    //delay (1000);
    //delay(2000);

    // Disable external pin interrupt on wake up pin.
    detachInterrupt(0);


    //activating radio
    radio.powerUp();
    //waiting
    delay(200);

    // Do something here
    // Example: Read sensor, data logging, data transmission.

    printf("After delay\n");
    //Serial.flush();

    if (sendedCounter != waterSignalCount) {
        printf("Sending waterLevel: %lu\n", waterSignalCount);
        if (sendData(waterSignalCount)) {
            sendedCounter = waterSignalCount;
            printf("Saved waterLevel: %lu\n", sendedCounter);
        }
    }

    //delay(1000);
}
