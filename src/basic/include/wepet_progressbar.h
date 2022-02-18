/******************************************************************************
*                                                                             *
* wepet_progressbar.h                                                         *
* ===================                                                         *
*                                                                             *
* Version: 1.0.1                                                              *
* Date   : 23.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_basic/                                *
******************************************************************************/

#ifndef __WEPET_PROGRESSBAR_H
#define __WEPET_PROGRESSBAR_H

// local headers

// wepet headers

// standard headers
#include <string>

// additional headers



namespace wepet {

//*****************************************************************************
//**************************{class cProgressbar}*******************************
//*****************************************************************************
class cProgressbar {
  public:
    cProgressbar(void);
    ~cProgressbar(void);

    bool Init(const std::string caption, const int max = 100,
      const int min = 0, const int size = 50);
    bool Update(const int new_position);

    void Reset(void);
    void Reset(const std::string new_caption);
    void Skip(void);

  protected:
    void Clear(void);

    std::string caption_;

    int pos_;
    int max_;
    int offset_;

    int size_;
};

} // namespace wepet {
#endif // #ifndef __WEPET_PROGRESSBAR_H
