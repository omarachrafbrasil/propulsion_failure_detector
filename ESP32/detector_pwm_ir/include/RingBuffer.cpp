#ifndef RINGBUFFER_H

template <typename T, unsigned int SIZE>
class RingBuffer
{
    volatile T buffer[SIZE];
    volatile T writepos;
    volatile T readpos;

public:
    RingBuffer() : writepos(0), readpos(0)
    {
    }

    volatile bool complete = false;

    __attribute__((always_inline)) void put(T c)
    {
        buffer[writepos] = c;
        writepos = ++writepos % SIZE;
    }

    __attribute__((always_inline)) T get()
    {
        T ret = buffer[readpos];
        readpos = ++readpos % SIZE;
        return ret;
    }

    __attribute__((always_inline)) bool isEmpty()
    {
        return readpos == writepos;
    }

    __attribute__((always_inline)) bool isNotEmpty()
    {
        return readpos != writepos;
    }

    __attribute__((always_inline)) T getLength()
    {
        return (T)(writepos - readpos) % SIZE;
    }

    __attribute__((always_inline)) void clear()
    {
        readpos = 0;
        writepos = 0;
    }
};

#define RINGBUFFER_H
#endif