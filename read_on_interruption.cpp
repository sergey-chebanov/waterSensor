#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cerrno>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <signal.h>


#include <wiringPi.h>
#include <unistd.h>
#include <RF24/RF24.h>

#include "DB.h"

using namespace std;
//
// Hardware configuration
//



// Radio pipe addresses for the 2 nodes to communicate.
//const uint8_t pipes[][6] = {"1Node", "2Node"};
const uint64_t pipes[2] = { 0xF0F0F0F0D2L,0xF0F0F0F0E1L };


typedef enum {
    RolePingOut = 0,
    RolePongBack = 1
} Role;

Role role = RolePongBack;

// Use GPIO Pin 17, which is Pin 0 for wiringPi library

#define BUTTON_PIN 0

// -------------------------------------------------------------------------

mutex m;
condition_variable interruption_cv;
bool stop_reading = false;

void myInterrupt(void) {
    printf("myInterrupt\n");
    interruption_cv.notify_one();
}



//thread worker that actually read
void readPayload() {

    // CE Pin, CSN Pin, SPI Speed
    RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_CLOCK_DIVIDER_32);

    // Setup and configure rf radio
    if (!radio.begin()) {
        printf("RF24 init failed. Are you running as a ROOT?\n");
        return;
    }

    //configure the chip
    radio.setRetries(15,15);
    radio.setChannel(68);
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
    // Dump the configuration of the rf unit for debugging
    radio.printDetails();

    radio.startListening();

    //actuall reading
    while (true) {
        printf("Inside loop\n");

        if (radio.available ()) {
            printf("radio available\n");
            unsigned long payload;

            // Fetch the payload, and see if this was the last one.
            radio.read( &payload, sizeof(unsigned long) );
            radio.stopListening();
            radio.write( &payload, sizeof(unsigned long) );
            // Now, resume listening so we catch the next packets.
            radio.startListening();
            unsigned long cold = payload;

            {
                lock_guard<mutex> lk(m);

                putMeasurement(0, cold);
                // Spew it
                printf("Got payload(%d) %lu...\n",sizeof(unsigned long), cold);
            }
        }
        //waiting for interruption
        unique_lock<mutex> lk(m);
        interruption_cv.wait(lk);
        if (stop_reading)
            return;
    }
}

void signal_callback_handler(int signum)
{
    unique_lock<mutex> lk(m);
    stop_reading = true;
    lk.unlock();
    interruption_cv.notify_one();
}

int main(int argc, char** argv)
{
    // Print preamble:
    printf("Example/read_on_interruption");

    signal(SIGINT, signal_callback_handler);

    thread worker(readPayload);

    // sets up the wiringPi library
    if (wiringPiSetup () < 0) {
        fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
        return 1;
    }

    // set Pin 17/0 generate an interrupt on high-to-low transitions
    // and attach myInterrupt() to the interrupt
    if ( wiringPiISR (BUTTON_PIN, INT_EDGE_FALLING, &myInterrupt) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }

    worker.join();

    return 0;
}
