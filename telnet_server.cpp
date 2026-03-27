/*!
  @file   telnet_server.cpp
  @brief  Definitions of Telnet_Server members and methods.
*/

#include "telnet_server.h"
#include "awg_serial.h"

TelnetStream Telnet;     ///< Global telnet stream used by Telnet_Server / DEBUG

bool Telnet_Server::pass_through = false;

void Telnet_Server::begin ()
{
  server.begin();
}

void Telnet_Server::loop ()
{
  if (!client || !client.connected())
  {
    client = server.accept();
    if (client)
    {
      Telnet.attach(client);
      Telnet << "\nespBode telnet connected. Type PASSTHROUGH to toggle.\n";
      line_buffer = "";
    }
  }

  // Read telnet input line(s)
  if (client && client.connected() && client.available() > 0)
  {
    while (client.available() > 0)
    {
      uint8_t b = (uint8_t)client.read();

      // Basic TELNET option negotiation filter:
      // Many clients send IAC (0xFF) sequences immediately after connect.
      // If we treat those bytes as user input, the first real command can be corrupted.
      if (b == 0xFF)
      {
        // Try to consume: IAC <command> <option>
        // If not all bytes are available yet, just bail out and let the next loop finish it.
        if (client.available() < 2) return;
        (void)client.read(); // command
        (void)client.read(); // option
        continue;
      }

      char c = (char)b;
      if (c == '\r') continue;

      if (c == '\n')
      {
        String line = line_buffer;
        line_buffer = "";
        onTelnetInput(line);
      }
      else if (line_buffer.length() < 256)
      {
        line_buffer += c;
      }
    }
  }

  // Copy data from the serial port if passthrough is enabled
  if ( pass_through )
  {
    while ( AWGSerial.available() > 0 )
    {
      char c = (char)AWGSerial.read();
      Telnet << c;
    }
  }
}

/*!
  @brief  Callback function used by the telnet server.

  @param  input   Line of text received by the telnet client

  The server calls this callback function whenever a line of data (terminated
  with an \\n) is received. The callback function converts the string to upper case and trims
  leading/trailing whitespace. It then tests the string to see if it consists of a recognized
  command. If so, the command is processed and the results are reported back to the telnet client.

  Recognized commands:

    PASSTHROUGH - toggles the pass_through state

  If the string of data is not a recognized command, the callback function will either discard
  the string (if ! pass_through) or pass the string via the serial interface to the connected
  AWG (if pass_through).
*/
void Telnet_Server::onTelnetInput ( String input ) {

  String s(input); 
  s.trim();
  s.toUpperCase();

  if ( s == "PASSTHROUGH" ) {
    pass_through = ! pass_through;

    Telnet.flush();

    Telnet << "\n" << s << ( pass_through ? " ON" : " OFF" ) << "\n";

  } else if ( pass_through ) {
    // Send exactly one LF; many instruments dislike CRLF.
    AWGSerial.print(input);
    AWGSerial.print('\n');

    // Opportunistically forward any immediate reply (some devices answer very quickly).
    uint32_t start = millis();
    while (millis() - start < 50)
    {
      while (AWGSerial.available() > 0)
      {
        char c = (char)AWGSerial.read();
        Telnet << c;
      }
      delay(1);
    }
  }
}
