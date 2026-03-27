/*!
  @file   espBode.ino
  @brief  Main file of espBode on Arduino IDE
*/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "Streaming.h"
#include "eth_config.h"
#include "debug.h"
#include "rpc_bind_server.h"
#include "vxi_server.h"
#include "telnet_server.h"
#include "awg_fy6900.h"
#include "awg_serial.h"
#include "awg_serial_config.h"

// global variables

AWG_FY6900      awg;                          ///< Use the FY6900 variant of the AWG_Server class
VXI_Server      vxi_server(awg);              ///< The VXI_Server
RPC_Bind_Server rpc_bind_server(vxi_server);  ///< The RPC_Bind_Server
Telnet_Server   telnet_server;                ///< The Telnet_Server

/*!
  @brief  Set up the Ethernet (W5500 over SPI) connection.

  Ethernet setup is separated out from the rest of the setup()
  routine just to keep the latter from being too cluttered.
*/
static void setup_Ethernet ()
{
  if (ETH_RST >= 0)
  {
    pinMode(ETH_RST, OUTPUT);
    digitalWrite(ETH_RST, LOW);
    delay(50);
    digitalWrite(ETH_RST, HIGH);
    delay(200);
  }

  SPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI, ETH_CS);
  Ethernet.init(ETH_CS);

  #ifdef STATIC_IP
    Ethernet.begin((uint8_t*)ETH_MAC, ESP_IP, ETH_DNS, ESP_GW, ESP_MASK);
  #else
    Ethernet.begin((uint8_t*)ETH_MAC);
  #endif

  // Some W5500 modules need a moment after begin()
  delay(250);

  if ( Debug.Channel() == DEBUG::VIA_SERIAL )
  {
    Debug.Progress() << "\nEthernet up; IP address = " << Ethernet.localIP().toString() << "\n\n";
  }

  #ifdef USE_LED
    blink(5,100);
  #endif
}

/*!
  @brief  Standard Arduino setup() function to perform initializations.

  Here we need to set up the LED pin, initialize the serial port,
  connect to the WiFi, and initialize the various servers.
*/
void setup()
{
    // USB console (optional). Keep separate from the AWG UART.
    Serial.begin(115200);

    // Dedicated UART for the AWG/instrument.
    AWGSerial.begin(awg.baud_rate(), AWG_SERIAL_MODE, AWG_RX_PIN, AWG_TX_PIN);

    #ifdef USE_LED
      pinMode(LED_BUILTIN, OUTPUT);
    #endif

  /*  Set desired debug level and output channel. Serial debugging
      at any level works flawlessly, but of course the Serial
      channel is needed to communicate with the AWG, so it is only
      suitable when testing without the AWG connected to the ESP-01.

      Telnet debugging allows testing WITH the AWG connected, but
      note that Telent with level = PACKET seems to get backed up,
      and the oscilloscope winds up having to retry several times
      to keep the communication flowing. It does work, but it slows
      everything down. However, debugging at level = PROGRESS or
      even level = SERIAL_IO works flawlessly. 
  */

  Debug.Via_Telnet();
  Debug.Filter_Progress();

  /*  Note that if Debug is directed to Telnet, anything sent to Debug at this point
      will effectively be lost since wifi is not yet connected. However, we do not want to
      assume that it is safe to direct the Debug messages to serial; if the AWG is connected,
      it may get confused. Therefore, we will direct the progress message below to serial
      only if Debug is specifically set to output to serial.
  */

  setup_Ethernet();

  /*  Initiailize the various servers - telnet_server,
      rpc_bind_server, and vxi_server.
  */

  awg.retry(2);               // validate settings with up to 2 retries
  vxi_server.begin();
  rpc_bind_server.begin();
  telnet_server.begin();
}

/*!
  @brief  Standard Arduino main loop

  The main loop simply calls the loop() method of each of the servers,
  allowing them to do any processing they need to do before passing
  control to the next server.
*/
void loop() {
  telnet_server.loop();
  rpc_bind_server.loop();
  vxi_server.loop();
}
