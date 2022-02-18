/******************************************************************************
*                                                                             *
* wepet_xbee_config.h                                                         *
* ===================                                                         *
*                                                                             *
* Version: 1.0.0                                                              *
* Date   : 22.02.17                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/                                          *
******************************************************************************/

#ifndef __WEPET_XBEE_CONFIG_H
#define __WEPET_XBEE_CONFIG_H

// local headers

// wepet headers
#include "wepet_comport.h"

// standard headers
#include <string>

// additional headers



namespace wepet {

//*********************************<Types>**************************************
enum eVerbosity {
  kVeNone,
  kVeLess,
  kVeNormal,
  kVeMore,
  kVeAll
};

//******************************************************************************
//*********************************{class cXbeeConfig}**************************
//******************************************************************************
class cXbeeConfig {
  public:
    cXbeeConfig(int argc, char **argv);

  private:
    bool OptionsParse(int argc, char **argv);
    bool OptionsCheck(void);

    bool OptionsRead(void);
    bool OptionsApply(void);

    bool XbeeOpen(void);
    bool XbeeTransmit(std::string send, std::string receive);
    bool XbeeTransmit(std::string send, int receive_count);

    void OutputHeader(void);
    void OutputHelp(void);
    void OutputParseError(std::string option);
    void OutputDone(void);

    cComPortBuffer port;
    eVerbosity verbosity;

    std::string comport;
    int baudrate_is;
    int baudrate;
    int channel;
    int channel_is;

    bool debug;
};

} //namespace wepet {
#endif // #ifndef __WEPET_XBEE_CONFIG_H