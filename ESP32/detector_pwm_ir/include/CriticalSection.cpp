#ifndef CRITICAL_SECTION_H

#define CRITICAL_SECTION_H

class CriticalSection {
private:
    static volatile int sectionInUse;

public:
    CriticalSection() {
        CriticalSection::sectionInUse = 0;
    }

    __attribute__((always_inline)) void enter() {
        while (CriticalSection::sectionInUse > 0) {
            // Do nothing
        }
        sectionInUse++;
    }

    __attribute__((always_inline)) void exit() {
        CriticalSection::sectionInUse--;
    }
};

int volatile CriticalSection::sectionInUse = 0;

#endif // CRITICAL_SECTION_H