/**
 * @(#) main.cpp
 *
 * Copyright (c) 2023 UVSBR PROJETOS DE SISTEMAS LTDA.
 *
 * This software is the confidential and proprietary information of
 * UVSBR PROJETOS DE SISTEMAS LTDA.("Confidential Information").
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement you entered
 * into with Antheus Tecnologia Ltda.
 */

 /**
  * A firmware to Propulsion Failure Detection of Unmanned Aerial Vehicles.
  *
  * @author: Omar Achraf
  * @version: 10/jully/2023 11H01
  */

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <vector>

#include <RingBuffer.h>
#include <CriticalSection.h>
#include <Correlation.h>

#include "SSD1306Wire.h"



const int IR_PIN = 25; // GPIO25 = D25
const int PWM_PIN = 26; // GPIO26 = D26
const int LED_BUILDIN = 2;
const int NUM_BLADES = 2; // Number of propeller blades
const int NUM_SAMPLE_PER_SEC = 10; // Number of Samples per second. Must be power of 10
const int SEC_SAMPLES = 3; // Number of seconds for sampling
const int SAMPLES_BUFFER_SIZE = NUM_SAMPLE_PER_SEC * SEC_SAMPLES;

volatile unsigned long pulseWidthPWMIRAM = 0;
volatile unsigned long pulseWidthPWM = 0;
volatile unsigned long counterPWM = 0;
volatile unsigned long counterIR = 0;

volatile unsigned long frequencyPWM = 0;
volatile unsigned long frequencyIR = 0;

volatile unsigned long counterTimer = 0;

volatile float rho = 1.0f;
volatile bool isFailure = false;

int lastMillis = 0;
char strBuf[50];


TaskHandle_t taskPWMHandler = NULL; // Read PWM signal and transform to pulse width
TaskHandle_t taskIRHandler = NULL; // Read pulses from Infra Read sensor
TaskHandle_t taskTimerHandler = NULL; // Transform values and correlate PWM x IR Frequency
TaskHandle_t taskFailureHandler = NULL; // Observer for failure flag and signal Fail
TaskHandle_t taskDisplayHandler = NULL; // Display for debugging

// Protótipos
void taskPWM(void *parameter);
void taskIR(void *parameter);
void taskTimer(void *parameter);
void taskFailure(void *parameter);
void taskDisplay(void *parameter);

// Timer interrup handler
hw_timer_t *timer = NULL;

// Initialize the OLED display
// SDA -> D21
// SCL -> D22
SSD1306Wire Display(0x3c, 21, 22);

CriticalSection CS;

RingBuffer<unsigned long, SAMPLES_BUFFER_SIZE> bufferPWM(0L, true); // create buffer ring fullfilled
RingBuffer<unsigned long, SAMPLES_BUFFER_SIZE> bufferRPS(0L, true); // create buffer ring fullfilled

std::vector<unsigned long> valuesPWM(SAMPLES_BUFFER_SIZE);
std::vector<unsigned long> valuesRPS(SAMPLES_BUFFER_SIZE);

Correlation<unsigned long, SAMPLES_BUFFER_SIZE> correlations;

void displayMessage();
void flagFailure(bool isOn);

void IRAM_ATTR PWMInterrupt() {
    vTaskResume(taskPWMHandler);
}

void IRAM_ATTR IRInterrupt() {
    vTaskResume(taskIRHandler);
}

void IRAM_ATTR onTimer() {
    vTaskResume(taskTimerHandler);
}

void flagFailure(bool isOn) {
    digitalWrite(LED_BUILDIN, isOn? HIGH : LOW); 
}

void taskPWM(void *parameter) {
    static unsigned long startTime = 0;
    static unsigned long endTime = 0;

    while (true) {
        vTaskSuspend(NULL);

        if (digitalRead(PWM_PIN) == HIGH) {
            startTime = micros();
            //Serial.print("+");
        } else {
            endTime = micros();
            //Serial.print("-");

            CS.enter();
            pulseWidthPWMIRAM = endTime - startTime;
            counterPWM++;
            //Serial.print(".");
            CS.exit();
        }    
    }
}

void taskIR(void *parameter) {
    while (true) {
        vTaskSuspend(NULL);

        CS.enter();

        //Serial.print(".");

        counterIR++;
        CS.exit();
    }
}

