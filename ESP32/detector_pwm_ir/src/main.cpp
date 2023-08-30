#include <Arduino.h>
#include <stdio.h>

#include "SSD1306Wire.h"

const int IR_PIN = 25; // GPIO25 = D25
const int PWM_PIN = 26; // GPIO26 = D26
const int LED_BUILDIN = 2;

unsigned long startTime;
unsigned long endTime;
unsigned long pulseLength;

volatile unsigned long counterPWM = 0;
volatile unsigned long counterIR = 0;
unsigned long frequency = 0;
int lastMillis = 0;
char strBuf[50];

hw_timer_t *timer = NULL;

// Initialize the OLED display
// SDA -> D21
// SCL -> D22
SSD1306Wire Display(0x3c, 21, 22);

void displayMessage();

void IRAM_ATTR PWMInterrupt() {
    if (digitalRead(PWM_PIN) == HIGH) {
        startTime = micros();
    } else {
        endTime = micros();
        pulseLength = endTime - startTime;
        counterPWM++;
    }
}

void IRAM_ATTR IRInterrupt() {
    counterIR++;
}

void IRAM_ATTR onTimer() {
    frequency = counterPWM;
    counterPWM = 0;
}

void setup() {
    Serial.begin(460800);

    Display.init();
    Display.flipScreenVertically();

    pinMode(LED_BUILDIN, OUTPUT);
    pinMode(IR_PIN, INPUT_PULLUP);
    pinMode(PWM_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(PWM_PIN), PWMInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(IR_PIN), IRInterrupt, RISING);

    timer = timerBegin(0, 80, true); // timer 0, prescaler, counter up
    timerAlarmWrite(timer, 1000000, true); // after this number of time the interrupt isr is called and counter reseted
    timerAttachInterrupt(timer, &onTimer, true); // attach interrupt handler, edge true
    timerAlarmEnable(timer); // enable if

    lastMillis = millis();

    interrupts();
}

void loop() {
    if (millis() - lastMillis > 100) {
        displayMessage();
        lastMillis = millis();
    }
}

void displayMessage() {
    Display.clear();
    Display.setTextAlignment(TEXT_ALIGN_LEFT);
    Display.setFont(ArialMT_Plain_16);

    sprintf(strBuf, "%05d us %03d Hz", pulseLength, frequency);
    Display.drawString(0, 5, String(strBuf));

    sprintf(strBuf, "%05d RPS %0.2f", counterIR, 1.0F);
    Display.drawString(0, 25, String(strBuf));

    sprintf(strBuf, "%05d RPM %c", 60 * counterIR, 'F');
    Display.drawString(0, 45, String(strBuf));

    Display.display();
}
