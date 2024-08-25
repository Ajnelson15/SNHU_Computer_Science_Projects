/*
* Andrew Nelson
* CS350_Thermostat Project
*
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



/* Standard Library Headers */
#include <stdint.h>  // Provides fixed-width integer types
#include <stddef.h>  // Provides size_t and NULL
#include <stdio.h>   // Provides snprintf for formatting strings

/* Driver Header Files */
#include <ti/drivers/GPIO.h>  // For GPIO control (LED and buttons)
#include <ti/drivers/I2C.h>   // For I2C communication with temperature sensor
#include <ti/drivers/UART.h>  // For UART communication
#include <ti/drivers/Timer.h> // For periodic task scheduling

/* Driver Configuration */
#include "ti_drivers_config.h"  // Board-specific configuration

/* Macro for easier UART output. */
#define DISPLAY(x) UART_write(uart, &output, x);

/* Tasks Structure*/
typedef struct task {
    int state;                  // Current state of the task's state machine
    unsigned long period;       // Period of the task in milliseconds
    unsigned long elapsedTime;  // Time elapsed since last tick of this task
    int (*TickFct)(int);        // Pointer to the task's tick function
} task;

/* Task Scheduler Timing Setup */
#define TASK_NUM 3                  // Number of tasks in the scheduler
#define TASK_PERIOD_GCD 100         // Greatest Common Divisor of all task periods in milliseconds
#define BUTTON_CHECK_PERIOD 200     // Period for checking button presses in milliseconds
#define TEMP_CHECK_PERIOD 500       // Period for checking temperature in milliseconds
#define SERVER_UPDATE_PERIOD 1000   // Period for updating server in milliseconds

/* Global Variables */
volatile int setpoint = 20;                     // Desired temperature in Celsius
volatile int currentTemp = 0;                   // Current temperature in Celsius
volatile unsigned long secondsSinceReset = 0;   // Time since last reset in seconds
volatile unsigned char TimerFlag = 0;           // Flag for timer interrupts, set in timerCallback

/* Function Declarations */
void timerCallback(Timer_Handle myHandle, int_fast16_t status);
void initTimer(void);
void initUART(void);
void initI2C(void);
void initGPIO(void);
int16_t readTemp(void);
void gpioButtonFxn(uint_least8_t index);

/* Button States for the button checking task's state machine*/
enum ButtonStates { BUTTON_INIT, BUTTON_WAIT, BUTTON_INCREASE, BUTTON_DECREASE };
int TickFct_ButtonCheck(int state);

/* Temperature States for the temperature checking task's state machine */
enum TempStates { TEMP_INIT, TEMP_CHECK };
int TickFct_TempCheck(int state);

/* Server Update States for the server update task's state machine */
enum ServerStates { SERVER_INIT, SERVER_UPDATE };
int TickFct_ServerUpdate(int state);

/* Global Handles */
Timer_Handle timer0;  // Handle for the Timer used in task scheduling
UART_Handle uart;     // Handle for UART communication
I2C_Handle i2c;       // Handle for I2C communication

/* I2C Global Variables */
static const struct {
    uint8_t address;    // I2C address of the sensor
    uint8_t resultReg;  // Register to read for temperature data
    char *id;           // Sensor ID string for identification
} sensors[3] = {
    { 0x48, 0x0000, "11X" },
    { 0x49, 0x0000, "116" },
    { 0x41, 0x0001, "006" }
};
uint8_t txBuffer[1];        // Buffer for I2C transmission (sending commands to sensor)
uint8_t rxBuffer[2];        // Buffer for I2C reception (receiving data from sensor)
I2C_Transaction i2cTransaction;  // Structure to hold details of I2C transaction

/* UART Global Variables */
char output[64];    // Buffer for UART output messages
int bytesToSend;    // Number of bytes to send over UART

/* Callback function for timer interrupts */
void timerCallback(Timer_Handle myHandle, int_fast16_t status) {
    TimerFlag = 1;  // Set the timer flag to indicate an interrupt has occurred
}

