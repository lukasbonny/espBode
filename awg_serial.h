// Centralizes which UART is used to talk to the AWG/instrument.
#ifndef AWG_SERIAL_H
#define AWG_SERIAL_H

#include <Arduino.h>

// Use a dedicated hardware UART for the AWG so the USB console (Serial)
// remains available and pins are explicit.
extern HardwareSerial& AWGSerial;

#endif

