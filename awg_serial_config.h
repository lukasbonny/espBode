// UART pin mapping for the AWG serial connection (ESP32-C3).
#ifndef AWG_SERIAL_CONFIG_H
#define AWG_SERIAL_CONFIG_H

#include <Arduino.h>

// Set these to the GPIOs you wired to the instrument.
// ESP32 UART direction:
// - AWG_TX_PIN is ESP32 TX (goes to instrument RX)
// - AWG_RX_PIN is ESP32 RX (comes from instrument TX)
#ifndef AWG_TX_PIN
#define AWG_TX_PIN 21
#endif

#ifndef AWG_RX_PIN
#define AWG_RX_PIN 20
#endif

// Optional: change serial config if needed (default 8N1).
#ifndef AWG_SERIAL_MODE
#define AWG_SERIAL_MODE SERIAL_8N1
#endif

#endif

