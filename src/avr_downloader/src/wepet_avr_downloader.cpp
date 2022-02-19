/******************************************************************************
*                                                                             *
* wepet_avr_downloader.cpp                                                    *
* ========================                                                    *
*                                                                             *
* Version: <see below>                                                        *
* Date   : <see below>                                                        *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_avr_downloader                        *
******************************************************************************/

#define AVR_DOWNLOADER_VERSION "2.0.4"
#define AVR_DOWNLOADER_DATE "01.09.2016"

// local headers
#include "wepet_avr_downloader.h"

#include "wepet_avrdude_file.h"

// wepet headers
#include "wepet_basic.h"
#include "wepet_hexfile.h"

// standard headers
#include <iostream>

// additional headers



namespace wepet {

//*****************************************************************************
//**************************[sResetSequence]***********************************
//*****************************************************************************

sResetSequence::sResetSequence(const eResetSequence type, const int value) :
  type_(type), value_(value) {

}

//*****************************************************************************
//**************************{class cAvrDownloader}*****************************
//*****************************************************************************

//**************************[cAvrDownloader]***********************************
cAvrDownloader::cAvrDownloader(int argc, char **argv) {

    //path_ = "";

    verbosity_ = vbInfo;

    //mcu_name_          = "";
    //device_param_      = "";
    //device_id_         = "";
    //device_desc_       = "";
    //device_signature_  = "";
    device_page_size_  = -1;
    device_page_count_ = -1;

    //filename_in_      = "";
    //filename_compare_ = "";
    //filename_out_     = "";
    //filename_changed_ = "";
    //flash_ ...
    bootstart_          = -1;

    comport_name_ = "/dev/ttyS0";
    baudrate_     = 57600;
    //comport_ ...
    command_timeout_ = 200;

    programmer_    = ptAuto;
    //programmer_id_ = "";
    //reset_.clear();

    flag_download_         = true;
    flag_verify_signature_ = true;
    flag_verify_flash_     = true;

    //progressbar_ ...


    if (! ParseOptions(argc, argv)) { return; }
    if (! CheckOptions()          ) { return; }

    if (! LoadDevice()            ) { return; }
    if (! LoadFiles()             ) { return; }

    if (! OpenComport()           ) { return; }
    if (! Download()              ) { return; }

    if (! SaveFiles()             ) { return; }

    OutputLine("done", vbSilent);
}

//**************************[~cAvrDownloader]**********************************
cAvrDownloader::~cAvrDownloader() {

}

//**************************[ParseOptions]*************************************
bool cAvrDownloader::ParseOptions(int argc, char **argv) {

    if ((argc < 2) || (argv == NULL)) {
        OutputHeader();
        OutputHelp();
        return false;
    }

    std::string temp_path = argv[0];
    for (int a = temp_path.size() - 1; a >= 0; a--) {
        if ((temp_path[a] == '/') || (temp_path[a] == '\\') ||
          (temp_path[a] == ':')) {
            path_ = temp_path.substr(0, a + 1);
            break;
        }
    }

    for (int a = 1; a < argc; a++) {
        std::string arg = argv[a];

        if ((arg ==     "?") ||
          (arg ==      "-?") ||
          (arg ==      "-h") ||
          (arg ==    "help") ||
          (arg ==  "--help")) {
            OutputHeader();
            OutputHelp();
            return false;
        }

        if (arg == "-I") {
            a++;
            if (a >= argc) {
                OutputParseError(arg);
                return false;
            }

            filename_in_ = argv[a];
            continue;
        }

        if (arg == "-i") {
            a++;
            if (a >= argc) {
                OutputParseError(arg);
                return false;
            }

            filename_compare_ = argv[a];
            continue;
        }

        if (arg == "-O") {
            a++;
            if (a >= argc) {
                OutputParseError(arg);
                return false;
            }

            filename_out_ = argv[a];
            continue;
        }

        if (arg == "-C") {
            a++;
            if (a >= argc) {
                OutputParseError(arg);
                return false;
            }

            filename_changed_ = argv[a];
            continue;
        }

        if (arg == "-R") {
            if (a >= argc) {
                OutputParseError(arg);
                return false;
            }

            while (a < argc-1) {
                std::string reset = argv[a+1];

                if (reset == "") {
                    OutputParseError(arg + " [...] " + "\"\"");
                    return false;
                }
                if (reset[0] == '-') {
                    break;
                }

                a++;
                if ((reset == "RTS+") || (reset == "RTS" )) {
                    reset_.push_back(sResetSequence(rsRts, 0xFF));
                    continue;
                }
                if ((reset == "RTS-") || (reset == "rts" )) {
                    reset_.push_back(sResetSequence(rsRts, 0x00));
                    continue;
                }
                if ((reset == "DTR+") || (reset == "DTR" )) {
                    reset_.push_back(sResetSequence(rsDtr, 0xFF));
                    continue;
                }
                if ((reset == "DTR-") || (reset == "dtr" )) {
                    reset_.push_back(sResetSequence(rsDtr, 0x00));
                    continue;
                }

                int time;
                if (! wepet::StrToInt(reset, time)) {
                    OutputParseError(arg + " [...] " + reset);
                    return false;
                }
                if (time <   10) {time =   10;}
                if (time > 2000) {time = 2000;}
                reset_.push_back(sResetSequence(rsWait, time));

                continue;
            }
            continue;
        }

        if (arg == "-p") {
            a++;
            if (a >= argc) {
                OutputParseError(arg);
                return false;
            }

            mcu_name_ = argv[a];
            continue;
        }

        if (arg == "-b") {
            a++;
            if (a >= argc) {
                OutputParseError(arg);
                return false;
            }

            int baud;
            if (! wepet::StrToInt(argv[a], baud)) {
                OutputParseError(arg + argv[a]);
                return false;
            }

            baudrate_ = baud;
            continue;
        }

        if (arg == "-c") {
            a++;
            if (a >= argc) {
                OutputParseError(arg);
                return false;
            }
            std::string programmer = wepet::LowerCase(argv[a]);

            if (programmer == "auto") {
                programmer_ = ptAuto;
                programmer_id_ = "";
                continue;
            }

            if ((programmer == "flash") || (programmer  == "fd" )) {
                programmer_ = ptFlash1;
                programmer_id_ = "FD";
                continue;
            }

            if ((programmer == "flash2") || (programmer  == "f2" )) {
                programmer_ = ptFlash2;
                programmer_id_ = "F2";
                continue;
            }

            if ((programmer == "flash3") || (programmer  == "f3" )) {
                programmer_ = ptFlash3;
                programmer_id_ = "F3";
                continue;
            }

            if ((programmer == "flash4") || (programmer  == "f4" )) {
                programmer_ = ptFlash4;
                programmer_id_ = "F4";
                continue;
            }

            OutputParseError(arg + " " + programmer);
            return false;
        }

        if (arg == "-P") {
            a++;
            if (a >= argc) {
                OutputParseError(arg);
                return false;
            }

            comport_name_ = argv[a];
            continue;
        }

        if (arg == "-T") {
            a++;
            if (a >= argc) {
                OutputParseError(arg);
                return false;
            }

            int time;
            if (! wepet::StrToInt(argv[a], time)) {
                OutputParseError(arg + argv[a]);
                return false;
            }

            command_timeout_ = time;
            continue;
        }

        if (arg ==   "-q") {
            if (verbosity_ > vbMin) {
                verbosity_ = (eVerbosity) (verbosity_ - 1);
            }
            continue;
        }

        if (arg ==   "-v") {
            if (verbosity_ < vbMax) {
                verbosity_ = (eVerbosity) (verbosity_ + 1);
            }
            continue;
        }

        if (arg ==   "-n") {
            flag_download_ = false;
            continue;
        }

        if (arg ==   "-F") {
            flag_verify_signature_ = false;
            continue;
        }

        if (arg ==   "-V") {
            flag_verify_flash_ = false;
            continue;
        }

        if ((arg ==   "-debug") || (arg ==   "-DEBUG")) {
            verbosity_ = vbMax;
            continue;
        }

        OutputParseError(arg);
        return false;
    }

    return true;
}

//**************************[CheckOptions]*************************************
bool cAvrDownloader::CheckOptions() {

    if (verbosity_ >= vbQuiet) {
        OutputHeader();
    }

    if (verbosity_ >= vbVerbose) {
        OutputOptions();
    }

    if (mcu_name_ == "") {
        OutputError("no avr device given (option -p)");
        return false;
    }

    if (filename_in_ == "") {
        OutputError("no inputfile given (option -I)");
        return false;
    }

    if ((reset_.size() > 0) &&
      (verbosity_ >= vbVerbose)) {
        OutputResetSequence();
    }

    return true;
}

//**************************[LoadDevice]***************************************
bool cAvrDownloader::LoadDevice() {

    if (mcu_name_ == "") {
        return true;
    }

    cAvrdudeFile file(path_ + "avrdude.conf");

    if (file.HasError()) {
        OutputError(file.ReturnLastError());
        return false;
    }

    if (! file.SetController(mcu_name_)) {
        OutputError(file.ReturnLastError());
        return false;
    }

    device_desc_       = file.GetDescription();
    device_id_         = file.GetId();
    device_page_count_ = file.GetPageCount();
    device_page_size_  = file.GetPageSize();
    device_signature_  = file.GetSignature();
    bootstart_         = device_page_count_ * device_page_size_;

    if (! flash_.Clear(device_page_size_, device_page_count_)) {
        OutputError(flash_.ReturnLastError());
        return false;
    }

    if (verbosity_ >= vbVerbose) {
        OutputDevice();
    }

    return true;
}

//**************************[LoadFiles]****************************************
bool cAvrDownloader::LoadFiles() {

    if (filename_in_ == "") {
        OutputError("no input file specified");
        return false;
    }

    if (filename_compare_ != "") {
        flash_.LoadFromHexFile(filename_compare_);
        if (flash_.HasError()) {
            OutputError(flash_.ReturnLastError());
            return false;
        }
        OutputLine("input file \"" + filename_compare_ + "\" loaded",
          vbVerbose);

        flash_.Reset();
    }

    flash_.LoadFromHexFile(filename_in_);
    if (flash_.HasError()) {
        OutputError(flash_.ReturnLastError());
        return false;
    }
    OutputLine("input file \"" + filename_in_ + "\" loaded", vbVerbose);

    OutputLine("", vbVerbose);

    return true;
}

//**************************[OpenComport]*****************************************
bool cAvrDownloader::OpenComport() {

    comport_.BufferTimeSet(command_timeout_);
    if (! comport_.SettingBaudRateSet(baudrate_)) {
        OutputError("can not set baudrate of comport (" +
          wepet::IntToStr(baudrate_) + " baud)");
    }
    if (! comport_.SettingByteSizeSet(kCpByteSize8)) {
        OutputError("can not set bytesize of comport (8 bits)");
    }
    if (! comport_.SettingParitySet(kCpParityNone)) {
        OutputError("can not set parity of comport (no parity)");
    }
    if (! comport_.SettingStopBitsSet(kCpStopBits2)) {
        OutputError("can not set stop bits of comport (2 stop bits)");
    }

    if (! comport_.Open(comport_name_)) {
        OutputError("can not open comport (" + comport_name_ + ")");
    }

    return true;

}

//**************************[Download]*****************************************
bool cAvrDownloader::Download() {

    if (reset_.size()) {
        if (! DownloadDetect()) {
            DownloadReset();
        }
    }
    if (! DownloadDetect()) {
        OutputError("bootloader is not responding");
        return false;
    }

    DownloadWarning();

    if (! DownloadCheck()) {
        return false;
    }

    DownloadStart();

    if (! DownloadPages()) {
        return false;
    }

    if (! DownloadVerify()) {
        return false;
    }

    DownloadExit();

    return true;
}

//**************************[SaveFiles]****************************************
bool cAvrDownloader::SaveFiles() {

    if (filename_changed_ != "") {
        flash_.SaveToHexFile(filename_changed_, fsGetChanged);
        if (flash_.HasError()) {
            OutputError(flash_.ReturnLastError());
            return false;
        }
        OutputLine("output file \"" + filename_compare_ + "\" saved",
          vbVerbose);
    }


    if (filename_out_ != "") {
        flash_.SaveToHexFile(filename_out_);
        if (flash_.HasError()) {
            OutputError(flash_.ReturnLastError());
            return false;
        }
        OutputLine("output file \"" + filename_out_ + "\" saved",
          vbVerbose);
    }

    return true;
}

//**************************[DownloadReset]************************************
bool cAvrDownloader::DownloadReset() {

    if (reset_.size() < 1) { return true; }

    if (! comport_.IsOpened()) { return false; }

    OutputLine("Reset sequence", vbInfo);

    for (int i = 0; i < reset_.size(); i++) {
        switch (reset_[i].type_) {
            case rsNone:
                OutputLine("  [none]", vbDebug);
                break;

            case rsWait:
                OutputLine("  wait " +
                  wepet::IntToStr(reset_[i].value_, 4) + "ms", vbDebug);
                comport_.Wait(reset_[i].value_);
                break;

            case rsRts:
                if (reset_[i].value_) {
                    OutputLine("  set   rts", vbDebug);
                    comport_.LineRtsSet(true);
                } else {
                    OutputLine("  clear rts", vbDebug);
                    comport_.LineRtsSet(false);
                }

                break;

            case rsDtr:
                if (reset_[i].value_) {
                    OutputLine("  set   dtr", vbDebug);
                    comport_.LineDtrSet(true);
                } else {
                    OutputLine("  clear dtr", vbDebug);
                    comport_.LineDtrSet(false);
                }
                break;
        }
    }

    OutputLine("", vbInfo);

    comport_.HWBufferFlush(true, true);
    comport_.BufferClear();

    return true;
}

//**************************[DownloadDetect]***********************************
bool cAvrDownloader::DownloadDetect() {

    if (! comport_.IsOpened()) { return false; }

    if (! DownloadCommand("?", programmer_id_, "",
      2, 1, "DetectBootloader", false)) {
        return false;
    }

    if (programmer_id_ != "") {
        return true;
    }

    std::string id = comport_.BufferGet().substr(0,2);
    if (id == "FD") {
        programmer_id_ = id;
        programmer_    = ptFlash1;
        OutputLine("detected bootloader version 1", vbVerbose);
        OutputLine("", vbVerbose);
        return true;
    }
    if (id == "F2") {
        programmer_id_ = id;
        programmer_    = ptFlash2;
        OutputLine("detected bootloader version 2", vbVerbose);
        OutputLine("", vbVerbose);
        return true;
    }
    if (id == "F3") {
        programmer_id_ = id;
        programmer_    = ptFlash3;
        OutputLine("detected bootloader version 3", vbVerbose);
        OutputLine("", vbVerbose);
        return true;
    }
    if (id == "F4") {
        if (! comport_.BufferWait(3)) {
            OutputLine("bootloader id incomplete", vbVerbose);
            OutputLine("", vbVerbose);
            return false;
        }
        id = comport_.BufferGet();
        if (id == "F4\r") {
            programmer_id_ = id;
            programmer_    = ptFlash4;
            OutputLine("detected bootloader version 4", vbVerbose);
            OutputLine("", vbVerbose);
            return true;
        }
    }

    OutputLine("unknown bootloader id (" + id + ")", vbVerbose);
    OutputLine("", vbVerbose);
    return false;
}

//**************************[DownloadWarning]**********************************
void cAvrDownloader::DownloadWarning() {

    switch (programmer_) {
        case ptFlash1:
            //OutputLine("flash version tested - October 2015", vbDebug);
            //OutputLine("", vbDebug);
            break;

        case ptFlash2:
            //OutputLine("flash version tested - October 2015", vbDebug);
            //OutputLine("", vbDebug);
            break;

        case ptFlash3:
            OutputLine("warning: this flash version was not tested yet");
            OutputLine("");
            break;

        case ptFlash4:
            //OutputLine("flash version tested - October 2015", vbDebug);
            //OutputLine("", vbDebug);
            break;
    }
}

//**************************[DownloadCheck]************************************
bool cAvrDownloader::DownloadCheck() {

    comport_.HWBufferFlush(true, true);
    comport_.BufferClear();

    std::string send;
    int receive_count = 0;
    switch (programmer_) {
        case ptFlash1: send =    "I"; receive_count =  7;
            if (flash_.FlashSizeGet() > 0x10000) { receive_count++; } break;
        case ptFlash2: send =    "I"; receive_count = 10; break;
        case ptFlash3: send =    "I"; receive_count =  9;
            if (flash_.FlashSizeGet() > 0x10000) { receive_count++; } break;
        case ptFlash4: send = "info"; receive_count = 10; break;
        default      : DownloadError("unknown bootloader version (" +
            wepet::IntToHex(programmer_, -2) + ")"); return false;
    }
    std::string error = "failed to receive full bootloader info";
    if (! DownloadCommand(send, receive_count, error)) {
        return false;
    }
    std::string data = comport_.BufferGet();
    if (data.size() < receive_count) { DownloadError(error); return false; }


    std::string signature;
    int bootstart = -1;
    int pagesize  = -1;

    error = "wrong bootloader info";
    if (programmer_ == ptFlash1) {
        if (data[0] != 'I') {
            DownloadError(error);
            return false;
        }
        signature = data.substr(1,3);
        pagesize  = (((int) data[4] & 0xFF)      ) << 1;
        bootstart = (((int) data[5] & 0xFF)      ) << 1;
        bootstart|= (((int) data[6] & 0xFF) <<  8) << 1;
        if (receive_count > 7) {
            bootstart|= (((int) data[7] & 0xFF) << 16) << 1;
        }

    } else {
        if (programmer_ < ptFlash4) {
            if (data[0] != 'i') {
                DownloadError(error);
                return false;
            }
        } else {
            if (data[0] != 'I') {
                DownloadError(error);
                return false;
            }
        }
        signature = data.substr(1,3);
        pagesize  = ((int) data[4] & 0xFF);
        pagesize |= ((int) data[5] & 0xFF) << 8;
        bootstart = ((int) data[6] & 0xFF);
        bootstart|= ((int) data[7] & 0xFF) << 8;
        if (receive_count > 9) {
            bootstart|= ((int) data[8] & 0xFF) << 16;
        }
        if (data[receive_count - 1] != '\r') {
            DownloadError(error);
            return false;
        }
    }

    if (signature != device_signature_) {
        std::string msg = "wrong signature " + ConvertToHex(signature) +
          " (expected " + ConvertToHex(device_signature_) + ")";

        if (flag_verify_signature_) {
            OutputError(msg);
            return false;
        } else {
            OutputLine(msg, vbInfo);
            OutputLine("", vbInfo);
        }
    } else {
        OutputLine("signature ok", vbDebug);
    }

    if (pagesize != device_page_size_) {
        OutputError("wrong page size " + wepet::IntToStr(pagesize) +
          " (expected " + wepet::IntToStr(device_page_size_) + ")");
        return false;
    } else {
        OutputLine("page size ok", vbDebug);
    }

    bootstart_ = bootstart;
    OutputLine("bootloader start address " + wepet::IntToStr(bootstart) +
      + " (" + wepet::IntToHex(bootstart,-6) + ")", vbDebug);
    OutputLine("", vbDebug);

    return true;
}

//**************************[DownloadStart]************************************
void cAvrDownloader::DownloadStart() {

    int total_bytes = 0;
    int total_pages = 0;

    for (int page = flash_.PageMinGet(); page <= flash_.PageMaxGet(); page++) {
        if ((flash_.PageStateGet(page) & fsGetChanged) == 0x00) { continue; }
        for (int word = (flash_.PageSizeGet() / 2) - 1; word >= 0; word--) {
            if ((flash_.WordStateGet(page, word) & fsGetChanged) == 0x00) {
                continue;
            }
            total_bytes+= 2;
        }
        total_pages++;
    }

    OutputLine("Downloading " + wepet::IntToStr(total_bytes,6) + " bytes (" +
      wepet::IntToStr(total_pages * flash_.PageSizeGet(), 6) + ")");
    OutputLine("");

    progressbar_.Init("", total_pages);
}

//**************************[DownloadPages]************************************
bool cAvrDownloader::DownloadPages() {

    if (! flag_download_) {
        OutputLine("download deactivated",vbVerbose);
        return true;
    }

    if (verbosity_ < vbDebug) {
        progressbar_.Reset("Download  ");
        progressbar_.Update(0);
    }

    int written_pages = 0;
    int error_count = 0;

    for (int page = flash_.PageMinGet(); page <= flash_.PageMaxGet(); page++) {
        if ((flash_.PageStateGet(page) & fsGetChanged) == 0x00) { continue; }

        if (error_count >= 50) {
            OutputError(wepet::IntToStr(error_count) +
              ". error - aborting download");
            return false;
        }

        if (! DownloadSetAddress(page * flash_.PageSizeGet())) {
            error_count++;
            page--;
            continue;
        }

        if (! DownloadPage(page)) {
            error_count++;
            page--;
            continue;
        }

        written_pages++;
        if (verbosity_ < vbDebug) {
            progressbar_.Update(written_pages);
        }
    }

    if (verbosity_ < vbDebug) {
        OutputLine("", vbSilent);
    }
    return true;
}

//**************************[DownloadSetAddress]*******************************
bool cAvrDownloader::DownloadSetAddress(const int address) {

    // check input
    if ((address < 0) || (address >= flash_.FlashSizeGet())) {
        return false;
    }

    // clear comport
    comport_.HWBufferFlush(true, true);
    comport_.BufferClear();

    // set address
    std::string send;
    std::string receive;
    if (programmer_ == ptFlash1) {
        send = "A";
        send+= (char) (((address >> 1)      ) & 0xFF);
        send+= (char) (((address >> 1) >>  8) & 0xFF);
        if (flash_.FlashSizeGet() <= 0x10000) {
            send+= (char) (((address >> 1) >> 16) & 0xFF);
        }
        receive = "\r";

    } else {
        switch (programmer_) {
            case ptFlash2: send = "A"     ; receive = "a"; break;
            case ptFlash3: send = "Aax"   ; receive = "a"; break;
            case ptFlash4: send = "setadr"; receive = "A"; break;

            default      : DownloadError("unknown bootloader version (" +
                wepet::IntToHex(programmer_, -2) + ")"); return false;
        }
        std::string adr = "???";
        adr[0] = (char) ( address        & 0xFF);
        adr[1] = (char) ((address >>  8) & 0xFF);
        adr[2] = (char) ((address >> 16) & 0xFF);

        send+= adr;
        receive+= adr + '\r';
    }

    std::string error = "failed to set address (" +
      wepet::IntToHex(address)+ ")";
    if (! DownloadCommand(send, receive, error)) { return false; }

    return true;
}

//**************************[DownloadPage]*************************************
bool cAvrDownloader::DownloadPage(const int page) {

    // check input
    if ((page < 0) || (page >= flash_.FlashSizeGet())) {
        return false;
    }

    // clear comport
    comport_.HWBufferFlush(true, true);
    comport_.BufferClear();

    // clear page buffer
    if (programmer_ <  ptFlash4) {
        std::string send;
        std::string receive;

        switch (programmer_) {
            case ptFlash1: send = "E"  ; receive = "\r" ; break;
            case ptFlash2: send = "E"  ; receive = "e\r"; break;
            case ptFlash3: send = "Eex"; receive = "e\r"; break;

            default      : DownloadError("unknown bootloader version (" +
                wepet::IntToHex(programmer_, -2) + ")"); return false;
        }
        std::string error = "failed to clear page buffer";
        if (! DownloadCommand(send, receive, error, 0, 1)) { return false; }
    }

    // download page buffer
    std::string data;
    int crc = 0;

    for (int word = 0; word < flash_.PageSizeGet() / 2; word++) {
        uint8_t lo = flash_.DataLoGet(page, word);
        uint8_t hi = flash_.DataHiGet(page, word);

        data+= (char) lo; crc+=lo;
        data+= (char) hi; crc+=hi;
    }
    crc&= 0xFF;

    std::string send;
    std::string receive;
    if (programmer_ == ptFlash1) {
        send = "D" + data;
        receive+= (char) crc;
    } else {
        switch (programmer_) {
            case ptFlash2: send = "D"    ; receive = "d"; break;
            case ptFlash3: send = "Ddx"  ; receive = "d"; break;
            case ptFlash4: send = "write"; receive = "W"; break;

            default      : DownloadError("unknown bootloader version (" +
                wepet::IntToHex(programmer_, -2) + ")"); return false;
        }
        send+= data;
        receive+= (char) crc;
        receive+= '\r';
    }
    std::string error = "failed to fill page buffer";
    if (! DownloadCommand(send, receive, error, 0, 1)) { return false; }

    // write page buffer
    if (programmer_ <  ptFlash4) {
        std::string send;
        std::string receive;

        switch (programmer_) {
            case ptFlash1: send = "P"  ; receive = "\r" ; break;
            case ptFlash2: send = "P"  ; receive = "p\r"; break;
            case ptFlash3: send = "Ppx"; receive = "p\r"; break;

            default      : DownloadError("unknown bootloader version (" +
                wepet::IntToHex(programmer_, -2) + ")"); return false;
        }
        std::string error = "failed to write page buffer";
        if (! DownloadCommand(send, receive, error, 0, 1)) { return false; }
    }

    return true;
}

//**************************[DownloadVerify]***********************************
bool cAvrDownloader::DownloadVerify() {

    if (! flag_verify_flash_) {
        OutputLine("flash verification deactivated",vbVerbose);
        return true;
    }

    if (programmer_ < ptFlash4) {
        OutputLine("bootloader does not support verification");
        return true;
    }

    if (verbosity_ < vbDebug) {
        progressbar_.Reset("Verifying ");
        progressbar_.Update(0);
    }

    int verified_pages = 0;
    int error_count = 0;
    int error_count_same_page = 0;

    for (int page = flash_.PageMinGet(); page <= flash_.PageMaxGet(); page++) {
        if ((flash_.PageStateGet(page) & fsGetChanged) == 0x00) { continue; }

        if (error_count >= 50) {
            OutputError(wepet::IntToStr(error_count) +
              ". error - aborting verification");
            return false;
        }
        if (error_count_same_page >= 5) {
            OutputError(wepet::IntToStr(error_count) +
              ". error at same page - aborting verification");
            return false;
        }

        if (! DownloadSetAddress(page * flash_.PageSizeGet())) {
            error_count++;
            page--;
            continue;
        }

        if (! DownloadVerifyPage(page)) {
            error_count++;
            page--;
            continue;
        }

        error_count_same_page = 0;
        verified_pages++;
        if (verbosity_ < vbDebug) {
            progressbar_.Update(verified_pages);
        }
    }

    if (verbosity_ < vbDebug) {
        OutputLine("", vbSilent);
    }
    return true;
}

//**************************[DownloadVerifyPage]*******************************
bool cAvrDownloader::DownloadVerifyPage(const int page) {

    // check input
    if ((page < 0) || (page >= flash_.FlashSizeGet())) {
        return false;
    }

    // clear comport
    comport_.HWBufferFlush(true, true);
    comport_.BufferClear();

    // check programmer
    if (programmer_ <  ptFlash4) {
        OutputError("programmer does not support verification");
        return false;
    }

    // download page buffer
    std::string send = "read";
    int receive_count = 3 + flash_.PageSizeGet();

    std::string error = "failed to read page";
    if (! DownloadCommand(send, "", error, receive_count, 1)) {
        return false;
    }

    std::string data = comport_.BufferGet();
    if (data.size() < receive_count) { DownloadError(error); return false; }
    if ((data[0] != 'R') && (data[flash_.PageSizeGet() + 2] != '\r')) {
        DownloadError(error);
        return false;
    }

    int crc = 0;
    for (int word = 0; word < flash_.PageSizeGet() / 2; word++) {
        char lo = (char) flash_.DataLoGet(page, word);
        char hi = (char) flash_.DataHiGet(page, word);
        if ((data[word * 2 + 1] != lo) || (data[word * 2 + 2] != hi)) {
            DownloadError("failed to verify word " + wepet::IntToStr(word) +
              " on page " + wepet::IntToStr(page));
            return false;
        }
        crc+= lo + hi;
    }
    crc&= 0xFF;

    if (data[flash_.PageSizeGet() + 1] != (char) crc) {
        DownloadError("wrong checksum");
        return false;
    }

    return true;
}

//**************************[DownloadExit]*************************************
bool cAvrDownloader::DownloadExit() {

    // clear comport
    comport_.HWBufferFlush(true, true);
    comport_.BufferClear();

    // stop bootloader and run program
    std::string send;
    std::string receive;
    switch (programmer_) {
        case ptFlash1: send = "X"   ; receive = "Q"  ; break;
        case ptFlash2: send = "X"   ; receive = "x\r"; break;
        case ptFlash3: send = "Xxx" ; receive = "x\r"; break;
        case ptFlash4: send = "exit"; receive = "@"  ; break;

        default      : DownloadError("unknown bootloader version (" +
            wepet::IntToStr(programmer_) + ")"); return false;
    }

    std::string error = "failed to start controller";
    if (! DownloadCommand(send, receive, error)) { return false; }

    return true;
}

//**************************[DownloadError]************************************
void cAvrDownloader::DownloadError(std::string error) {

    OutputLine(error);

    for (int i = 0; i < 5; i++) {
        comport_.HWBufferFlush(true, true);
        comport_.BufferClear();

        comport_.Wait(100);
        if (comport_.HWBufferInSizeGet() == 0) {
            return;
        }
    }
}

//**************************[DownloadCommand]**********************************
bool cAvrDownloader::DownloadCommand(const std::string transmit,
  const char receive_char, const std::string error, const int retry) {

    std::string str;
    str+= receive_char;
    return DownloadCommand( transmit, str, error, retry);
}

//**************************[DownloadCommand]**********************************
bool cAvrDownloader::DownloadCommand(const std::string transmit,
  const int receive_count, const std::string error, const int retry) {

    return DownloadCommand( transmit, "", error, receive_count, retry);
}

//**************************[DownloadCommand]**********************************
bool cAvrDownloader::DownloadCommand(const std::string transmit,
  const std::string receive, const std::string error,
  const int receive_count, const int retry, const std::string debug,
  const bool recurs) {

    int count = 0;

    while (count++ < retry) {
        std::string str_count;
        if (count > 1) { str_count+= " (" + wepet::IntToStr(count) + ")"; }

        if ((count > 1) && recurs) {
            if (! DownloadCommand("?", programmer_id_, "Recovery failed",
              0, 3, "Recover", false)) {
                return false;
            }
        }

        comport_.HWBufferFlush(true, true);
        comport_.BufferClear();

        if (verbosity_ == vbDebug) {
            OutputLine("  " + debug + str_count, vbDebug);
            OutputLine("    transmit string: " +
              wepet::StrOut(transmit, 20), vbDebug);
            if (receive != "") {
                OutputLine("    expected string: " +
                  wepet::StrOut(receive, 20), vbDebug);
            } else {
                OutputLine("    expected count : " +
                  wepet::IntToStr(receive_count) + " bytes", vbDebug);
            }
        }

        if (! comport_.Transmit(transmit)) {
            OutputLine("    failed to transmit" + str_count, vbVerbose);
            continue;
        }
        bool wait = false;
        if (receive != "") {
            wait = comport_.BufferWait(receive);
        } else {
            wait = comport_.BufferWait(receive_count);
        }
        if (! wait) {
            if ((verbosity_ == vbVerbose) && (count == 1)) {
                OutputLine("  " + debug + str_count, vbVerbose);
            }
            OutputLine("    failed to receive" + str_count, vbVerbose);
            continue;
        }

        OutputLine("    ok  :-)", vbDebug);
        if (count > 1) {
            OutputLine("", vbVerbose);
        } else {
            OutputLine("", vbDebug);
        }
        return true;
    }

    if (error != "") {
        DownloadError(error);
    } else {
        OutputLine("", vbVerbose);
    }

    return false;
}

//**************************[OutputHeader]*************************************
void cAvrDownloader::OutputHeader(void) {
    std::cout                                                   << std::endl;
    std::cout << "AVR-Downloader"                               << std::endl;
    std::cout << "=============="                               << std::endl;
    std::cout                                                   << std::endl;
    std::cout << "Version: " << AVR_DOWNLOADER_VERSION          << std::endl;
    std::cout << "Date   : " << AVR_DOWNLOADER_DATE             << std::endl;
    std::cout << "Author : Peter Weissig"                       << std::endl;
    std::cout << "Website: " <<
      "https://github.com/peterweissig/cpp_avr_downloader"      << std::endl;
    std::cout                                                   << std::endl;
}

//**************************[OutputHelp]***************************************
void cAvrDownloader::OutputHelp(void) {
    std::cout                                                   << std::endl;
    std::cout << "Usage: downloader [options]"                  << std::endl;
    std::cout << "Options:"                                     << std::endl;

    std::cout << "  -I <infile>      " <<
      "Required. Specify input file."                           << std::endl;

    std::cout << "  -i <infile>      " <<
      "Specify a input file for comparison."                    << std::endl;

    std::cout << "  -O <outfile>     " <<
      "Specify output  file containing all data."               << std::endl;

    std::cout << "  -C <outfile>     " <<
      "Specify output  file containing changed data."           << std::endl;

    std::cout                                                   << std::endl;

    std::cout << "  -R <steps>       " <<
      "Specify a step of the reset sequence."                   << std::endl;

    std::cout << "     + 10..2000    " <<
      "Delay in milliseconds."                                  << std::endl;

    std::cout << "     + DTR+        " <<
      "Set DTR."                                                << std::endl;

    std::cout << "     + RTS+        " <<
      "Set RTS."                                                << std::endl;

    std::cout << "     + DTR-        " <<
      "Clear DTR."                                              << std::endl;

    std::cout << "     + RTS-        " <<
      "Clear RTS."                                              << std::endl;

    std::cout                                                   << std::endl;

    std::cout << "  -p <partno>      " <<
      "Required. Specify AVR device."                           << std::endl;

    std::cout << "  -b <baudrate>    " <<
      "Override RS-232 baud rate. [default 57600]"              << std::endl;

    std::cout << "  -c <programmer>  " <<
      "Specify programmer type: [default auto]"                 << std::endl;

    std::cout << "     + Auto        " <<
      "Auto detect version."                                    << std::endl;

    std::cout << "     + Flash       " <<
      "Use flash bootloader version 1."                         << std::endl;

    std::cout << "     + Flash2      " <<
      "Use flash bootloader version 2."                         << std::endl;

    std::cout << "     + Flash3      " <<
      "Use flash bootloader version 3."                         << std::endl;

    std::cout << "     + Flash4      " <<
      "Use flash bootloader version 4."                         << std::endl;

    std::cout << "  -P <port>        " <<
      "Specify connection port [default /dev/ttyS0]"            << std::endl;

    std::cout << "  -T <timeout>     " <<
      "Specify max. time commands [default 200ms]"              << std::endl;

    std::cout                                                   << std::endl;

    std::cout << "  -F               " <<
      "Override invalid signature check."                       << std::endl;

    std::cout << "  -n               " <<
      "Do not download."                                        << std::endl;

    std::cout << "  -V               " <<
      "Do not verify."                                          << std::endl;

    std::cout                                                   << std::endl;

    std::cout << "  -v               " <<
      "Verbose output."                                         << std::endl;

    std::cout << "  -q               " <<
      "Quell output."                                           << std::endl;

    std::cout << "  -?               " <<
      "Display this usage"                                      << std::endl;

    std::cout                                                   << std::endl;
    std::cout                                                   << std::endl;
}

//**************************[OutputOptions]************************************
void cAvrDownloader::OutputOptions(void) {
    std::cout << "Settings:"                                    << std::endl;

    std::cout << "  Input file     : " << filename_in_          << std::endl;
    std::cout << "  Output file    : " << filename_out_         << std::endl;
    std::cout << "  Compare file   : " << filename_compare_     << std::endl;
    std::cout << "  Changed file   : " << filename_changed_     << std::endl;

    std::cout << "  Partno         : " << mcu_name_             << std::endl;

    std::cout << "  Programmer     : ";
    switch (programmer_) {
        case ptNone   : std::cout << "None"   << std::endl; break;
        case ptAuto   : std::cout << "Auto"   << std::endl; break;
        case ptFlash1 : std::cout << "Flash1" << std::endl; break;
        case ptFlash2 : std::cout << "Flash2" << std::endl; break;
        case ptFlash3 : std::cout << "Flash3" << std::endl; break;
        case ptFlash4 : std::cout << "Flash4" << std::endl; break;
        default       : std::cout << "Error"  << std::endl; break;
    }
    std::cout << "  Port           : " << comport_name_         << std::endl;
    std::cout << "  Baudrate       : " << baudrate_             << std::endl;
    std::cout << "  TimeOut        : " << command_timeout_      << std::endl;

    std::cout << "  Signature check: " <<
      wepet::BoolToStr(flag_verify_signature_) << std::endl;
    std::cout << "  Download flash : " <<
      wepet::BoolToStr(flag_download_) << std::endl;
    std::cout << "  Verify flash   : " <<
      wepet::BoolToStr(flag_verify_flash_) << std::endl;

    std::cout << "  Verbosity      : ";
    switch (verbosity_) {
        case vbSilent  : std::cout << "Silent"  << std::endl; break;
        case vbQuiet   : std::cout << "Quiet"   << std::endl; break;
        case vbInfo    : std::cout << "Info"    << std::endl; break;
        case vbVerbose : std::cout << "Verbose" << std::endl; break;
        case vbDebug   : std::cout << "Debug"   << std::endl; break;
        default        : std::cout << "Error"   << std::endl; break;
    }

    std::cout                                                   << std::endl;
}

//**************************[OutputOptions]************************************
void cAvrDownloader::OutputDevice(void) {
    std::cout << "Device:"                                      << std::endl;

    std::cout << "  ID             : " << device_id_            << std::endl;
    std::cout << "  Description    : " << device_desc_          << std::endl;
    std::cout << "  Page size      : " << device_page_size_     << std::endl;
    std::cout << "  Number of pages: " << device_page_count_    << std::endl;

    std::cout << "  Signature      : " << ConvertToHex(device_signature_) <<
      std::endl;

    std::cout                                                   << std::endl;
}

//**************************[OutputResetSequence]*******************************
void cAvrDownloader::OutputResetSequence(void) {

    std::cout << "Reset sequence:" << std::endl;

    for (int i = 0; i < reset_.size(); i++) {
        switch (reset_[i].type_) {
            case rsNone:
                std::cout << "  [None]" << std::endl;
                break;

            case rsWait:
                std::cout << "  Wait " <<
                  wepet::IntToStr(reset_[i].value_, 4) << "ms" << std::endl;
                break;

            case rsRts:
                if (reset_[i].value_) {
                    std::cout << "  Set   RTS" << std::endl;
                } else {
                    std::cout << "  Clear RTS" << std::endl;
                }
                break;

            case rsDtr:
                if (reset_[i].value_) {
                    std::cout << "  Set   DTR" << std::endl;
                } else {
                    std::cout << "  Clear DTR" << std::endl;
                }
                break;

            default:
                std::cout << "  [Error]" << std::endl;
                break;
        }
    }
    std::cout << std::endl;
}

//**************************[OutputLine]***************************************
void cAvrDownloader::OutputLine(const std::string line,
  const eVerbosity verbosity) {

    if (verbosity_ >= verbosity) {
        progressbar_.Skip();
        std::cout << line << std::endl;
    }
}

//**************************[OutputParseError]*********************************
void cAvrDownloader::OutputError(const std::string error) {

    if (verbosity_ > vbSilent) {
        progressbar_.Skip();
        std::cout << std::endl << "Error - " << error <<  std::endl;
    }
}

//**************************[OutputParseError]*********************************
void cAvrDownloader::OutputParseError(const std::string option) {

    if (verbosity_ >= vbInfo) {
        OutputHeader();
    }

    OutputError("parsing " + option);
}

//**************************[ConvertToHex]*************************************
std::string cAvrDownloader::ConvertToHex(const std::string data) {

    std::string result;
    for (int i = 0; i < data.size(); i++) {
        if (i > 0) { result+= ' '; }
        result+= wepet::IntToHex((int) data[i],-2);
    }

    return result;
}

} //namespace wepet {