// Initialize the timer for task scheduling
void initTimer(void) {
    Timer_Params params;

    // Initialize the Timer driver
    Timer_init();

    // Initialize the Timer parameters
    Timer_Params_init(&params);
    params.period = TASK_PERIOD_GCD * 1000;  // Convert milliseconds to microseconds
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    // Open and start the timer
    timer0 = Timer_open(CONFIG_TIMER_0, &params);
    if (timer0 == NULL || Timer_start(timer0) == Timer_STATUS_ERROR) {
        while (1) {}  // If timer fails to start, enter infinite loop
    }
}

// Initialize UART for communication
void initUART(void) {
    UART_Params uartParams;

    // Initialize the UART driver
    UART_init();

    // Configure the UART parameters
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.baudRate = 115200;  // Set baud rate to 115200

    // Open the UART driver
    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL) {
        while (1);  // If UART fails to open, enter infinite loop
    }
}

// Initialize I2C for communication with temperature sensor
void initI2C(void) {
    int8_t i, found;
    I2C_Params i2cParams;

    DISPLAY(snprintf(output, 64, "Initializing I2C Driver - "))

    // Initialize the I2C driver
    I2C_init();

    // Configure the I2C parameters
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    // Open the driver
    i2c = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2c == NULL) {
        DISPLAY(snprintf(output, 64, "Failed\n\r"))
        while (1);  // If I2C fails to open, enter infinite loop
    }
    DISPLAY(snprintf(output, 32, "Passed\n\r"))

    // Boards were shipped with different sensors.
    // Welcome to the world of embedded systems.
    // Try to determine which sensor we have.
    // Scan through the possible sensor addresses

    /* Common I2C transaction setup */
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 0;
    found = false;

    // Try to detect which temperature sensor is present
    // This loop attempts to communicate with each known sensor type
    for (i = 0; i < 3; ++i) {
        i2cTransaction.slaveAddress = sensors[i].address;
        txBuffer[0] = sensors[i].resultReg;
        DISPLAY(snprintf(output, 64, "Is this %s? ", sensors[i].id))
        if (I2C_transfer(i2c, &i2cTransaction)) {
            DISPLAY(snprintf(output, 64, "Found\n\r"))
            found = true;
            break;
        }
        DISPLAY(snprintf(output, 64, "No\n\r"))
    }

    if (found) {
        DISPLAY(snprintf(output, 64, "Detected TMP%s I2C address: %x\n\r", sensors[i].id, i2cTransaction.slaveAddress))
    } else {
        DISPLAY(snprintf(output, 64, "Temperature sensor not found, contact professor\n\r"))
    }
}

// Read temperature from the sensor
int16_t readTemp(void) {
    int16_t temperature = 0;

    i2cTransaction.readCount = 2;
    if (I2C_transfer(i2c, &i2cTransaction)) {
        /*
        * Extract degrees C from the received data;
        * see TMP sensor datasheet
        */
        temperature = (rxBuffer[0] << 8) | (rxBuffer[1]);
        temperature *= 0.0078125;  // Convert to Celsius

        /*
        * If the MSB is set '1', then we have a 2's complement
        * negative value which needs to be sign extended
        */
        if (rxBuffer[0] & 0x80) {
            temperature |= 0xF000;
        }
    } else {
        // If the I2C fails, display an error message
        DISPLAY(snprintf(output, 64, "Error reading temperature sensor (%d)\n\r", i2cTransaction.status))
        DISPLAY(snprintf(output, 64, "Please power cycle your board by unplugging USB and plugging back in.\n\r"))
    }
    return temperature;
}

// GPIO button callback function this function is called when a button interrupt occurs
void gpioButtonFxn(uint_least8_t index) {
    if (index == CONFIG_GPIO_BUTTON_0) {
        setpoint++;  // Increase setpoint when button 0 is pressed
    } else if (index == CONFIG_GPIO_BUTTON_1) {
        setpoint--;  // Decrease setpoint when button 1 is pressed
    }
}

