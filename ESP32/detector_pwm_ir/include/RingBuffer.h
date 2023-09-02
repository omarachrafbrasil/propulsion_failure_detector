/**
 * @(#) RingBuffer.h
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
  * Class Ring Buffer data structure.
  *
  * @author: Omar Achraf
  * @version: 10/jully/2023 11H01
  */


#ifndef RINGBUFFER_H

#define RINGBUFFER_H

template <typename T, int SIZE>
class RingBuffer
{
    volatile T buffer[SIZE];
    volatile int writepos;
    volatile int readpos;

public:
    RingBuffer(T value, const bool isFilled = false) : writepos(0), readpos(0) {
        if (isFilled) {
            for (int i = 0; i < SIZE; i++) {
                put(value);
            }   
        }
    }

    __attribute__((always_inline)) void put(T c) {
        buffer[writepos] = c;
        writepos = ++writepos % SIZE;
    }

    __attribute__((always_inline)) T get() {
        T ret = buffer[readpos];
        readpos = ++readpos % SIZE;
        return ret;
    }

    __attribute__((always_inline)) T peek(int i) {
        T ret = buffer[i % SIZE];
        return ret;
    }

    __attribute__((always_inline)) bool isEmpty() {
        return readpos == writepos;
    }

    __attribute__((always_inline)) T getLength() {
        return (T)(writepos - readpos) % SIZE;
    }

    __attribute__((always_inline)) void clear() {
        readpos = 0;
        writepos = 0;
    }

    __attribute__((always_inline)) void copyTo(std::vector<T>& valuesOutBuffer) {
        int rp = readpos; // thread safe
        for (int i = 0; i < SIZE; i++) {
            valuesOutBuffer[i] = buffer[(rp + i) % SIZE];
        }
    }
};

#endif