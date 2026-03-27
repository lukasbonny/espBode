#ifndef TELNET_SERVER_H
#define TELNET_SERVER_H

/*!
  @file   telnet_server.h
  @brief  Declaration of the Telnet_Server class.
*/

#include "net_compat.h"
#include "Streaming.h"

// Telnet output stream used by DEBUG and passthrough.
class TelnetStream : public Print {
  public:
    TelnetStream() = default;

    void attach(NetClient client) { m_client = client; m_has_client = true; }
    void detach() { if (m_has_client) m_client.stop(); m_has_client = false; }
    bool connected() { return m_has_client && (m_client.connected() != 0); }

    virtual size_t write(uint8_t b) override {
      if (!connected()) return 0;
      return m_client.write(&b, 1);
    }
    size_t write(const uint8_t* buffer, size_t size) {
      if (!connected()) return 0;
      return m_client.write(buffer, size);
    }
    void flush() { if (connected()) m_client.flush(); }

  private:
    NetClient m_client;
    bool m_has_client = false;
};

extern TelnetStream Telnet;   ///< Global telnet stream used by Telnet_Server / DEBUG

/*!
  @brief  The Telnet_Server class implements command checking
          and passthrough using a basic telnet service.
*/
class Telnet_Server {

  public:

    Telnet_Server ()  ///< Default constructor does nothing
      {}
    
    ~Telnet_Server () ///< Default destructor does nothing
      {}

    /*!
      @brief  Sets up the onTelnetInput callback and
              initiates the Telnet service.
    */
    void  begin ();

    /*!
      @brief  Call this at least once per main loop to
              process any passthrough data; also calls
              Telnet.loop()
    */
    void  loop ();

  protected:

    static  void onTelnetInput ( String s );

    static  bool  pass_through;   ///< State variable shows whether PASSTHROUGH is enabled

    NetServer_ext  server{23};
    NetClient      client;
    String         line_buffer;
};

#endif