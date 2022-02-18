/******************************************************************************
*                                                                             *
* wepet_progressbar.cpp                                                       *
* =====================                                                       *
*                                                                             *
* Version: 1.0.1                                                              *
* Date   : 23.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_basic/                                *
******************************************************************************/

// local headers
#include "wepet_progressbar.h"

// wepet headers

// standard headers
#include <iostream>

// additional headers



namespace wepet {

//*****************************************************************************
//**************************{class cProgressbar}*******************************
//*****************************************************************************


//**************************[cProgressbar]*************************************
cProgressbar::cProgressbar() {

    Clear();
}

//**************************[~cProgressbar]************************************
cProgressbar::~cProgressbar() {

}

//**************************[Init]*********************************************
bool cProgressbar::Init(const std::string caption, const int max,
  const int min, const int size) {

    if ((min >= max) || (size < 2)) {
        Clear();
        return false;
    }
    caption_ = caption;

    pos_     = -1;
    max_     = max - min;
    offset_  = min;

    size_    = size - 1;

    return true;
}

//**************************[update]*******************************************
bool cProgressbar::Update(const int new_position) {

    if ((max_ <= 0) || (size_ < 2)) {
        return false;
    }

    if (pos_ < 0) {
        std::cout << caption_ << '#';
        pos_ = 0;
    }

    int new_pos = new_position - offset_;
    if (new_pos > max_) {
        new_pos = max_;
    }

    if (new_pos > pos_) {
        int count_pos = (size_ * pos_    ) / max_;
        int count_new = (size_ * new_pos ) / max_;

        while (count_pos < count_new) {
            std::cout << "#";
            count_pos++;
        }

        pos_ = new_pos;
        if (pos_ == max_) {
            std::cout << std::endl;
        }
    }

    std::cout.flush();

    return true;
}

//**************************[Reset]********************************************
void cProgressbar::Reset() {

    pos_ = -1;
}

//**************************[Reset]********************************************
void cProgressbar::Reset(const std::string new_caption) {

    Reset();
    caption_ = new_caption;
}

//**************************[Skip]*********************************************
void cProgressbar::Skip() {

    if ((pos_ >= 0) && (pos_ < max_)) {
        std::cout << std::endl;
    }
    Reset();
}

//**************************[Clear]********************************************
void cProgressbar::Clear() {

    caption_ = "";

    pos_     = -1;
    max_     = -1;
    offset_  = -1;

    size_    = -1;
}

} // namespace wepet {