void taskTimer(void *parameter) {
    while (true) {
        vTaskSuspend(NULL);

        CS.enter();

        // copy values to thread save variables
        pulseWidthPWM = pulseWidthPWMIRAM;
        frequencyPWM = counterPWM * NUM_SAMPLE_PER_SEC;
        frequencyIR = counterIR * NUM_SAMPLE_PER_SEC;

        // Insert in buffer lists
        bufferPWM.get(); // free a slot
        bufferRPS.get(); // free a slot

        bufferPWM.put(pulseWidthPWM);
        bufferRPS.put(frequencyIR);

        // reset frequency counters
        counterPWM = 0;
        counterIR = 0;

        // samples collected, determine rho
        if (counterTimer % SAMPLES_BUFFER_SIZE == 0) {
            // copy ring buffers to linear buffers
            bufferPWM.copyTo(valuesPWM);
            bufferRPS.copyTo(valuesRPS);

            rho = correlations.spearman(valuesPWM, valuesRPS);
            //rho = correlations.pearson(valuesPWM, valuesRPS);

            if (std::abs(rho) < 0.75f) {
                isFailure = true;
            } else {
                isFailure = false;
            }           
        }

        CS.exit();

        counterTimer++;
    }
}

void taskFailure(void *parameter) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(100));

        if (isFailure) {
            flagFailure(true);
        } else {
            flagFailure(false);
        }
    }
}

void taskDisplay(void *parameter) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(100));
        
        displayMessage();
    }
}

void setup() {
    // Hardware setup

    Serial.begin(460800);

    Display.init();
    Display.flipScreenVertically();

    pinMode(LED_BUILDIN, OUTPUT);
    pinMode(IR_PIN, INPUT_PULLUP);
    pinMode(PWM_PIN, INPUT_PULLUP);

    flagFailure(false);

    // Task creation

    UBaseType_t priority = 2;
    xTaskCreatePinnedToCore(taskPWM, "PWM Task", 2048, NULL, priority, &taskPWMHandler, 1);
    xTaskCreatePinnedToCore(taskIR, "IR Task", 1024, NULL, priority, &taskIRHandler, 1);
    xTaskCreatePinnedToCore(taskTimer, "Timer Task", 2048, NULL, priority, &taskTimerHandler, 1);
    xTaskCreatePinnedToCore(taskFailure, "Failure Task", 1024, NULL, priority, &taskFailureHandler, 1);
    xTaskCreatePinnedToCore(taskDisplay, "Display Task", 2048, NULL, priority, &taskDisplayHandler, 1);

    // Interrupt creation

    attachInterrupt(digitalPinToInterrupt(PWM_PIN), PWMInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(IR_PIN), IRInterrupt, RISING);

    timer = timerBegin(0, 80, true); // timer 0, prescaler, counter up
    timerAlarmWrite(timer, 1000000 / NUM_SAMPLE_PER_SEC, true); // after this number of time the interrupt isr is called and counter reseted
    timerAttachInterrupt(timer, &onTimer, true); // attach interrupt handler, edge true
    timerAlarmEnable(timer); // enable if

    lastMillis = millis();

    interrupts();
}

void loop() {
    //if (millis() - lastMillis > 1000) {
    //    displayMessage();
    //    lastMillis = millis();
    //}
}

void displayMessage() {
    static bool heart = false;

    CS.enter();
    unsigned long pwm = pulseWidthPWM;
    unsigned long freqPWM = frequencyPWM;
    unsigned long RPS = frequencyIR / NUM_BLADES; // revolutions per second
    float r = rho;
    CS.exit();

    unsigned long RPM = 60 * RPS;

    char failChar = ' ';

    if (isFailure) {
        failChar = 'F';
    }

    Display.clear();
    Display.setTextAlignment(TEXT_ALIGN_LEFT);
    Display.setFont(ArialMT_Plain_16);

    sprintf(strBuf, "%05d us %03d Hz", pwm, freqPWM);
    Display.drawString(0, 5, String(strBuf));

    sprintf(strBuf, "%05d RPS %0.2f", RPS, r);
    Display.drawString(0, 25, String(strBuf));

    sprintf(strBuf, "%05d RPM   %c", RPM, failChar);
    Display.drawString(0, 45, String(strBuf));

    sprintf(strBuf, "%c", heart? '*' : ' ');
    Display.drawString(120, 45, String(strBuf));

    Display.display();

    //Serial.printf("%05d PWM us  %03d PWM Hz  %05d RPS   %ld counter\n", pwm, freqPWM, RPS, counterTimer);

    // Print PWM and RPS values
    if (isFailure) {
        Serial.printf("\nrho: %3.2f\n", r);

        Serial.printf("PWM: ");
        for (const unsigned long value : valuesPWM) {
            Serial.printf("%00000ld, ", value);
        }
        Serial.println();

        Serial.printf("RPS: ");
        for (const unsigned long value : valuesRPS) {
            Serial.printf("%00000ld, ", value);
        }
        Serial.println();
    }

    heart = !heart;
}
