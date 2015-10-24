/******************************************************************************
*                                                                             *
* wepet_flash.cpp                                                             *
* ===============                                                             *
*                                                                             *
* Version: 1.0.5                                                              *
* Date   : 19.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/                                          *
******************************************************************************/

// local headers
#include "wepet_flash.h"

// wepet headers
#include "wepet_basic.h"
#include "wepet_hexfile.h"

// standard headers
#include <fstream>
#include <sstream>

// additional headers



namespace wepet {

//*****************************************************************************
//**************************[sFlash...]****************************************
//*****************************************************************************

sFlashWord::sFlashWord(): lo(0xFF), hi(0xFF), state(fsNone) {
}
const sFlashWord& sFlashWord::operator = (const sFlashWord &other) {

    lo    = other.lo   ;
    hi    = other.hi   ;
    state = other.state;

    return *this;
}


sFlashPage::sFlashPage() :offset_words(0), state(fsNone) {
}
const sFlashPage& sFlashPage::operator = (const sFlashPage &other) {

    words        = other.words       ;
    offset_words = other.offset_words;
    state        = other.state       ;

    return *this;
}

//*****************************************************************************
//**************************[cFlash]*******************************************
//*****************************************************************************

//**************************[cFlash]*******************************************
cFlash::cFlash(const int page_size, const int page_limit) {

    Clear(page_size, page_limit);
}

//**************************[~cFlash]******************************************
cFlash::~cFlash() {
}

//**************************[Clear]********************************************
bool cFlash::Clear(const int page_size, const int page_limit) {

    flash_.pages.clear();
    flash_.offset_pages = 0;

    flash_.state = fsNone;

    if (page_size <= 0) {
        last_error_ = "page_size(" + wepet::IntToStr(page_size) + ") <= 0 ";
        flash_.limit_pages = 0;
        flash_.limit_words = 0;
        return false;
    }
    flash_.limit_words = page_size / 2;

    if (page_limit <= 0) {
      flash_.limit_pages = -1;
    } else {
      flash_.limit_pages = page_limit;
    }

    return true;
}

//**************************[Reset]********************************************
void cFlash::Reset(void) {

    flash_.state = (eFlashState) (flash_.state & fsGetKnown);

    for (int i_page = flash_.pages.size()-1; i_page >= 0; i_page--) {
        sFlashPage &page = flash_.pages[i_page];
        page.state = (eFlashState) (page.state & fsGetKnown);

        for (int i_word = page.words.size()-1; i_word >= 0; i_word--) {
            sFlashWord &word = page.words[i_word];
            word.state = (eFlashState) (word.state & fsGetKnown);
        }
    }
}

//**************************[DataSet]******************************************
bool cFlash::DataSet(const int address, const uint8_t data) {

    int page, word;
    bool high;
    if (! ConvertAddress(address, page, word, high)) {
        return false;
    }

    return DataSet(page, word, high, data);
}

//**************************[DataLoSet]****************************************
bool cFlash::DataLoSet(const int page, const int word, const uint8_t data) {

    return DataSet(page, word, false, data);
}

//**************************[DataHiSet]****************************************
bool cFlash::DataHiSet(const int page, const int word, const uint8_t data) {

    return DataSet(page, word, true, data);
}

//**************************[DataGet]******************************************
uint8_t cFlash::DataGet(const int address) {

    int page, word;
    bool high;
    if (! ConvertAddress(address, page, word, high)) {
        return 0xFF;
    }

    return DataGet(page, word, high);
}

//**************************[DataLoGet]****************************************
uint8_t cFlash::DataLoGet(const int page, const int word) {

    return DataGet(page, word, false);
}

//**************************[DataHiGet]****************************************
uint8_t cFlash::DataHiGet(const int page, const int word) {

    return DataGet(page, word, true);
}

//**************************[FlashStateGet]************************************
const eFlashState cFlash::FlashStateGet () {

    return flash_.state;
}

//**************************[PageStateGet]*************************************
const eFlashState cFlash::PageStateGet (const int page) {

    if (! CheckPage(page)) {
        return fsNone;
    }

    if ((page < flash_.offset_pages) ||
      (page >= flash_.offset_pages + flash_.pages.size())) {
        return fsNone;
    }
    sFlashPage &ref_page = flash_.pages[page - flash_.offset_pages];

    return ref_page.state;
}

//**************************[WordStateGet]*************************************
const eFlashState cFlash::WordStateGet (const int page, const int word) {

    if (! CheckWord(page, word)) {
        return fsNone;
    }

    if ((page < flash_.offset_pages) ||
      (page >= flash_.offset_pages + flash_.pages.size())) {
        return fsNone;
    }
    sFlashPage &ref_page = flash_.pages[page - flash_.offset_pages];

    if ((word < ref_page.offset_words) ||
      (word >= ref_page.offset_words + ref_page.words.size())) {
        return fsNone;
    }
    sFlashWord &ref_word = ref_page.words[word - ref_page.offset_words];

    return ref_word.state;
}

//**************************[ToString]*****************************************
std::string cFlash::ToString(int line_size, const eFlashState mask) {

    if (line_size < 4) {
        line_size = 4;
    }

    std::stringstream ss;
    bool output = false;
    int line_count = 0;

    for (int page = 0; page < flash_.pages.size(); page++) {

        sFlashPage &ref_page = flash_.pages[page];
        if ((ref_page.state & mask) == 0) {
            continue;
        }
        line_count = 0;

        for (int word = 0; word < ref_page.words.size(); word++) {

            sFlashWord &ref_word = ref_page.words[word];
            if ((ref_word.state & mask) == 0) {
                line_count = 0;
                continue;
            }

            if (line_count > 0) {
                line_count--;
            } else {
                if (output == true) {
                    ss << std::endl;
                }
                ss << wepet::IntToHex(page + flash_.offset_pages, 4) << " ";
                ss << wepet::IntToHex(word + ref_page.offset_words, 2) << ": ";

                line_count = line_size - 1;
            }
            ss << " " << wepet::IntToHex(ref_word.lo, 2);
            ss << " " << wepet::IntToHex(ref_word.hi, 2);
            output = true;
        }
        line_count = true;
    }
    if (output) {
        ss << std::endl;
    } else {
        ss << "<empty>" << std::endl;
    }

    return ss.str();
}

//**************************[SaveToHexFile]************************************
bool cFlash::SaveToHexFile(const std::string filename,
  const eFlashState mask) {

    cHexfileWriter writer;

    for (int page = 0; page < flash_.pages.size(); page++) {

        sFlashPage &ref_page = flash_.pages[page];
        if ((ref_page.state & mask) == 0) { continue; }

        for (int word = 0; word < ref_page.words.size(); word++) {

            sFlashWord &ref_word = ref_page.words[word];
            if ((ref_word.state & mask) == 0) { continue; }

            int address = ConvertAddress(page + flash_.offset_pages,
              word + ref_page.offset_words);
            if (address < 0) {
                last_error_ = writer.ReturnLastError();
                return false;
            }

            if (! writer.SetData(address  , ref_word.lo)) {
                last_error_ = writer.ReturnLastError();
                return false;
            }
            if (! writer.SetData(address+1, ref_word.hi)) {
                last_error_ = writer.ReturnLastError();
                return false;
            }
        }
    }
    if (! writer.SaveToFile(filename)) {
        last_error_ = writer.ReturnLastError();
        return false;
    }

    return true;
}

//**************************[LoadFromHexFile]**********************************
bool cFlash::LoadFromHexFile(const std::string filename) {

    Clear();

    cHexfileReader reader;
    if (! reader.LoadFromFile(filename)) {
        last_error_ = reader.ReturnLastError();
        return false;
    }

    int address_old = 0;
    while (! reader.IsEndOfFile()) {
        int address;
        uint8_t data;
        if (reader.GetData(address, data)) {
            if ((address_old >= address) ||
              ((address_old | 0x0001) != (address | 0x0001))) {
                int page, word;
                bool high;
                if (! ConvertAddress(address, page, word, high)) {
                    Clear();
                    return false;
                }

                if (WordStateGet(page, word) & fsGetUsed) {
                    Clear();
                    last_error_ = "data word already set";
                    return false;
                }
            }
            DataSet(address, data);
            address_old = address;
        }

        ++reader;

        if (reader.HasError()) {
            Clear();
            last_error_ = reader.ReturnLastError();
            return false;
        }

    }

    return true;
}

//**************************[PageSizeGet]**************************************
int cFlash::FlashSizeGet() const {

    if (PageLimitGet() > 0) {
        return PageLimitGet() * PageSizeGet();
    } else {
        return (flash_.offset_pages + flash_.pages.size()) * PageSizeGet();
    }
}

//**************************[PageSizeGet]**************************************
int cFlash::PageSizeGet() const {

    return flash_.limit_words * 2;
}

//**************************[PageLimitGet]*************************************
int cFlash::PageLimitGet() const {

    return flash_.limit_pages;
}

//**************************[PageMinGet]***************************************
int cFlash::PageMinGet() const {

    return flash_.offset_pages;
}

//**************************[PageMaxGet]***************************************
int cFlash::PageMaxGet() const {

    return flash_.offset_pages + flash_.pages.size() - 1;
}

//**************************[ReturnLastError]**********************************
std::string cFlash::ReturnLastError(void) {

    std::string result = last_error_;
    last_error_ = "";
    return result;
}

//**************************[HasError]*****************************************
bool cFlash::HasError(void) const {

    return last_error_ != "";
}


//**************************[CheckAddress]*************************************
bool cFlash::CheckAddress(const int address) {

    if (address < 0) {
        last_error_ = "address (" + wepet::IntToStr(address) + ") < 0 ";
        return false;
    }

    int page_size = 2 * flash_.limit_words;
    if (page_size <= 0) {
        last_error_ = "page_size(" + wepet::IntToStr(page_size) + ") <= 0 ";
        return false;
    }

    if (flash_.limit_pages >= 0) {
        if (address >= flash_.limit_pages * page_size) {
            last_error_ = "address (" + wepet::IntToStr(address) + ") >= " +
              wepet::IntToStr(flash_.limit_pages) + "x" +
              wepet::IntToStr(page_size) + " (" +
              wepet::IntToStr(flash_.limit_pages * page_size) + ")";
            return false;
        }
    }

    return true;
}

//**************************[CheckPage]****************************************
bool cFlash::CheckPage(const int page) {

    if (page < 0) {
        last_error_ = "page (" + wepet::IntToStr(page) + ") < 0 ";
        return false;
    }
    if ((flash_.limit_pages >= 0) && (page >= flash_.limit_pages)) {
        last_error_ = "page (" + wepet::IntToStr(page) + ") >= " +
          "page_limit (" + wepet::IntToStr(flash_.limit_pages) + ")";
        return false;
    }

    return true;
}

//**************************[CheckWord]****************************************
bool cFlash::CheckWord(const int page, const int word) {

    if (! CheckPage(page)) {
        return false;
    }

    if (word < 0) {
        last_error_ = "word (" + wepet::IntToStr(word) + ") < 0 ";
        return false;
    }
    if (word >= flash_.limit_words) {
        last_error_ = "word (" + wepet::IntToStr(word) + ") >= " +
          "page_size/2 (" + wepet::IntToStr(flash_.limit_words) + ")";
        return false;
    }

    return true;
}

//**************************[ConvertAddress]***********************************
bool cFlash::ConvertAddress(const int address, int &page, int &word,
  bool &high) {

    if (! CheckAddress(address)) {
        return false;
    }

    page = (address / 2) / flash_.limit_words;
    word = (address / 2) % flash_.limit_words;
    high = (address % 2) != 0;

    return true;
}

//**************************[ConvertAddress]***********************************
int cFlash::ConvertAddress(const int page, const int word, const bool high) {

    if (! CheckWord(page, word)) { return -1;}

    int result;
    result = (page * flash_.limit_words + word) * 2;
    if (high) {
        result++;
    }
    return result;
}

//**************************[DataSet]******************************************
bool cFlash::DataSet(const int page, const int word, const bool high,
  const uint8_t data) {

    if (! CheckWord(page, word)) {
        return false;
    }

    if (flash_.pages.size() < 1) {
        flash_.pages.push_back(sFlashPage());
        flash_.offset_pages = page;
    } else if (page < flash_.offset_pages) {
        int diff  = flash_.offset_pages - page;
        int count = flash_.pages.size();

        flash_.pages.resize(count + diff);
        for (int i = count-1; i >= 0; i--) {
            flash_.pages[i + diff] = flash_.pages[i];
        }
        for (int i = diff-1; i >= 0; i--) {
            flash_.pages[i] = sFlashPage();
        }

        flash_.offset_pages = page;
    } else if (page >= flash_.offset_pages + flash_.pages.size()) {
        flash_.pages.resize(page - flash_.offset_pages + 1);
    }
    sFlashPage &ref_page = flash_.pages[page - flash_.offset_pages];


    if (ref_page.words.size() < 1) {
        ref_page.words.push_back(sFlashWord());
        ref_page.offset_words = word;
    } else if (word < ref_page.offset_words) {
        int diff  = ref_page.offset_words - word;
        int count = ref_page.words.size();

        ref_page.words.resize(count + diff);
        for (int i = count-1; i >= 0; i--) {
            ref_page.words[i + diff] = ref_page.words[i];
        }
        for (int i = diff-1; i >= 0; i--) {
            ref_page.words[i] = sFlashWord();
        }

        ref_page.offset_words = word;
    } else if (word >= ref_page.offset_words + ref_page.words.size()) {
        ref_page.words.resize(word - ref_page.offset_words + 1);
    }
    sFlashWord &ref_word = ref_page.words[word - ref_page.offset_words];


    bool known = ref_word.state & fsGetKnown;
    ref_word.state = (eFlashState) (ref_word.state | fsSetUsed);
    ref_page.state = (eFlashState) (ref_page.state | fsSetUsed);
    flash_.state = (eFlashState)   (flash_.state   | fsSetUsed);

    if (high) {
        if (known && (ref_word.hi == data)) {
            return true;
        }
        ref_word.hi = data;
    } else {
        if (known && (ref_word.lo == data)) {
            return true;
        }
        ref_word.lo = data;
    }
    ref_word.state = (eFlashState) (ref_word.state | fsSetChanged);
    ref_page.state = (eFlashState) (ref_page.state | fsSetChanged);
    flash_.state   = (eFlashState) (flash_.state   | fsSetChanged);

    return true;
}

//**************************[DataGet]******************************************
uint8_t cFlash::DataGet(const int page, const int word, const bool high) {

    if (! CheckWord(page, word)) {
        return 0xFF;
    }

    if ((page < flash_.offset_pages) ||
      (page >= flash_.offset_pages + flash_.pages.size())) {
        return 0xFF;
    }
    sFlashPage &ref_page = flash_.pages[page - flash_.offset_pages];


    if ((word < ref_page.offset_words) ||
      (word >= ref_page.offset_words + ref_page.words.size())) {
        return 0xFF;
    }
    sFlashWord &ref_word = ref_page.words[word - ref_page.offset_words];

    if (high) {
        return ref_word.hi;
    } else {
        return ref_word.lo;
    }
}

} //namespace wepet {
