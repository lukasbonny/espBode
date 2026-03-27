// Compatibility layer to run espBode networking over W5500 Ethernet.
#ifndef NET_COMPAT_H
#define NET_COMPAT_H

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

using NetClient = EthernetClient;
using NetUDP = EthernetUDP;

// Provide WiFiServer-like API: default constructor + begin(port) + accept() + stop().
// EthernetServer doesn't support changing port after construction, so we wrap it.
class NetServer_ext {
 public:
  NetServer_ext() = default;
  explicit NetServer_ext(uint16_t port) { begin(port); }

  void begin() {
    if (!m_server) begin(23);
    else m_server->begin();
  }

  void begin(uint16_t port) {
    if (m_server) { delete m_server; m_server = nullptr; }
    m_port = port;
    m_server = new EthernetServer(m_port);
    m_server->begin();
  }

  void stop() {
    // EthernetServer doesn't expose stop(); best effort.
    if (m_server) { delete m_server; m_server = nullptr; }
  }

  NetClient accept() {
    if (!m_server) return NetClient();
    return m_server->available();
  }

 private:
  uint16_t m_port = 23;
  EthernetServer* m_server = nullptr;
};

#endif

