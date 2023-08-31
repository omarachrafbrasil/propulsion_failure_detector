/**
 * @(#) CriticalSection.h
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
  * Class for crititical section control.
  *
  * @author: Omar Achraf
  * @version: 10/jully/2023 11H01
  */

 
#ifndef CRITICAL_SECTION_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define CRITICAL_SECTION_H

class CriticalSection {
private:
    //static volatile int sectionInUse;
    SemaphoreHandle_t mutex;

public:
    CriticalSection() {
        //CriticalSection::sectionInUse = 0;
        mutex = xSemaphoreCreateMutex();
    }

    __attribute__((always_inline)) void enter() {
        //while (CriticalSection::sectionInUse > 0) {
        //    // Do nothing
        //}
        //sectionInUse++;
        xSemaphoreTake(mutex, portMAX_DELAY);
    }

    __attribute__((always_inline)) void exit() {
        //CriticalSection::sectionInUse--;
        xSemaphoreGive(mutex);
    }
};

//int volatile CriticalSection::sectionInUse = 0;

#endif // CRITICAL_SECTION_H