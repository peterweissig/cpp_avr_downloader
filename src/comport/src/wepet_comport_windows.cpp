/******************************************************************************
*                                                                             *
* wepet_comport_windows.cpp                                                   *
* =========================                                                   *
*                                                                             *
* Version: 1.1.4                                                              *
* Date   : 18.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_comport/                              *
*                                                                             *
* Untested since several versions                                             *
******************************************************************************/

// local headers
#include "wepet_comport.h"

// wepet headers

// standard headers
#include <iostream>
//#include <fstream>
//#include <sstream>

// additional headers
#include <windows.h>

namespace wepet {

//**************************[cComPort]*****************************************
cComPort::cComPort() {

    port_file = INVALID_HANDLE_VALUE;

    port_settings.BaudRate =         57600;
    port_settings.ByteSize =  kCpByteSize8;
    port_settings.StopBits =  kCpStopBits2;
    port_settings.Parity   = kCpParityNone;

    port_settings.fBinary       = true;
    port_settings.fParity       = false;
    port_settings.fAbortOnError = false;

    port_settings.fOutxCtsFlow     = false;
    port_settings.fOutxDsrFlow     = false;
    port_settings.fDsrSensitivity  = false;
    port_settings.fDtrControl      = DTR_CONTROL_DISABLE;
    port_settings.fRtsControl      = RTS_CONTROL_DISABLE;

    port_settings.fTXContinueOnXoff = true;
    port_settings.fOutX             = false;
    port_settings.fInX              = false;

    port_settings.fErrorChar = false;
    port_settings.fNull      = false;
    port_settings.ErrorChar  = 0;
    port_settings.EofChar    = 0;
    port_settings.EvtChar    = 0;

    port_settings.XonLim     = 0;
    port_settings.XoffLim    = 0;
    port_settings.XoffChar   = 0;
    port_settings.XonChar    = 0;

    port_settings.fDummy2    = 0;
    port_settings.wReserved  = 0;
    port_settings.wReserved1 = 0;


    port_buffer_in_size  = 256;
    port_buffer_out_size = 256;
}

//**************************[~cComPort]****************************************
cComPort::~cComPort() {

    Close();
}

//**************************[Open]*********************************************
bool cComPort::Open(std::string port_name) {

    if (IsOpened()) {
        Close();
    }

    receive_buffer = "";

    port_file = CreateFile(port_name.data(), GENERIC_READ | GENERIC_WRITE, 0,
      NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (! IsOpened()) { return false; }


    if (! SetupComm(port_file, port_buffer_in_size, port_buffer_out_size)) {
        Close();
        return false;
    }

    COMMTIMEOUTS temp_timeouts;
    temp_timeouts.ReadIntervalTimeout        = 0;
    temp_timeouts.ReadTotalTimeoutMultiplier = 0;
    temp_timeouts.ReadTotalTimeoutConstant   = 0;
    temp_timeouts.WriteTotalTimeoutMultiplier = 0;
    temp_timeouts.WriteTotalTimeoutConstant   = 0;
    if (! SetCommTimeouts(port_file, &temp_timeouts)) {
        Close();
        return false;
    }

    if (! SetCommState(port_file, &port_settings)) {
        Close();
        return false;
    }

    return true;
}

//**************************[IsOpened]*****************************************
bool cComPort::IsOpened() {

    return (port_file != INVALID_HANDLE_VALUE);
}

//**************************[Close]********************************************
void cComPort::Close() {

    if (! IsOpened()) {
        return;
    }

    CloseHandle(port_file);
    port_file = INVALID_HANDLE_VALUE;
}

//**************************[Transmit]*****************************************
bool cComPort::Transmit(std::string text) {

    DWORD count;

    if (! IsOpened()) {
        return false;
    }

    if (! WriteFile(port_file, &(text[0]),text.size(),&count,NULL)) {
        return false;
    }

    if (count != text.size()) {
        return false;
    }

    return true;
}

//**************************[Receive]******************************************
std::string cComPort::Receive() {

    int count_in;
    DWORD count_out;
    std::string result;

    if (! IsOpened()) {
        return "";
    }

    count_in = HWBufferInCountGet();

    if (count_in < 1) {return "";}

    result.resize(count_in);

    if (! ReadFile(port_file, &(result[0]),count_in,&count_out,NULL)) {
        return "";
    }

    result.resize(count_out);

    return result;
}

//**************************[LineRtsSet]***************************************
bool cComPort::LineRtsSet(bool state) {

    DWORD flag;
    if (! IsOpened()) {
        return false;
    }

    if (state) { flag = SETRTS;} else { flag = CLRRTS;}

    if (! EscapeCommFunction(port_file, flag)) {
        return false;
    }

    return true;
}

//**************************[LineDtrSet]***************************************
bool cComPort::LineDtrSet(bool state) {

    DWORD flag;
    if (! IsOpened()) {
        return false;
    }

    if (state) { flag = SETDTR;} else { flag = CLRDTR;}

    if (! EscapeCommFunction(port_file, flag)) {
        return false;
    }

    return true;
}

//**************************[HWBufferInSizeGet]********************************
int cComPort::HWBufferInSizeGet() {

    return port_buffer_in_size;
}

//**************************[HWBufferOutSizeGet]*******************************
int cComPort::HWBufferOutSizeGet() {

    return port_buffer_out_size;
}

//**************************[HWBufferSizeSet]**********************************
bool cComPort::HWBufferSizeSet(int buffer_in_size, int buffer_out_size) {

    if (IsOpened()) {
        if (! SetupComm(port_file, buffer_in_size, buffer_out_size)) {
            return false;
        }
    }

    port_buffer_in_size = buffer_in_size;
    port_buffer_out_size = buffer_out_size;
    return true;
}

//**************************[HWBufferInCountGet]*******************************
int cComPort::HWBufferInCountGet() {

    COMSTAT temp_stat;
    DWORD temp;

    if (! IsOpened()) {
        return -1;
    }

    if (! ClearCommError(port_file, &temp, &temp_stat)) {
        return -1;
    }


    return temp_stat.cbInQue;
}

//**************************[HWBufferOutCountGet]******************************
int cComPort::HWBufferOutCountGet() {

    COMSTAT temp_stat;
    DWORD temp;

    if (! IsOpened()) {
        return -1;
    }

    if (! ClearCommError(port_file, &temp, &temp_stat)) {
        return -1;
    }


    return temp_stat.cbOutQue;
}

//**************************[HWBufferFlush]************************************
bool cComPort::HWBufferFlush(bool buffer_in, bool buffer_out) {

    DWORD temp_flags;

    if (! IsOpened()) { return true;}
    if ((buffer_in == false) && (buffer_out == false)) {return true;}

    if (buffer_in)  {temp_flags|= PURGE_RXCLEAR | PURGE_RXABORT;}
    if (buffer_out) {temp_flags|= PURGE_TXCLEAR | PURGE_TXABORT;}

    if (! PurgeComm(port_file, temp_flags)) {
        return false;
    }

    return true;
}

//**************************[SettingBaudRateGet]*******************************
int cComPort::SettingBaudRateGet() {

    return port_settings.BaudRate;
}

//**************************[SettingByteSizeGet]*******************************
eComPortByteSize cComPort::SettingByteSizeGet() {

    return (eComPortByteSize) port_settings.ByteSize;
}

//**************************[SettingStopBitsGet]*******************************
eComPortStopBits cComPort::SettingStopBitsGet() {

    return (eComPortStopBits) port_settings.StopBits;
}

//**************************[SettingParityGet]*********************************
eComPortParity cComPort::SettingParityGet() {

    return (eComPortParity) port_settings.Parity;
}

//**************************[SettingBaudRateSet]*******************************
bool cComPort::SettingBaudRateSet(int baud_rate) {

    DWORD temp_baud_rate;

    temp_baud_rate = port_settings.BaudRate;
    port_settings.BaudRate = baud_rate;

    if (! IsOpened()) {
        return true;
    }

    if (! SetCommState(port_file, &port_settings)) {
        port_settings.BaudRate = temp_baud_rate;
        return false;
    }

    return true;
}

//**************************[SettingByteSizeSet]*******************************
bool cComPort::SettingByteSizeSet(eComPortByteSize byte_size) {

    int temp_byte_size;

    temp_byte_size = port_settings.ByteSize;
    port_settings.ByteSize = byte_size;

    if (! IsOpened()) {
        return true;
    }

    if (! SetCommState(port_file, &port_settings)) {
        port_settings.ByteSize = temp_byte_size;
        return false;
    }

    return true;
}

//**************************[SettingStopBitsSet]*******************************
bool cComPort::SettingStopBitsSet(eComPortStopBits stop_bits) {

    int temp_stop_bits;

    temp_stop_bits = port_settings.StopBits;
    port_settings.StopBits = stop_bits;

    if (! IsOpened()) {
        return true;
    }

    if (! SetCommState(port_file, &port_settings)) {
        port_settings.StopBits = temp_stop_bits;
        return false;
    }

    return true;
}

//**************************[SettingParitySet]*********************************
bool cComPort::SettingParitySet(eComPortParity parity) {

    int temp_parity;

    temp_parity = port_settings.Parity;
    port_settings.Parity= parity;

    if (! IsOpened()) {
        return true;
    }

    if (! SetCommState(port_file, &port_settings)) {
        port_settings.Parity = temp_parity;
        return false;
    }

    return true;
}

//**************************[GetCurrentTime]***********************************
int64_t cComPortBuffer::GetCurrentTime() const {

    return (int64_t) GetTickCount();
}

//**************************[SleepOneMilliSecond]******************************
void cComPortBuffer::SleepOneMilliSecond() const {

    // this function is mainly for linux to allow non-blocking sleep
}


} // namespace wepet {
