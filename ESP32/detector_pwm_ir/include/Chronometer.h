/**
 * @(#) Chronometer.h
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
  * Class for nano seconds Chronometer component.
  *
  * @author: Omar Achraf
  * @version: 12/september/2023 18H23
  */

 
#ifndef CHRONOMETER_H

#include <iostream>
#include "freertos/FreeRTOS.h"


#define CHRONOMETER_H

class Chronometer {
private:
    static volatile bool printOn;
    char buffer[50];
    std::string text;

public:
    std::string label;
    uint64_t startTime;
    uint64_t stopTime;

    Chronometer(std::string label) : buffer(""), label(label), startTime(0), stopTime(0) {
    }

    __attribute__((always_inline)) void start() {
        startTime = esp_timer_get_time();
    }

    __attribute__((always_inline)) void stop() {
        stopTime = esp_timer_get_time();
    }

    __attribute__((always_inline)) uint64_t elapsedTime() {
        return stopTime - startTime;
    }

    __attribute__((always_inline)) uint64_t timestamp() {
        return esp_timer_get_time();
    }

    __attribute__((always_inline)) static void setPrintOn(bool isOn) {
        Chronometer::printOn = isOn;
    }

    __attribute__((always_inline)) void printTimestamp(std::string mark, bool isPrintNow = true) {
        if (Chronometer::printOn == false) {
            return;
        }

        std::sprintf(buffer, "%s;%s;%ld\n", label.c_str(), mark.c_str(), esp_timer_get_time());
        std::string message(buffer);
        text += message;

        if (isPrintNow) {
            Serial.printf("%s", text.c_str());
            text = "";
        }
    }
};

bool volatile Chronometer::printOn = true;


#endif 