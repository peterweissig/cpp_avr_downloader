/******************************************************************************
*                                                                             *
* wepet_avrdude_file.cpp                                                      *
* ======================                                                      *
*                                                                             *
* Version: 1.0.0                                                              *
* Date   : 17.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_avr_downloader                        *
******************************************************************************/

// local headers
#include "wepet_avrdude_file.h"

// wepet headers

// standard headers
#include <fstream>
#include <sstream>

// additional headers



namespace wepet {

//**************************[cAvrdudeFile]*************************************
cAvrdudeFile::cAvrdudeFile(const std::string filename) :
  cTextFile(filename) {

    Reset();
}

//**************************[~cAvrdudeFile]************************************
cAvrdudeFile::~cAvrdudeFile() {

}

//**************************[SetController]************************************
bool cAvrdudeFile::SetController(const std::string controller) {

    Reset();
    std::string mcu = wepet::LowerCase(controller);

    bool found = false;

    std::string id;
    std::string description;
    std::string signature;
    bool flash_memory;
    int page_count;
    int page_size;

    for (int i = 0; i < lines_.size(); i++) {
        if (GetLine(i) == "part") {
            if (found) { break; }

            id          = "";
            description = "";
            signature   = "";
            flash_memory = false;
            page_count  = -1;
            page_size   = -1;
        }


        std::string param = GetLineParam(i);

        if (param    == "" ) { continue; }
        if (param[0] == '#') { continue; }

        if (param == "id") {
            std::string temp = GetLineValue(i);
            id = temp.substr(1, temp.size()-2);
            if (wepet::LowerCase(id) == mcu) { found = true; }

        } else if (param == "desc") {
            std::string temp = GetLineValue(i);
            description = temp.substr(1, temp.size()-2);
            if (wepet::LowerCase(description) == mcu) { found = true; }

        } else if (param == "signature") {
            std::string temp = GetLineValue(i);
            int start = 0;
            while (start < temp.size()) {
                int end = start;
                do {
                    end++;
                    if (end >= temp.size()) { break; }
                } while ((temp[end] != ' ') && (temp[end] != 9));
                int value;
                if (! wepet::StrToInt(temp.substr(start, end - start),
                  value)) {
                    signature = "";
                    break;
                }
                signature+= (char) value;

                do {
                    end++;
                    if (end >= temp.size()) { break; }
                } while ((temp[end] == ' ') || (temp[end] == 9));
                start = end;
            }

            if (signature == mcu) { found = true; }

        } else if (param == "memory \"flash\"") {
            flash_memory = true;
            continue;

        } else if (param == ";") {
            flash_memory = false;
            continue;

        } else if (flash_memory) {
            if (param == "page_size") {
                wepet::StrToInt(GetLineValue(i) , page_size);

            } else if (param == "num_pages") {
                wepet::StrToInt(GetLineValue(i) , page_count);

            } else {
                continue;
            }

        } else {
            continue;
        }

        if (found &&
          (id != "") && (description != "") && (signature != "") &&
          (page_count > 0) && (page_size > 0)) {
            id_          = id;
            description_ = description;
            signature_   = signature;
            page_count_  = page_count;
            page_size_   = page_size;

            return true;
        }
    }

    if (found) {
        last_error_ = "controller \"" + mcu + "\" not fully described";
    } else {
        last_error_ = "controller \"" + mcu + "\" not found";
    }

    return false;
}

//**************************[GetId]********************************************
std::string cAvrdudeFile::GetId() const {

    return id_;
}

//**************************[GetDescription]***********************************
std::string cAvrdudeFile::GetDescription() const {

    return description_;
}

//**************************[GetSignature]*************************************
std::string cAvrdudeFile::GetSignature() const {

    return signature_;
}

//**************************[GetPageSize]**************************************
int cAvrdudeFile::GetPageSize() const {

    return page_size_;
}

//**************************[GetPageCount]*************************************
int cAvrdudeFile::GetPageCount() const {

    return page_count_;
}

//**************************[ReturnLastError]**********************************
std::string cAvrdudeFile::ReturnLastError() {

    return cTextFile::ReturnLastError();
}

//**************************[HasError]*****************************************
bool cAvrdudeFile::HasError() const {

    return cTextFile::HasError();
}

//**************************[Reset]********************************************
void cAvrdudeFile::Reset() {

    id_          = "";
    description_ = "";
    signature_   = "";

    page_count_  = -1;
    page_size_   = -1;
}

//**************************[GetLineParam]*************************************
std::string cAvrdudeFile::GetLineParam(const int nr) {

    std::string line = GetLine(nr);

    int start = 0;
    while (start < line.size()) {
        char c = line[start];
        if ((c != ' ') && (c != 9)) break;
        start++;
    }

    int end = start;
    while (end < line.size()) {
        if (line[end] == '=') break;
        end++;
    }

    end--;
    while (end >= start) {
        char c = line[end];
        if ((c != ' ') && (c != 9)) break;
        end--;
    }

    return line.substr(start, end-start + 1);
}

//**************************[GetLineValue]*************************************
std::string cAvrdudeFile::GetLineValue(const int nr) {

    std::string line = GetLine(nr);

    int start = 0;
    while (start < line.size()) {
        char c = line[start];
        if (c == '=') break;
        start++;
    }
    start++;
    while (start < line.size()) {
        char c = line[start];
        if ((c != ' ') && (c != 9)) break;
        start++;
    }

    int end = line.size() - 1;
    while (end >= start) {
        char c = line[end];
        if ((c != ' ') && (c != 9) && (c != ';')) break;
        end--;
    }

    if (end >= start) {
        return line.substr(start, end-start + 1);
    } else  {
        return "";
    }
}

} //namespace wepet {
