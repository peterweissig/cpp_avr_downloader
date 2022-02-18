/******************************************************************************
*                                                                             *
* wepet_comport.cpp                                                           *
* =================                                                           *
*                                                                             *
* Version: 1.1.4                                                              *
* Date   : 18.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_comport/                              *
******************************************************************************/

// local headers
#include "wepet_comport.h"

// wepet headers

// standard headers

// additional headers



#if (defined(__WIN32) || defined(__WIN64))
    #include "wepet_comport_windows.cpp"
#else
    #include "wepet_comport_linux.cpp"
#endif //#if (defined(__WIN32) || defined(__WIN64))

namespace wepet {

//**************************[~cComPortBuffer]**********************************
cComPortBuffer::cComPortBuffer() {

    receive_time = 100;
}

//**************************[~cComPortBuffer]**********************************
cComPortBuffer::~cComPortBuffer() {

    Close();
}

//**************************[BufferGet]****************************************
std::string cComPortBuffer::BufferGet() const {

    return receive_buffer;
}

//**************************[BufferUpdate]*************************************
void cComPortBuffer::BufferUpdate() {

    receive_buffer.append(Receive());
}

//**************************[BufferClear]**************************************
void cComPortBuffer::BufferClear() {

    receive_buffer = "";
}

//**************************[BufferWait]***************************************
bool cComPortBuffer::BufferWait(int count) {

    if (count < 1) {return true;}

    int64_t time_start;
    int64_t time_curr;

    if (receive_buffer.size() >= count) {return true; }

    if (! IsOpened()) { return false; }

    time_start = GetCurrentTime();
    if (time_start < 0) { return false; }

    do {
        SleepOneMilliSecond();

        BufferUpdate();
        if (receive_buffer.size() >= count) { return true; }

        time_curr = GetCurrentTime();
        if (time_curr < 0) { return false; }

     } while (time_curr - time_start <= receive_time);

    return false;

}

  //**************************[BufferWait]*************************************
bool cComPortBuffer::BufferWait(std::string text) {

    if (text == "") { return true; }

    int64_t time_start;
    int64_t time_curr;

    int pos_curr;
    int pos_max;

    pos_max = receive_buffer.size();
    if (pos_max > text.size()) { pos_max = text.size();}

    for (pos_curr = 0; pos_curr < pos_max; pos_curr++) {
        if (text[pos_curr] != receive_buffer[pos_curr]) { return false; }
    }

    if (text.size() <= pos_curr) { return true; }

    if (! IsOpened()) { return false;}

    time_start = GetCurrentTime();
    if (time_start < 0) { return false; }

    do {
        SleepOneMilliSecond();

        BufferUpdate();
        if (receive_buffer.size() > pos_curr) {
            pos_max = receive_buffer.size();
            if (pos_max > text.size()) { pos_max = text.size(); }

            for (; pos_curr < pos_max; pos_curr++) {
                if (text[pos_curr] != receive_buffer[pos_curr]) {
                    return false;
                }
            }
            if (text.size() <= pos_curr) { return true; }
        }

        time_curr = GetCurrentTime();
        if (time_curr < 0) { return false; }

    } while (time_curr - time_start <= receive_time);

    return false;
}

//**************************[BufferTimeSet]************************************
void cComPortBuffer::BufferTimeSet(int milliseconds) {

    if (milliseconds <     1) {milliseconds =     1;}
    if (milliseconds > 10000) {milliseconds = 10000;}

    receive_time = milliseconds;
}

//**************************[Wait]*********************************************
void cComPortBuffer::Wait(int milliseconds) const {

    int64_t time_start;
    int64_t time_elapsed;

    time_start = GetCurrentTime();
    do {
        SleepOneMilliSecond();
        time_elapsed = GetCurrentTime() - time_start;
    } while (time_elapsed <= milliseconds);
}

} // namespace wepet {
