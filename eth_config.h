// Ethernet (W5500 over SPI) configuration for ESP32.
#ifndef ETH_CONFIG_H
#define ETH_CONFIG_H

#include <Arduino.h>
#include <IPAddress.h>

// ----- SPI pinout (adjust to your wiring) -----
// If you use the ESP32 default VSPI pins, you can comment these out and let SPI.begin() default.
#ifndef ETH_SPI_SCK
#define ETH_SPI_SCK 4
#endif
#ifndef ETH_SPI_MISO
#define ETH_SPI_MISO 5
#endif
#ifndef ETH_SPI_MOSI
#define ETH_SPI_MOSI 6
#endif

// W5500 chip select (CS/SS)
#ifndef ETH_CS
#define ETH_CS 7
#endif

// Optional reset pin for the W5500 module (set to -1 if not connected)
#ifndef ETH_RST
#define ETH_RST -1
#endif

// ----- MAC address -----
// Must be unique on your LAN.
static const uint8_t ETH_MAC[6] = { 0x02, 0xEB, 0x0D, 0xE2, 0x00, 0x01 };

// ----- IP configuration -----
// Comment out STATIC_IP to use DHCP.
#define STATIC_IP

#ifdef STATIC_IP
static const IPAddress ESP_IP(192, 168, 1, 249);
static const IPAddress ESP_GW(192, 168, 1, 1);
static const IPAddress ESP_MASK(255, 255, 255, 0);
static const IPAddress ETH_DNS(192, 168, 1, 1);
#endif

#endif

