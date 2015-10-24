/******************************************************************************
*                                                                             *
* wepet_avrdude_file.h                                                        *
* ====================                                                        *
*                                                                             *
* Version: 1.0.0                                                              *
* Date   : 17.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/                                          *
******************************************************************************/

#ifndef __WEPET_AVRDUDE_FILE_H
#define __WEPET_AVRDUDE_FILE_H

// local headers

// wepet headers
#include "wepet_basic.h"
#include "wepet_textfile.h"

// standard headers
#include <string>
#include <vector>

// additional headers



namespace wepet {

//*****************************************************************************
//**************************{class cAvrdudeFile}*******************************
//*****************************************************************************
class cAvrdudeFile : protected cTextFile {
  public:
    cAvrdudeFile(const std::string filename);
    ~cAvrdudeFile(void);

    bool SetController(std::string controller);

    std::string GetId(void) const;
    std::string GetDescription(void) const;
    std::string GetSignature(void) const;
    int GetPageSize(void) const;
    int GetPageCount(void) const;

    std::string ReturnLastError(void);
    bool HasError(void) const;

  protected:
    void Reset(void);
    std::string GetLineParam(const int nr);
    std::string GetLineValue(const int nr);

    std::string id_;
    std::string description_;
    std::string signature_;
    int page_size_;
    int page_count_;
};

} // namespace wepet {
#endif // #ifndef __WEPET_AVRDUDE_FILE_H
