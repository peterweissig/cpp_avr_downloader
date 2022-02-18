/******************************************************************************
*                                                                             *
* wepet_xbee_config.cpp                                                       *
* =====================                                                       *
*                                                                             *
* Version: <see below>                                                        *
* Date   : <see below>                                                        *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/                                          *
******************************************************************************/

#define XBEE_CONFIG_VERSION "1.0.0"
#define XBEE_CONFIG_DATE "22.02.2017"

// local headers
#include "wepet_xbee_config.h"

// wepet headers
#include "wepet_basic.h"

// standard headers
#include <iostream>

// additional headers



namespace wepet {

//*********************************[cXbeeConfig]********************************
cXbeeConfig::cXbeeConfig(int argc, char **argv) {
  verbosity = kVeNormal;

  comport  = "/dev/ttyS0";
  baudrate_is = 9600;
  baudrate = -1;
  channel  = -1;

  debug = false;

  if (! OptionsParse(argc, argv)) {return;}

  if (! OptionsCheck()) {return;}

  if (! OptionsRead() ) {return;}

  if (! OptionsApply()) {return;}

  OutputDone();
}

//*********************************[OptionsParse]*******************************
bool cXbeeConfig::OptionsParse(int argc, char **argv) {

  if ((argc < 2) || (argv == NULL)) {
    OutputHeader();
    OutputHelp();
    return false;
  }

  for (int a = 1; a < argc; a++) {
    std::string arg = argv[a];

    if ((arg ==    "?") ||
        (arg ==   "-?") ||
        (arg ==    "h") ||
        (arg ==   "-h") ||
        (arg ==  "--h") ||
        (arg == "help")) {
      OutputHeader();
      OutputHelp();
      return false;
    }

    if ((arg ==    "b") ||
        (arg ==   "-b")) {
      a++;
      if (a < argc) {
        int temp;
        if (! StrToInt(argv[a],temp)) {
          OutputParseError(arg + " " + argv[a]);
          return false;
        }
        baudrate = temp;
        continue;
      } else {
        OutputParseError(arg);
        return false;
      }
      continue;
    }

    if ((arg ==    "c") ||
        (arg ==   "-c")) {
      a++;
      if (a < argc) {
        int temp;
        if (! StrToInt(argv[a],temp)) {
          OutputParseError(arg + " " + argv[a]);
          return false;
        }
        channel = temp;
        continue;
      } else {
        OutputParseError(arg);
        return false;
      }
      continue;
    }

    if ((arg ==    "P") ||
        (arg ==   "-P")) {
      a++;
      if (a < argc) {
        comport = argv[a];
        continue;
      } else {
        OutputParseError(arg);
        return false;
      }
      continue;
    }

    if ((arg ==    "q") ||
        (arg ==   "-q")) {
      if (verbosity > kVeNone) {verbosity = (eVerbosity) (verbosity - 1);};
      continue;
    }

    if ((arg ==    "v") ||
        (arg ==   "-v")) {
      if (verbosity < kVeAll) {verbosity = (eVerbosity) (verbosity + 1);};
      continue;
    }

    if ((arg ==    "debug") ||
        (arg ==   "-debug") ||
        (arg ==   " DEBUG") ||
        (arg ==   "-DEBUG")) {
      debug = true;
      continue;
    }

    OutputParseError(argv[a]);
        return false;
  }

  return true;
}

//*********************************[OptionsCheck]*******************************
bool cXbeeConfig::OptionsCheck() {
  if (verbosity > kVeLess) {
    OutputHeader();
  }

  if (baudrate >= 0) {
    switch (baudrate) {
      case      0:
      case      1:
      case      2:
      case      3:
      case      4:
      case      5:
      case      6:
      case      7:               break;
      case   1200: baudrate = 0; break;
      case   2400: baudrate = 1; break;
      case   4800: baudrate = 2; break;
      case   9600: baudrate = 3; break;
      case  19200: baudrate = 4; break;
      case  38400: baudrate = 5; break;
      case  57600: baudrate = 6; break;
      case 115200: baudrate = 7; break;
      default    :
        std::cout << "Error unsupported baudrate (" << baudrate << ")." << std::endl;
        return false;
    }
  }

  if (channel >= 0) {
    if ((channel < 11) || (channel > 26)) {
      std::cout << "Error unsupported channel (" << channel << ")." << std::endl;
    }
  }

  return true;
}

//*********************************[OptionsRead]********************************
bool cXbeeConfig::OptionsRead() {
  char c;
  if (!XbeeOpen()) {return false;}

  std::cout << "Firmware Version:" << std::endl;
  if (!XbeeTransmit("ATVR\r",5)) {
    std::cout << "  Error" << std::endl;
    port.Close();
    return false;
  }
  std::cout << "  " << port.BufferGet().substr(0,4) << std::endl;

  std::cout << "Hardware Version:" << std::endl;
  if (!XbeeTransmit("ATHV\r",5)) {
    std::cout << "  Error" << std::endl;
    port.Close();
    return false;
  }
  std::cout << "  " << port.BufferGet().substr(0,4) << std::endl;

  std::cout << "Baudrate:" << std::endl;
  if (!XbeeTransmit("ATBD\r",2)) {
    std::cout << "  Error" << std::endl;
    port.Close();
    return false;
  }
  std::cout << "  " << port.BufferGet().substr(0,1) << std::endl;

  std::cout << "Parity:" << std::endl;
  if (!XbeeTransmit("ATNB\r",2)) {
    std::cout << "  Error" << std::endl;
    port.Close();
    return false;
  }
  std::cout << "  " << port.BufferGet().substr(0,1) << std::endl;

  std::cout << "Channel:" << std::endl;
  if (!XbeeTransmit("ATCH\r",2)) {
    std::cout << "  Error" << std::endl;
    port.Close();
    return false;
  }
  c = port.BufferGet()[0];
  if      (('0' <= c) && (c <= '9')) { channel_is = (((int) c ) & 0xFF) - 48; }
  else if (('A' <= c) && (c <= 'F')) { channel_is = (((int) c ) & 0xFF) - 55; }
  else                               { channel_is =                       -1; }

  if (port.BufferGet()[1] == '\r') {
    std::cout << "  " << port.BufferGet().substr(0,1) << std::endl;
  } else {
    if (channel_is >= 0) {
      channel_is*= 16;
      if      (('0' <= c) && (c <= '9')) { channel_is+= (((int) c ) & 0xFF) - 48; }
      else if (('A' <= c) && (c <= 'F')) { channel_is+= (((int) c ) & 0xFF) - 55; }
      else                               { channel_is =                       -1; }
    }

    std::cout << "  " << port.BufferGet().substr(0,2) << std::endl;
    port.BufferWait(3);
  }

  return true;
}

//*********************************[OptionsApply]*******************************
bool cXbeeConfig::OptionsApply() {
  std::string s;
  if (!port.IsOpened()) {return false;}

  if (baudrate >= 0) {
    std::cout << "Set Baudrate:" << std::endl;
    s = "ATBD";
    s+= (char) (baudrate + 48);
    s+= '\r';
    if (!XbeeTransmit(s,"OK\r")) {
      std::cout << "  Error" << std::endl;
      port.Close();
      return false;
    }
    std::cout << "  " << port.BufferGet().substr(0,2) << std::endl;
  }

  if (channel >= 0) {
    std::cout << "Set Channel:" << std::endl;
    s = "ATCH";
    if (channel < 16) {
      if (channel < 10) {
        s+= (char) (channel + 48);
      } else {
        s+= (char) (channel + 55);
      }
    } else {
      if ((channel >> 4) < 10) {
        s+= (char) ((channel >> 4) + 48);
      } else {
        s+= (char) ((channel >> 4) + 55);
      }
      if ((channel & 0x0F) < 10) {
        s+= (char) ((channel & 0x0F) + 48);
      } else {
        s+= (char) ((channel & 0x0F) + 55);
      }
    }
    s+= '\r';
    if (!XbeeTransmit(s,"OK\r")) {
      std::cout << "  Error" << std::endl;
      port.Close();
      return false;
    }
    std::cout << "  " << port.BufferGet().substr(0,2) << std::endl;
  }

  if ((baudrate >= 0) || (channel >= 0)) {
    std::cout << "Apply Changes:" << std::endl;

    if (baudrate < 0) {
      if (!XbeeTransmit("ATAC\r","OK\r")) {
        std::cout << "  Error" << std::endl;
        port.Close();
        return false;
      }
      std::cout << "  " << port.BufferGet().substr(0,2) << std::endl;
    } else {
      XbeeTransmit("ATAC\r","OK\r");

      switch (baudrate) {
        case  0: baudrate =   1200; break;
        case  1: baudrate =   2400; break;
        case  2: baudrate =   4800; break;
        case  3: baudrate =   9600; break;
        case  4: baudrate =  19200; break;
        case  5: baudrate =  38400; break;
        case  6: baudrate =  57600; break;
        case  7: baudrate = 115200; break;
        default: baudrate =   9600;
      }
      port.SettingBaudRateSet(baudrate);
      port.Wait(200);

      if (!XbeeTransmit("ATAC\r","OK\r")) {
        std::cout << "  Error" << std::endl;
        port.Close();
        return false;
      }
      std::cout << "  " << port.BufferGet().substr(0,2) << std::endl;
    }

    std::cout << "Write Changes:" << std::endl;
    if (!XbeeTransmit("ATWR\r","OK\r")) {
      std::cout << "  Error" << std::endl;
      port.Close();
      return false;
    }
    std::cout << "  " << port.BufferGet().substr(0,2) << std::endl;
  }

  std::cout << "Exit:" << std::endl;
  if (!XbeeTransmit("ATCN\r","OK\r")) {
    std::cout << "  Error" << std::endl;
    port.Close();
    return false;
  }
  std::cout << "  " << port.BufferGet().substr(0,2) << std::endl;

  port.Close();
  return true;
}

//*********************************[XbeeOpen]***********************************
bool cXbeeConfig::XbeeOpen() {
  int br = -2;

  baudrate_is = 9600;

  port.SettingBaudRateSet(baudrate_is);
  port.SettingByteSizeSet(kCpByteSize8);
  port.SettingParitySet(kCpParityNone);
  port.SettingStopBitsSet(kCpStopBits2);
  port.BufferTimeSet(1500);

  if (! port.Open(comport)) {
    std::cout << "Error - Can't open comport \"" << comport << "\"." << std::endl;
    return false;
  }

  if (verbosity > kVeNormal) {
    std::cout << "Trying to connect to Xbee." << std::endl;
  }

  while (1) {
    if (verbosity > kVeNormal) {
      std::cout << "  " << baudrate_is << "baud" << std::endl;
    }

    if (XbeeTransmit("+++", "OK\r")) {
      if (verbosity > kVeNormal) {
        std::cout << "Xbee connected." << std::endl;
      }
      port.BufferTimeSet(500);

      return true;
    }

    br++;
    switch (br) {
      case -1: baudrate_is =  57600; break;
      case  0: baudrate_is =   1200; break;
      case  1: baudrate_is =   2400; break;
      case  2: baudrate_is =   4800; break;
      case  3: baudrate_is =   9600; break;
      case  4: baudrate_is =  19200; break;
      case  5: baudrate_is =  38400; break;
      case  6: baudrate_is =  57600; break;
      case  7: baudrate_is = 115200; break;
      default:
        port.Close();
        return false;
    }
    port.SettingBaudRateSet(baudrate_is);
  }
}

//*********************************[XbeeTransmit]*******************************
bool cXbeeConfig::XbeeTransmit(std::string send, int receive_count) {
  if (!port.IsOpened()) {return false;}

  port.HWBufferFlush(true, true);
  port.BufferClear();

  if (debug) {
    std::cout << "  transmitting: " << StrOut(send   ,10) << std::endl;
    std::cout << "  receiving   : " << receive_count << " bytes" << std::endl;
  }

  port.Transmit(send);
  bool result = port.BufferWait(receive_count);

  if (debug) {
    std::cout << "  received    : " << StrOut(port.BufferGet(),10) << std::endl;
    std::cout                                             << std::endl;
  }

  return result;
}

//*********************************[XbeeTransmit]*******************************
bool cXbeeConfig::XbeeTransmit(std::string send, std::string receive) {
  if (!port.IsOpened()) {return false;}

  port.HWBufferFlush(true, true);
  port.BufferClear();

  if (debug) {
    std::cout << "  transmitting: " << StrOut(send   ,10) << std::endl;
    std::cout << "  receiving   : " << StrOut(receive,10) << std::endl;
  }

  port.Transmit(send);
  bool result = port.BufferWait(receive);

  if (debug) {
    std::cout << "  received    : " << StrOut(port.BufferGet(),10) << std::endl;
    std::cout                                             << std::endl;
  }

  return result;
}

//*********************************[OutputHeader]*******************************
void cXbeeConfig::OutputHeader(void) {
  std::cout                                                   << std::endl;
  std::cout << "Xbee"                                         << std::endl;
  std::cout << "===="                                         << std::endl;
  std::cout                                                   << std::endl;
  std::cout << "Version: " << XBEE_CONFIG_VERSION             << std::endl;
  std::cout << "Date   : " << XBEE_CONFIG_DATE                << std::endl;
  std::cout << "Author : Peter Weissig"                       << std::endl;
  std::cout                                                   << std::endl;
}

//*********************************[OutputHelp]*********************************
void cXbeeConfig::OutputHelp(void) {
  std::cout                                                   << std::endl;
  std::cout << "Usage: xbee [options]"                        << std::endl;
  std::cout << "Options:"                                     << std::endl;
  std::cout << "  -P <port>                  Specify connection port."                            << std::endl;
  std::cout                                                                                       << std::endl;
  std::cout << "  -b <baudrate>              Override device baud rate."                          << std::endl;
  std::cout << "                             0 -    1200"                                         << std::endl;
  std::cout << "                             1 -    2400"                                         << std::endl;
  std::cout << "                             2 -    4800"                                         << std::endl;
  std::cout << "                             3 -    9600"                                         << std::endl;
  std::cout << "                             4 -   19200"                                         << std::endl;
  std::cout << "                             5 -   38400"                                         << std::endl;
  std::cout << "                             6 -   57600"                                         << std::endl;
  std::cout << "                             7 -  115200"                                         << std::endl;
  std::cout                                                                                       << std::endl;
  std::cout << "  -c <channel>               Override device channel."                            << std::endl;
  std::cout << "                             0x0B - 11"                                           << std::endl;
  std::cout << "                             0x0C - 12"                                           << std::endl;
  std::cout << "                             ..."                                                 << std::endl;
  std::cout << "                             0x1A - 26"                                           << std::endl;
  std::cout                                                                                       << std::endl;
  std::cout << "  -v                         Verbose output."                                     << std::endl;
  std::cout << "  -?                         Display this usage"                                  << std::endl;
  std::cout                                                   << std::endl;

  std::cout                                                   << std::endl;
}

//*********************************[OutputParseError]***************************
void cXbeeConfig::OutputParseError(std::string option) {
  if (verbosity > kVeLess) {
    OutputHeader();
  }

  if (verbosity > kVeNone) {
    std::cout << "Error by parsing option \"" << option << "\"." << std::endl;
  }
}

//*********************************[OutputDone]*********************************
void cXbeeConfig::OutputDone() {
  if (verbosity > kVeLess) {
    std::cout << std::endl << "Done" << std::endl;
  }
}

} //namespace wepet {
