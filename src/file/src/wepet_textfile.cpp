/******************************************************************************
*                                                                             *
* wepet_textfile.cpp                                                          *
* ==================                                                          *
*                                                                             *
* Version: 1.0.7                                                              *
* Date   : 30.01.19                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_file/                                 *
******************************************************************************/

// local headers
#include "wepet_textfile.h"

// wepet headers
#include "wepet_basic.h"

// standard headers
#include <fstream>
#include <sstream>

// additional headers



namespace wepet {

//**************************[cTextFile]****************************************
cTextFile::cTextFile() {

}

//**************************[cTextFile]****************************************
cTextFile::cTextFile(std::string filename) {

    LoadFromFile(filename);
}

//**************************[~cTextFile]***************************************
cTextFile::~cTextFile() {

}

//**************************[Clear]********************************************
void cTextFile::Clear() {

    lines_.clear();
}

//**************************[Size]*********************************************
int cTextFile::Size() const {

    return lines_.size();
}

//**************************[GetLine]******************************************
std::string cTextFile::GetLine(int number) {


    if (number <           0) {
        last_error_ = "line_number (" + wepet::IntToStr(number) + ") < 0";
        return "";
    }
    if (number >= lines_.size()) {
        last_error_ = "line_number (" + wepet::IntToStr(number) + ") >= ";
        last_error_+= "line_count (" + wepet::IntToStr(lines_.size()) + ")";
        return "";
    }

    return lines_[number];
}

//**************************[SetLine]******************************************
bool cTextFile::SetLine(int number, std::string line) {

    if (number <           0) {
        last_error_ = "line_number (" + wepet::IntToStr(number) + ") < 0";
        return false;
    }
    if (number >= lines_.size()) {
        AddLine(line);
        return true;
    }

    lines_[number] = line;
    return true;
}

//**************************[AddLine]******************************************
void cTextFile::AddLine(std::string line) {

    lines_.push_back(line);
}

//**************************[SaveToString]*************************************
std::string cTextFile::SaveToString(void) const {

    std::stringstream ss;

    for (int i = 0; i < lines_.size(); i++) {
        ss << lines_[i] << '\n';
    }

    return ss.str();
}


//**************************[LoadFromString]***********************************
bool cTextFile::LoadFromString(const std::string &str) {

    lines_.clear();

    int start = 0;
    int pos = 0;

    while (pos < str.size()) {
        char c = str[pos];
        if ((c == '\r') || (c == '\n')) {
            AddLine(str.substr(start,pos - start));

            pos++;
            if (pos >= str.size()) { break; }

            if (((c == '\r') && (str[pos] == '\n')) ||
              ((c == '\n') && (str[pos] == '\r'))) {
                pos++;
            }
            start = pos;
        } else {
            pos++;
        }
    }
    if (pos > start) {
        AddLine(str.substr(start,pos - start));
    }

    return true;
}

//**************************[SaveToFile]***************************************
bool cTextFile::SaveToFile(const std::string filename) {

    return SaveString(filename, SaveToString());
}


//**************************[LoadFromFile]*************************************
bool cTextFile::LoadFromFile(const std::string filename) {

    lines_.clear();

    std::string temp_data;
    if (! LoadString(filename, temp_data)) { return false; }

    return LoadFromString(temp_data);
}

//**************************[ReturnLastError]**********************************
std::string cTextFile::ReturnLastError(void) {

    std::string result = last_error_;
    last_error_ = "";
    return result;
}

//**************************[HasError]*****************************************
bool cTextFile::HasError(void) const {

    return last_error_ != "";
}

//**************************[SaveString]***************************************
bool cTextFile::SaveString(const std::string &filename,
  const std::string &data) {

    std::ofstream fout(filename.data());
    if (!fout) {
        last_error_ = "can not save to file \"" + filename + "\"";
        return false;
    }

    fout << data;
    fout.close();

    return true;
}

//**************************[LoadString]***************************************
bool cTextFile::LoadString(const std::string &filename, std::string &data) {
    std::stringstream ss;

    std::ifstream fin(filename.data());
    if (!fin) {
        last_error_ = "can not load from file \"" + filename + "\"";
        return false;
    }

    ss << fin.rdbuf();
    fin.close();

    data = ss.str();

    return true;
}

} //namespace wepet {