// Initialize GPIO for LED and buttons
void initGPIO(void) {
    // Initialize the GPIO driver
    GPIO_init();

    // Configure LED and buttons
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    // Set initial LED state to off
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);

    // Set up button callbacks
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn);
    GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn);

    // Enable button interrupts
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
}

// This function implements the state machine for checking button presses
int TickFct_ButtonCheck(int state) {
    switch (state) {
        case BUTTON_INIT:
            // Initialize button state
            state = BUTTON_WAIT;
            break;
        case BUTTON_WAIT:
            // Wait for button press
            if (GPIO_read(CONFIG_GPIO_BUTTON_0)) {
                state = BUTTON_INCREASE;
            } else if (GPIO_read(CONFIG_GPIO_BUTTON_1)) {
                state = BUTTON_DECREASE;
            }
            break;
        case BUTTON_INCREASE:
            // Increase setpoint if not at maximum
            if (setpoint < 99) setpoint++;
            state = BUTTON_WAIT;
            break;
        case BUTTON_DECREASE:
            // Decrease setpoint if not at minimum
            if (setpoint > 0) setpoint--;
            state = BUTTON_WAIT;
            break;
    }
    return state;
}

// This function implements the state machine for checking temperature
int TickFct_TempCheck(int state) {
    switch (state) {
        case TEMP_INIT:
            // Initialize temperature check state
            state = TEMP_CHECK;
            break;
        case TEMP_CHECK:
            // Read current temperature
            currentTemp = readTemp();
            state = TEMP_CHECK;
            break;
    }
    return state;
}

// This function implements the state machine for updating the server
int TickFct_ServerUpdate(int state) {
    int heat; // Variable to store heating status

    switch (state) {
        case SERVER_INIT:
            // Initialize server update state
            state = SERVER_UPDATE;
            break;
        case SERVER_UPDATE:
            // Determine if heating is needed
            if (currentTemp < setpoint) {
                heat = 1;  // Heating is needed
            } else {
                heat = 0;  // Heating is not needed
            }

            // Set LED based on heating status
            if (heat == 1) {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
            } else {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
            }

            // Send status update via UART
            DISPLAY(snprintf(output, 64, "<%02d,%02d,%d,%04d>\n\r", currentTemp, setpoint, heat, secondsSinceReset))

            secondsSinceReset++;
            state = SERVER_UPDATE;
            break;
    }
    return state;
}

// Main thread function
void* mainThread(void* arg0) {
    task tasks[TASK_NUM];
    int i;

    // Initialize tasks
    tasks[0].state = BUTTON_INIT;
    tasks[0].period = BUTTON_CHECK_PERIOD;
    tasks[0].elapsedTime = 0;
    tasks[0].TickFct = &TickFct_ButtonCheck;

    tasks[1].state = TEMP_INIT;
    tasks[1].period = TEMP_CHECK_PERIOD;
    tasks[1].elapsedTime = 0;
    tasks[1].TickFct = &TickFct_TempCheck;

    tasks[2].state = SERVER_INIT;
    tasks[2].period = SERVER_UPDATE_PERIOD;
    tasks[2].elapsedTime = 0;
    tasks[2].TickFct = &TickFct_ServerUpdate;

    // Initialize hardware
    initGPIO();
    initUART();
    initI2C();
    initTimer();

    // Main loop
        while (1) {
            // Check each task
            for (i = 0; i < TASK_NUM; i++) {
                // If it's time for a task to run
                if (tasks[i].elapsedTime >= tasks[i].period) {
                    // Run the task's tick function and update its state
                    tasks[i].state = tasks[i].TickFct(tasks[i].state);
                    // Reset the task's elapsed time
                    tasks[i].elapsedTime = 0;
                }
                // Increment the task's elapsed time
                tasks[i].elapsedTime += TASK_PERIOD_GCD;
            }

            // Wait for timer flag
            while (!TimerFlag) {
            }
            TimerFlag = 0;  // Reset the timer flag
        }
    }
