/******************************************************************************
*                                                                             *
* wepet_comport.h                                                             *
* ===============                                                             *
*                                                                             *
* Version: 1.1.4                                                              *
* Date   : 18.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_comport/                              *
******************************************************************************/

#ifndef __WEPET_COMPORT_H
#define __WEPET_COMPORT_H

// local headers

// wepet headers

// standard headers
#include <string>
#include <stdint.h>

// additional headers
#if (defined(__WIN32) || defined(__WIN64))
#else
    #include <termios.h>
#endif //#if (defined(__WIN32) || defined(__WIN64))



namespace wepet {

enum eComPortByteSize {
    kCpByteSize5 = 5,
    kCpByteSize6 = 6,
    kCpByteSize7 = 7,
    kCpByteSize8 = 8
};

enum eComPortStopBits {
    kCpStopBits1 = 0,
    kCpStopBits2 = 2
};

enum eComPortParity {
    kCpParityNone  = 0,
    kCpParityOdd   = 1,
    kCpParityEven  = 2,
    kCpParityMark  = 3,
    kCpParitySpace = 4
};

//*****************************************************************************
//**************************{class cComPort}***********************************
//*****************************************************************************
class cComPort {
  public:
    cComPort(void);
    ~cComPort(void);

    bool Open(std::string port_name);
    bool IsOpened(void);
    void Close(void);

    bool Transmit(std::string text);
    std::string Receive(void);

    bool LineRtsSet(bool state);
    bool LineDtrSet(bool state);

    // this 3 functions are only for windows
    int HWBufferInSizeGet(void);
    int HWBufferOutSizeGet(void);
    bool HWBufferSizeSet(int buffer_in_size, int buffer_out_size);

    int HWBufferInCountGet(void);
    int HWBufferOutCountGet(void);
    bool HWBufferFlush(bool buffer_in, bool buffer_out);

    int              SettingBaudRateGet(void);
    eComPortByteSize SettingByteSizeGet(void);
    eComPortStopBits SettingStopBitsGet(void);
    eComPortParity   SettingParityGet  (void);
    bool SettingBaudRateSet(int baud_rate);
    bool SettingByteSizeSet(eComPortByteSize byte_size);
    bool SettingStopBitsSet(eComPortStopBits stop_bits);
    bool SettingParitySet  (eComPortParity parity);

  private:
    // internal system-dependend variables
    #if (defined(__WIN32) || defined(__WIN64))
        int port_file;
        HANDLE port_file;
        DCB port_settings;
        int port_buffer_in_size;
        int port_buffer_out_size;
    #else
        int port_file;
        termios port_settings, port_settings_old;
        int port_baudrate;
    #endif //#if (defined(__WIN32) || defined(__WIN64))
};

//*****************************************************************************
//**************************{class cComPortBuffer}*****************************
//*****************************************************************************
class cComPortBuffer : public cComPort {
  public:
    cComPortBuffer(void);
    ~cComPortBuffer(void);

    std::string BufferGet(void) const;
    void BufferUpdate(void);
    void BufferClear(void);

    bool BufferWait(int count);
    bool BufferWait(std::string text);
    void BufferTimeSet(int milliseconds);

    void Wait(int milliseconds) const;

  private:
    int64_t GetCurrentTime(void) const;
    // this function is only for linux to allow non-blocking sleep
    void SleepOneMilliSecond(void) const;

    std::string receive_buffer;
    int receive_time;
};

} // namespace wepet {
#endif // #ifndef __WEPET_COMPORT_H
