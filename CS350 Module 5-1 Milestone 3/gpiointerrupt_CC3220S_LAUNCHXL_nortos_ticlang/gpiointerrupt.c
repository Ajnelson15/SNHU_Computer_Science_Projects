/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== gpiointerrupt.c ========
 */
#include <stdint.h>                // Include standard integer types
#include <stddef.h>                // Include standard definitions
#include <ti/drivers/GPIO.h>        // Include TI GPIO driver
#include <ti/drivers/Timer.h>       // Include TI Timer driver
#include "ti_drivers_config.h"      // Include configuration header for TI drivers

/* Enum to define the different states for messages and LEDs */
enum CURRENT_MESSAGES {SOS_MESSAGE, OK_MESSAGE} CURRENT_MESSAGE, BUTTON_STATE;
enum LED_STATES {LED_RED, LED_GREEN, LED_OFF} LED_STATE;

/* Define the Morse code for SOS and OK messages using LED states */
enum LED_STATES sosMessage[] = {
    LED_RED, LED_OFF, LED_RED, LED_OFF, LED_RED, LED_OFF, LED_OFF, LED_OFF,  // SOS: S
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF, LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF, LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF, LED_OFF, LED_OFF, // SOS: O
    LED_RED, LED_OFF, LED_RED, LED_OFF, LED_RED, LED_OFF // SOS: S
};

enum LED_STATES okMessage[] = {
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF, LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF, LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF, LED_OFF, LED_OFF, // OK: O
    LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF, LED_RED, LED_OFF, LED_GREEN, LED_GREEN, LED_GREEN, LED_GREEN, LED_OFF // OK: K
};

unsigned int messageCounter = 0; // Counter to track position in the message array
unsigned int messageDelayCounter = 0; // Counter to track delay between messages
bool messageDelayActive = false; // Flag to indicate if delay is active

/* Function to set the LED state based on the current LED_STATE */
void setLEDs() {
    if (LED_STATE == LED_RED) { // Check if LED_STATE is RED

        // Turn on the red LED and turn off the green LED
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON); // Set red LED on
        GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF); // Set green LED off
        printf("LEDs: RED ON, GREEN OFF\n\n"); // Print status

    } else if (LED_STATE == LED_GREEN) { // Check if LED_STATE is GREEN
        // Turn on the green LED and turn off the red LED
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF); // Set red LED off
        GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON); // Set green LED on
        printf("LEDs: RED OFF, GREEN ON\n\n"); // Print status

    } else { // Otherwise, turn off both LEDs
        // Turn off both LEDs
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF); // Set red LED off
        GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF); // Set green LED off
        printf("LEDs: RED OFF, GREEN OFF\n\n"); // Print status
    }
}

/* Timer callback function */
void timerCallback(Timer_Handle myHandle, int_fast16_t status) {

    // Check if a delay between messages is currently active
    if (messageDelayActive) {
        // Increment the delay counter
        messageDelayCounter++;

        // Check if the delay period (7 * 500ms = 3500ms) has elapsed
        if (messageDelayCounter >= 7) {
            // Reset the delay counter
            messageDelayCounter = 0;
            // Deactivate the delay
            messageDelayActive = false;
            // Switch to the next message (as set by the button press)
            CURRENT_MESSAGE = BUTTON_STATE;
            // Reset the message counter for the new message
            messageCounter = 0;
            printf("Message delay completed. Switching to %d\n", BUTTON_STATE); // Print status

        } else {
            // Ensure both LEDs are off during the delay
            LED_STATE = LED_OFF;
            setLEDs(); // Update the LED states to both LEDs are off
        }

    } else {
        // Process the current message based on its type
        if (CURRENT_MESSAGE == SOS_MESSAGE) {
            // Set LED_STATE to the current state in the SOS message array
            LED_STATE = sosMessage[messageCounter];
            printf("Current message: SOS, LED State: %d, Counter: %d\n", LED_STATE, messageCounter); // Print status
            // Move to the next state in the SOS message array
            messageCounter++;

            // Check if the entire SOS message has been displayed
            if (messageCounter >= sizeof(sosMessage)/sizeof(sosMessage[0])) {
                // Start the delay after completing the SOS message
                messageDelayActive = true;
                printf("\nSOS message completed. Starting delay\n"); // Print status
            }

        } else if (CURRENT_MESSAGE == OK_MESSAGE) {
            // Set LED_STATE to the current state in the OK message array
            LED_STATE = okMessage[messageCounter];
            printf("Current message: OK, LED State: %d, Counter: %d\n", LED_STATE, messageCounter); // Print status
            // Move to the next state in the OK message array
            messageCounter++;

            // Check if the entire OK message has been displayed
            if (messageCounter >= sizeof(okMessage)/sizeof(okMessage[0])) {
                // Start the delay after completing the OK message
                messageDelayActive = true;
                printf("\nOK message completed. Starting delay\n"); // Print status
            }
        }

        // Update the LED states based on the current LED_STATE
        setLEDs();
    }
}

/* Function to initialize and start the timer */
void initTimer(void) {
    Timer_Handle timer0; // Handle for the timer instance
    Timer_Params params; // Structure to hold timer parameters

    Timer_init(); // Initialize the Timer driver
    Timer_Params_init(&params); // Initialize Timer parameters to default values
    params.period = 500000; // Set timer period to 500,000 microseconds (500 ms)
    params.periodUnits = Timer_PERIOD_US; // Period units are microseconds
    params.timerMode = Timer_CONTINUOUS_CALLBACK; // Continuous mode with callback
    params.timerCallback = timerCallback; // Set the callback function for the timer

    timer0 = Timer_open(CONFIG_TIMER_0, &params); // Open and configure Timer

    if (timer0 == NULL) {
        printf("Error: Timer initialization failed\n"); // Print error if timer initialization fails
        while (1) {} // Stay here if Timer initialization fails
    }

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        printf("Error: Timer start failed\n"); // Print error if timer start fails
        while (1) {} // Stay here if Timer start fails
    }
    printf("Timer initialized and started\n"); // Print status
}

/* GPIO button callback function */
void gpioButtonCallback(uint_least8_t index) {
    // Toggle between SOS_MESSAGE and OK_MESSAGE when button is pressed
    if (BUTTON_STATE == SOS_MESSAGE) {
        BUTTON_STATE = OK_MESSAGE; // Switch to OK_MESSAGE
        printf("Button pressed: Switching to OK_MESSAGE\n"); // Print status
    }
    else {
        BUTTON_STATE = SOS_MESSAGE; // Switch to SOS_MESSAGE
        printf("Button pressed: Switching to SOS_MESSAGE\n"); // Print status
    }
}

/* Main thread function */
void *mainThread(void *arg0) {

    GPIO_init(); // Initialize GPIO driver
    initTimer(); // Initialize and start the timer

    // Configure the LEDs as outputs and set their initial states to off
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW); // Set LED 0 as standard output, low initial state
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW); // Set LED 1 as standard output, low initial state
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF); // Set LED 0 off
    GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF); // Set LED 1 off

    // Configure the button as an input with a falling edge interrupt
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING); // Set button 0 as pull-up input with falling edge interrupt
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonCallback); // Set callback for button press
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0); // Enable interrupts for the button

    // If there is a second button, configure it the same
    if (CONFIG_GPIO_BUTTON_0 != CONFIG_GPIO_BUTTON_1) {
        GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING); // Set button 1 as pull-up input with falling edge interrupt
        GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonCallback); // Set callback for button 1 press
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1); // Enable interrupts for button 1
    }

    return NULL;
}
