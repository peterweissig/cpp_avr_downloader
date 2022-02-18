/******************************************************************************
*                                                                             *
* wepet_avr_downloader.h                                                      *
* ======================                                                      *
*                                                                             *
* Version: 2.0.4                                                              *
* Date   : 01.09.16                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/                                          *
******************************************************************************/

#ifndef __WEPET_AVR_DOWNLOADER_H
#define __WEPET_AVR_DOWNLOADER_H

// local headers

// wepet headers
#include "wepet_flash.h"
#include "wepet_comport.h"
#include "wepet_progressbar.h"

// standard headers
#include <string>
#include <vector>

// additional headers



namespace wepet {

//**************************<Types>********************************************
enum eProgrammerType {
    ptNone = -1,
    ptAuto = 0,
    ptFlash1,
    ptFlash2,
    ptFlash3,
    ptFlash4,
    ptMin = ptFlash1,
    ptMax = ptFlash4
};

enum eVerbosity {
    vbSilent,
    vbQuiet,
    vbInfo,
    vbVerbose,
    vbDebug,
    vbMin = vbSilent,
    vbMax = vbDebug
};

enum eResetSequence {
    rsNone,
    rsWait,
    rsRts,
    rsDtr,
    rsMin = rsWait,
    rsMax = rsDtr
};

struct sResetSequence {
    sResetSequence(const eResetSequence type, const int value);
    eResetSequence type_;
    int value_;
};

//*****************************************************************************
//**************************{class cAvrDownloader}*****************************
//*****************************************************************************
class cAvrDownloader {
  public:
    cAvrDownloader(int argc, char **argv);
    ~cAvrDownloader();

  private:
    bool ParseOptions(int argc, char **argv);
    bool CheckOptions(void);
    bool LoadDevice(void);
    bool LoadFiles(void);
    bool OpenComport(void);
    bool Download(void);
    bool SaveFiles(void);


    bool DownloadReset(void);
    bool DownloadDetect(void);
    void DownloadWarning(void);
    bool DownloadCheck(void);
    void DownloadStart(void);
    bool DownloadPages(void);
    bool DownloadSetAddress(const int address);
    bool DownloadPage(const int page);
    bool DownloadVerify(void);
    bool DownloadVerifyPage(const int page);
    bool DownloadExit(void);
    void DownloadError(const std::string error);

    bool DownloadCommand(const std::string transmit, const char receive_char,
      const std::string error = "", const int retry = 3);
    bool DownloadCommand(const std::string transmit, const int receive_count,
      const std::string error = "", const int retry = 3);
    bool DownloadCommand(const std::string transmit, const std::string receive,
      const std::string error = "Command failed", const int receive_count = 0,
      const int retry = 3, const std::string debug = "DownloadCommand",
      const bool recurs = true);

    void OutputHeader(void);
    void OutputHelp(void);
    void OutputOptions(void);
    void OutputDevice(void);
    void OutputResetSequence(void);

    void OutputLine(const std::string line,
      const eVerbosity verbosity = vbInfo);
    void OutputError(const std::string error);
    void OutputParseError(const std::string option);

    std::string ConvertToHex(const std::string data);

    std::string path_;


    eVerbosity verbosity_;


    std::string mcu_name_;
    std::string device_id_;
    std::string device_desc_;
    std::string device_signature_;
    int    device_page_size_;
    int    device_page_count_;


    std::string filename_in_ , filename_compare_;
    std::string filename_out_, filename_changed_;
    cFlash flash_;
    int bootstart_;


    std::string comport_name_;
    int baudrate_;
    cComPortBuffer comport_;
    int command_timeout_;


    eProgrammerType programmer_;
    std::string programmer_id_;
    std::vector <sResetSequence> reset_;
    bool flag_download_;
    bool flag_verify_signature_;
    bool flag_verify_flash_;


    cProgressbar progressbar_;
};

} //namespace wepet {
#endif // #ifndef __WEPET_AVR_DOWNLOADER_H
