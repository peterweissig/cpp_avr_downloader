/******************************************************************************
*                                                                             *
* wepet_comport_linux.cpp                                                     *
* =======================                                                     *
*                                                                             *
* Version: 1.1.3                                                              *
* Date   : 11.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_comport/                              *
******************************************************************************/

// local headers
#include "wepet_comport.h"

// wepet headers

// standard headers
#include <fstream>

// additional headers
#include <sys/time.h>

#include <fcntl.h>
#include <termio.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <unistd.h>



namespace wepet {

//**************************[cComPort]*****************************************
cComPort::cComPort() {

    port_file = -1;

    port_baudrate = 57600;

    port_settings.c_iflag = 0;
    port_settings.c_iflag|= IGNBRK ; // ignore BREAK condition
    port_settings.c_iflag|= IGNPAR ; // ignore (discard) parity errors
    /*
    port_settings.c_iflag|= BRKINT ; // map BREAK to SIGINTR
    port_settings.c_iflag|= PARMRK ; // mark parity and framing errors
    port_settings.c_iflag|= INPCK  ; // enable checking of parity errors
    port_settings.c_iflag|= ISTRIP ; // strip 8th bit off chars
    port_settings.c_iflag|= IGNCR  ; // ignore CR
    port_settings.c_iflag|= INLCR  ; // map NL into CR
    port_settings.c_iflag|= ICRNL  ; // map CR to NL (ala CRMOD)
    port_settings.c_iflag|= IXON   ; // enable output flow control
    port_settings.c_iflag|= IXOFF  ; // enable input flow control
    port_settings.c_iflag|= IXANY  ; // any char will restart after stop
    port_settings.c_iflag|= IMAXBEL; // ring bell on input queue full
    */

    port_settings.c_oflag = 0;
    /*
    port_settings.c_oflag|= OPOST ; // enable following output processing
    port_settings.c_oflag|= ONLCR ; // map NL to CR-NL (ala CRMOD)
    port_settings.c_oflag|= OCRNL ; // map CR to NL
    port_settings.c_oflag|= TABDLY; // tab delay mask
    port_settings.c_oflag|= TAB0  ; // no tab delay and expansion
    port_settings.c_oflag|= TAB3  ; // expand tabs to spaces
    port_settings.c_oflag|= ONOEOT; // discard EOT's '^D' on output)
    port_settings.c_oflag|= ONOCR ; // do not transmit CRs on column 0
    port_settings.c_oflag|= ONLRET; // on the terminal NL performs the CR function
    */

    port_settings.c_cflag = 0;
    port_settings.c_cflag|= CS8       ; // 8 bits
    port_settings.c_cflag|= CSTOPB    ; // send 2 stop bits
    port_settings.c_cflag|= CREAD     ; // enable receiver
    port_settings.c_cflag|= CLOCAL    ; // ignore modem status lines
    port_settings.c_cflag|= B38400    ;
    /*
    port_settings.c_cflag|= CSIZE     ; // character size mask
    port_settings.c_cflag|= CS5       ; // 5 bits (pseudo)
    port_settings.c_cflag|= CS6       ; // 6 bits
    port_settings.c_cflag|= CS7       ; // 7 bits
    port_settings.c_cflag|= PARENB    ; // parity enable
    port_settings.c_cflag|= PARODD    ; // odd parity, else even
    port_settings.c_cflag|= PAREXT    ; // Extended parity for mark and space parity
    port_settings.c_cflag|= HUPCL     ; // hang up on last close
    port_settings.c_cflag|= CCTS_OFLOW; // CTS flow control of output
    port_settings.c_cflag|= CRTSCTS   ; // same as CCTS_OFLOW
    port_settings.c_cflag|= CRTS_IFLOW; // RTS flow control of input
    port_settings.c_cflag|= MDMBUF    ; // flow control output via Carrier
    */

    port_settings.c_lflag = 0;
    /*
    port_settings.c_lflag|= ECHOKE    ; // visual erase for line kill
    port_settings.c_lflag|= ECHOE     ; // visually erase chars
    port_settings.c_lflag|= ECHO      ; // enable echoing
    port_settings.c_lflag|= ECHONL    ; // echo NL even if ECHO is off
    port_settings.c_lflag|= ECHOPRT   ; // visual erase mode for hardcopy
    port_settings.c_lflag|= ECHOCTL   ; // echo control chars as ^(Char)
    port_settings.c_lflag|= ISIG      ; // enable signals INTR, QUIT, [D]SUSP
    port_settings.c_lflag|= ICANON    ; // canonicalize input lines
    port_settings.c_lflag|= ALTWERASE ; // use alternate WERASE algorithm
    port_settings.c_lflag|= IEXTEN    ; // enable DISCARD and LNEXT
    port_settings.c_lflag|= EXTPROC   ; // external processing
    port_settings.c_lflag|= TOSTOP    ; // stop background jobs from output
    port_settings.c_lflag|= FLUSHO    ; // output being flushed (state)
    port_settings.c_lflag|= NOKERNINFO; // no kernel output from VSTATUS
    port_settings.c_lflag|= PENDIN    ; // XXX retype pending input (state)
    port_settings.c_lflag|= NOFLSH    ; // don't flush after interrupt
    */

    port_settings.c_cc[VMIN    ] = 0x00;
    port_settings.c_cc[VTIME   ] = 0x00;
    /*
    port_settings.c_cc[VEOF    ] = 0x00;
    port_settings.c_cc[VEOL    ] = 0x00;
    port_settings.c_cc[VEOL2   ] = 0x00;
    port_settings.c_cc[VERASE  ] = 0x00;
    port_settings.c_cc[VWERASE ] = 0x00;
    port_settings.c_cc[VKILL   ] = 0x00;
    port_settings.c_cc[VREPRINT] = 0x00;
    port_settings.c_cc[VINTR   ] = 0x00;
    port_settings.c_cc[VQUIT   ] = 0x00;
    port_settings.c_cc[VSUSP   ] = 0x00;
    port_settings.c_cc[VDSUSP  ] = 0x00;
    port_settings.c_cc[VSTART  ] = 0x00;
    port_settings.c_cc[VSTOP   ] = 0x00;
    port_settings.c_cc[VLNEXT  ] = 0x00;
    port_settings.c_cc[VDISCARD] = 0x00;
    port_settings.c_cc[VSTATUS ] = 0x00;
    */
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

    port_file = open(port_name.data(), O_RDWR | O_NONBLOCK | O_NOCTTY);
    if (! IsOpened()) { return false; }

    if (tcgetattr(port_file,&port_settings_old) < 0) {
        Close();
        return false;
    }

    if (tcsetattr(port_file,TCSANOW,&port_settings) < 0) {
        Close();
        return false;
    }

    if (tcgetattr(port_file,&port_settings) < 0) {
        Close();
        return false;
    }

    if (! SettingBaudRateSet(port_baudrate)) {
        Close();
        return false;
    }

    return true;
}

//**************************[IsOpened]*****************************************
bool cComPort::IsOpened() {

    return (port_file >= 0);
}

//**************************[Close]********************************************
void cComPort::Close() {

    if (! IsOpened()) {
        return;
    }

    tcsetattr(port_file,TCSANOW,&port_settings_old);

    close(port_file);
    port_file = -1;
}

//**************************[Transmit]*****************************************
bool cComPort::Transmit(std::string text) {

    int count;

    if (! IsOpened()) {
        return false;
    }

    count = write(port_file, &(text[0]),text.size());
    if (count != text.size()) {
        return false;
    }

    return true;
}

//**************************[Receive]******************************************
std::string cComPort::Receive() {

    int count_in;
    int count_out;
    std::string result;

    if (! IsOpened()) {
        return "";
    }

    count_in = HWBufferInCountGet();
    if (count_in < 1) { return ""; }

    result.resize(count_in);
    count_out = read(port_file, &(result[0]),count_in);
    if (count_out < 0) {
        return "";
    }

    result.resize(count_out);
    return result;
}

//**************************[LineRtsSet]***************************************
bool cComPort::LineRtsSet(bool state) {

    int temp_status;

    if (! IsOpened()) {
        return false;
    }

    if (ioctl(port_file, TIOCMGET, &temp_status) == -1) {
        return false;
    }

    if (state) {
        temp_status|=  TIOCM_RTS;
    } else{
        temp_status&= ~TIOCM_RTS;
    }

    if (ioctl(port_file, TIOCMSET, &temp_status) == -1) {
        return false;
    }

    return true;
}

//**************************[LineDtrSet]***************************************
bool cComPort::LineDtrSet(bool state) {

    int temp_status;

    if (! IsOpened()) {
        return false;
    }

    if (ioctl(port_file, TIOCMGET, &temp_status) == -1) {
        return false;
    }

    if (state) {
        temp_status|=  TIOCM_DTR;
    } else{
        temp_status&= ~TIOCM_DTR;
    }

    if (ioctl(port_file, TIOCMSET, &temp_status) == -1) {
        return false;
    }

    return true;
}

//**************************[HWBufferInSizeGet]********************************
int cComPort::HWBufferInSizeGet() {

    // Dummy function - only working in windows
    return -1;
}

//**************************[HWBufferOutSizeGet]*******************************
int cComPort::HWBufferOutSizeGet() {

    // Dummy function - only working in windows
    return -1;
}

//**************************[HWBufferSizeSet]**********************************
bool cComPort::HWBufferSizeSet(int buffer_in_size, int buffer_out_size) {

    // Dummy function - only working in windows
    return true;
}

//**************************[HWBufferInCountGet]*******************************
int cComPort::HWBufferInCountGet() {

    int in_count;

    if (! IsOpened()) {
        return -2;
    }

    if (ioctl(port_file, FIONREAD, &in_count) == -1) {
        return -1;
    }

    return in_count;
}

//**************************[HWBufferOutCountGet]******************************
int cComPort::HWBufferOutCountGet() {

    int out_count;

    if (! IsOpened()) {
        return -2;
    }

    #if defined(FIONWRITE)
        if (ioctl(port_file, FIONWRITE, &out_count) == -1) {
            return -1;
        }

        return out_count;
    #else // #if defined(FIONWRITE)
        return -3;
    #endif // #if defined(FIONWRITE)
}

//**************************[HWBufferFlush]************************************
bool cComPort::HWBufferFlush(bool buffer_in, bool buffer_out) {

    if (! IsOpened()) { return true; }
    if ((buffer_in == false) && (buffer_out == false)) { return true; }

    if (buffer_in) {
        if (buffer_out) {
            return (tcflush(port_file, TCIOFLUSH) != -1);
        } else {
            return (tcflush(port_file, TCIFLUSH) != -1);
        }
    } else {
        if (buffer_out) {
            return (tcflush(port_file, TCOFLUSH) != -1);
        } else {
            return true;
        }
    }
}

//**************************[SettingBaudRateGet]*******************************
int cComPort::SettingBaudRateGet() {

    if (! IsOpened()) {
        return port_baudrate;
    }

    if (tcgetattr(port_file, &port_settings) == -1) {
         return -1;
    }

    switch (port_settings.c_cflag & (CBAUD | CBAUDEX)) {
        case (     B0) : return      0;
        case (    B50) : return     50;
        case (    B75) : return     75;
        case (   B110) : return    110;
        case (   B134) : return    134;
        case (   B150) : return    150;
        case (   B200) : return    200;
        case (   B300) : return    300;
        case (   B600) : return    600;
        case (  B1200) : return   1200;
        case (  B1800) : return   1800;
        case (  B2400) : return   2400;
        case (  B4800) : return   4800;
        case (  B9600) : return   9600;
        case ( B19200) : return  19200;
        case ( B38400) :
            serial_struct temp_serial;
            if (ioctl(port_file, TIOCGSERIAL,&temp_serial) == -1) {
                return 38400;
            }

            if ((temp_serial.flags & ASYNC_SPD_MASK) != ASYNC_SPD_CUST) {
                return  38400;
            }

            if (temp_serial.custom_divisor == 0) {
                return -3;
            }
            return temp_serial.baud_base / temp_serial.custom_divisor;

        case ( B57600)     : return  57600;
        #if defined( B76800)
            case ( B76800) : return  76800;
        #endif
        #if defined(B115200)
            case (B115200) : return 115200;
        #endif
        #if defined(B153600)
            case (B153600) : return 153600;
        #endif
        #if defined(B230400)
            case (B230400) : return 230400;
        #endif
        #if defined(B307200)
            case (B307200) : return 307200;
        #endif
        #if defined(B460800)
            case (B460800) : return 460800;
        #endif

        default        : return     -2;
    }
}

//**************************[SettingByteSizeGet]*******************************
eComPortByteSize cComPort::SettingByteSizeGet() {

    if (IsOpened()) {
        if (tcgetattr(port_file, &port_settings) == -1) {
            return (eComPortByteSize) -2;
        }
    }

    switch (port_settings.c_cflag & CSIZE) {
        case (CS5) : return  kCpByteSize5; break;
        case (CS6) : return  kCpByteSize6; break;
        case (CS7) : return  kCpByteSize7; break;
        case (CS8) : return  kCpByteSize8; break;
        default    : return (eComPortByteSize) -1; break;
    }
}

//**************************[SettingStopBitsGet]*******************************
eComPortStopBits cComPort::SettingStopBitsGet() {

    if (IsOpened()) {
        if (tcgetattr(port_file, &port_settings) == -1) {
            return (eComPortStopBits) -2;
        }
    }

    if (port_settings.c_cflag & CSTOPB) {
        return kCpStopBits2;
    } else {
        return kCpStopBits1;
    }
}

//**************************[SettingParityGet]*********************************
eComPortParity cComPort::SettingParityGet() {

    if (IsOpened()) {
        if (tcgetattr(port_file, &port_settings) == -1) {
            return (eComPortParity) -2;
        }
    }

    if ((port_settings.c_cflag & PARENB) == 0) {
        return kCpParityNone;
    }

    #if defined(PAREXT)
        if (port_settings.c_cflag & PAREXT) {
            if (port_settings.c_cflag & PARODD) {
                return kCpParityOdd;
            } else {
                return kCpParityEven;
            }
        } else {
            if (port_settings.c_cflag & PARODD) {
                return kCpParityMark;
            } else {
                return kCpParitySpace;
            }
        }
    #else // if defined(PAREXT)
        if (port_settings.c_cflag & PARODD) {
            return kCpParityOdd;
        } else {
            return kCpParityEven;
        }
    #endif // if defined(PAREXT)
}

//**************************[SettingBaudRateSet]*******************************
bool cComPort::SettingBaudRateSet(int baud_rate) {

    port_baudrate = baud_rate;

    if (! IsOpened()) {
        return true;
    }

    if (tcgetattr(port_file, &port_settings) == -1) {
        return false;
    }

    port_settings.c_cflag&= ~(CBAUD | CBAUDEX);
    switch (baud_rate) {
        case (     0) : port_settings.c_cflag|=      B0; break;
        case (    50) : port_settings.c_cflag|=     B50; break;
        case (    75) : port_settings.c_cflag|=     B75; break;
        case (   110) : port_settings.c_cflag|=    B110; break;
        case (   134) : port_settings.c_cflag|=    B134; break;
        case (   150) : port_settings.c_cflag|=    B150; break;
        case (   200) : port_settings.c_cflag|=    B200; break;
        case (   300) : port_settings.c_cflag|=    B300; break;
        case (   600) : port_settings.c_cflag|=    B600; break;
        case (  1200) : port_settings.c_cflag|=   B1200; break;
        case (  1800) : port_settings.c_cflag|=   B1800; break;
        case (  2400) : port_settings.c_cflag|=   B2400; break;
        case (  4800) : port_settings.c_cflag|=   B4800; break;
        case (  9600) : port_settings.c_cflag|=   B9600; break;
        case ( 19200) : port_settings.c_cflag|=  B19200; break;
        case ( 38400) : port_settings.c_cflag|=  B38400; break;
        case ( 57600) : port_settings.c_cflag|=  B57600; break;
        #if defined( B76800)
            case ( 76800) : port_settings.c_cflag|=  B76800; break;
        #endif
        #if defined(B115200)
            case (115200) : port_settings.c_cflag|= B115200; break;
        #endif
        #if defined(B153600)
            case (153600) : port_settings.c_cflag|= B153600; break;
        #endif
        #if defined(B230400)
            case (230400) : port_settings.c_cflag|= B230400; break;
        #endif
        #if defined(B307200)
            case (307200) : port_settings.c_cflag|= B307200; break;
        #endif
        #if defined(B460800)
            case (460800) : port_settings.c_cflag|= B460800; break;
        #endif
        default           : port_settings.c_cflag|=  B38400; break;
    }

    if (tcsetattr(port_file, TCSANOW, &port_settings) == -1) {
        return false;
    }

    if ((port_settings.c_cflag & (CBAUD | CBAUDEX)) != B38400) {
        return true;
    }

    serial_struct temp_serial;

    if (ioctl(port_file, TIOCGSERIAL,&temp_serial) == -1) {
        return true;
    }

    if (baud_rate == 38400) {
        if (temp_serial.flags & ASYNC_SPD_MASK) {
            temp_serial.flags&= ~ASYNC_SPD_MASK;

            if (ioctl(port_file, TIOCSSERIAL,&temp_serial) == -1) {
                return false;
            }
        }
    } else {
        temp_serial.flags&= ~ASYNC_SPD_MASK;
        temp_serial.flags|=  ASYNC_SPD_CUST;

        temp_serial.custom_divisor = (temp_serial.baud_base + baud_rate / 2) /
          baud_rate;

        if (ioctl(port_file, TIOCSSERIAL,&temp_serial) == -1) {
            return false;
        }
    }

    return true;
}


//**************************[SettingByteSizeSet]*******************************
bool cComPort::SettingByteSizeSet(eComPortByteSize byte_size) {

    if (IsOpened()) {
        if (tcgetattr(port_file, &port_settings) == -1) {
            return false;
        }
    }

    port_settings.c_cflag&= ~CSIZE;
    switch (byte_size) {
        case (kCpByteSize5) : port_settings.c_cflag|= CS5; break;
        case (kCpByteSize6) : port_settings.c_cflag|= CS6; break;
        case (kCpByteSize7) : port_settings.c_cflag|= CS7; break;
        default             : port_settings.c_cflag|= CS8; break;
    }

    if (IsOpened()) {
        if (tcsetattr(port_file, TCSANOW, &port_settings) == -1) {
            return false;
        }
    }

    return true;
}

//**************************[SettingStopBitsSet]*******************************
bool cComPort::SettingStopBitsSet(eComPortStopBits stop_bits) {

    if (IsOpened()) {
        if (tcgetattr(port_file, &port_settings) == -1) {
            return false;
        }
    }

    if (stop_bits == kCpStopBits1) {
        port_settings.c_cflag&= ~CSTOPB;
    } else {
        port_settings.c_cflag|=  CSTOPB;
    }

    if (IsOpened()) {
        if (tcsetattr(port_file, TCSANOW, &port_settings) == -1) {
            return false;
        }
    }

    return true;
}

//**************************[SettingParitySet]*********************************
bool cComPort::SettingParitySet(eComPortParity parity) {

    if (IsOpened()) {
        if (tcgetattr(port_file, &port_settings) == -1) {
            return false;
        }
    }

    #if defined(PAREXT)
        port_settings.c_cflag&= ~(PARENB | PARODD | PAREXT);
    #else // if defined(PAREXT)
        port_settings.c_cflag&= ~(PARENB | PARODD         );
    #endif // if defined(PAREXT)

    switch (parity) {
        case (kCpParityOdd)   :
            port_settings.c_cflag|= PARENB | PARODD         ; break;
        case (kCpParityEven)  :
            port_settings.c_cflag|= PARENB                  ; break;
        #if defined(PAREXT)
            case (kCpParityMark)  :
                port_settings.c_cflag|= PARENB | PARODD | PAREXT; break;
            case (kCpParitySpace) :
                port_settings.c_cflag|= PARENB |          PAREXT; break;
        #endif // if defined(PAREXT)
        default               :                               break;
    }

    if (IsOpened()) {
        if (tcsetattr(port_file, TCSANOW, &port_settings) == -1) {
            return false;
        }
    }

  return true;
}

//**************************[GetCurrentTime]***********************************
int64_t cComPortBuffer::GetCurrentTime() const {

    timespec time;

    if (clock_gettime(CLOCK_MONOTONIC, &time)) {
        return (uint64_t) -1;
    }

    return (int64_t) time.tv_sec * (int64_t) 1000 + (time.tv_nsec / 1000000);
}

//**************************[SleepOneMilliSecond]******************************
void cComPortBuffer::SleepOneMilliSecond() const {

    usleep(1000);
}

} // namespace wepet {
