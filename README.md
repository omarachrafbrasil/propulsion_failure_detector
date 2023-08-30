# propulsion_failure_detector
A firmware to Propulsion Failure Detection of Unmanned Aerial Vehicles


Omar Achraf / UVSBR - Unmanned Vehicle Systems do Brasil
jully/2023

ESP32 30 pin board contains the firmware with platformIO espressif freeRTOS having these tasks:
- PWM task: measeure PWM pulses at GPIO26 = D26, as well counter pulses (frequency);
- IR task: measure IR sensor Rising edge pulses on GPIO25 = D25, to counter pulses (frequency);
- Timer task: a Timer interrupt based in 0.1 seconds to collect PWM pulse width as well PWM counter plus IR counter;
- Display task: show, for debug, PWM and IR measurements in OLED display;
