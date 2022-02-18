/******************************************************************************
*                                                                             *
* wepet_flash.h                                                               *
* =============                                                               *
*                                                                             *
* Version: 1.0.5                                                              *
* Date   : 19.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/                                          *
******************************************************************************/

#ifndef __WEPET_FLASH_H
#define __WEPET_FLASH_H

// local headers

// wepet headers

// standard headers
#include <string>
#include <vector>

#include <stdint.h>

// additional headers



namespace wepet {

//**************************<Types>********************************************
enum eFlashState {
    fsNone       = 0x00,
    fsGetKnown   = 0x01,
    fsSetKnown   = fsGetKnown,
    fsGetUsed    = 0x02,
    fsSetUsed    = fsGetKnown | fsGetUsed,
    fsGetChanged = 0x04,
    fsSetChanged = fsGetKnown | fsGetUsed | fsGetChanged
};

struct sFlashWord {
    sFlashWord(void);
    const sFlashWord& operator = (const sFlashWord &other);

    uint8_t lo;
    uint8_t hi;
    eFlashState state;
};

struct sFlashPage {
    sFlashPage(void);
    const sFlashPage& operator = (const sFlashPage &other);

    std::vector <sFlashWord> words;
    int offset_words;
    eFlashState state;
};

struct sFlash {
    std::vector <sFlashPage> pages;
    int offset_pages;
    eFlashState state;

    int limit_pages;
    int limit_words;
};

//*****************************************************************************
//**************************{class cFlash}*************************************
//*****************************************************************************
class cFlash {
  public:
    cFlash(const int page_size = 256, const int page_limit = -1);
    ~cFlash(void);

    bool Clear(const int page_size = 256, const int page_limit = -1);
    void Reset(void);

    bool DataSet(const int address, const uint8_t data);
    bool DataLoSet(const int page, const int word, const uint8_t data);
    bool DataHiSet(const int page, const int word, const uint8_t data);

    uint8_t DataGet(const int address);
    uint8_t DataLoGet(const int page, const int word);
    uint8_t DataHiGet(const int page, const int word);

    const eFlashState FlashStateGet (void);
    const eFlashState PageStateGet (const int page);
    const eFlashState WordStateGet (const int page, const int word);


    std::string ToString(int line_size = 8,
      const eFlashState mask = fsGetUsed);
    bool SaveToHexFile(const std::string filename,
      const eFlashState mask = fsGetUsed);
    bool LoadFromHexFile(const std::string filename);

    int FlashSizeGet(void) const;
    int PageSizeGet(void) const;
    int PageLimitGet(void) const;
    int PageMinGet(void) const;
    int PageMaxGet(void) const;

    std::string ReturnLastError(void);
    bool HasError(void) const;

  private:
    bool CheckAddress(const int address);
    bool CheckPage(const int page);
    bool CheckWord(const int page, const int word);
    bool ConvertAddress(const int address, int &page, int &word, bool &high);
    int ConvertAddress(const int page, const int word,
      const bool high = false);
    bool DataSet(const int page, const int word, const bool high,
      const uint8_t data);
    uint8_t DataGet(const int page, const int word, const bool high);

    sFlash flash_;
    std::string last_error_;
};

} // namespace wepet {
#endif // #ifndef __WEPET_FLASH_H
