/******************************************************************************
*                                                                             *
* wepet_hexfile.cpp                                                           *
* =================                                                           *
*                                                                             *
* Version: 1.0.4                                                              *
* Date   : 19.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_file/                                 *
******************************************************************************/

// local headers
#include "wepet_hexfile.h"

// wepet headers
#include "wepet_basic.h"

// standard headers
#include <sstream>
#include <fstream>

// additional headers



namespace wepet {

//*****************************************************************************
//**************************{class cHexfileReader}*****************************
//*****************************************************************************

//**************************[cHexfileReader]***********************************
cHexfileReader::cHexfileReader() {

    offset_address_   =  0;
    current_address_  = -1;
    current_line_pos_ = -1;
    current_data_pos_ = -1;
    end_of_file_ = false;
}

//**************************[cHexfileReader]***********************************
cHexfileReader::cHexfileReader(const std::string filename) {

    LoadFromFile(filename);
}

//**************************[~cHexfileReader]**********************************
cHexfileReader::~cHexfileReader() {
}

//**************************[Reset]********************************************
void cHexfileReader::Reset() {

    offset_address_   =  0;
    current_address_  = -1;
    current_line_pos_ = -1;
    current_data_pos_ = -1;
    end_of_file_ = false;

    current_data_.clear();
    ++(*this);
}

//**************************[operator ++ ]*************************************
bool cHexfileReader::operator ++() {

    if (IsEndOfFile()) { return false;}
    if (HasError()   ) { return false;}

    if (current_line_pos_ < 0) {
        current_line_pos_ = -1;
    } else if (current_data_pos_ < current_data_.size() - 1) {
        current_data_pos_++;
        return true;
    }

    while (1) {
        current_line_pos_++;
        if (current_line_pos_ >= lines_.size()) {
            last_error_ = "unexpected end of file";
            end_of_file_ = true;
            return false;
        }

        if (! InterpreteCurrentLine()) {
            return false;
        }

        if (end_of_file_) {
            return false;
        }

        if (current_data_.size() > 0) {
            return true;
        }
    }
    return false;
}

//**************************[GetData]******************************************
bool cHexfileReader::GetData(int &address, uint8_t &data) {

    if (IsEndOfFile()) { return false;}

    if ((current_line_pos_ < 0) || (current_line_pos_ >= lines_.size())) {
        return false;
    }

    if (current_address_ < 0) {
        return false;
    }

    if ((current_data_pos_ < 0) ||
      (current_data_pos_ >= current_data_.size())) {
        return false;
    }

    address = ((current_address_ + current_data_pos_) & 0xFFFF) +
      offset_address_;
    data    = current_data_[current_data_pos_];

    return true;
}

//**************************[IsEndOfFile]**************************************
bool cHexfileReader::IsEndOfFile() const {

    return end_of_file_;
}

//**************************[LoadFromFile]*************************************
bool cHexfileReader::LoadFromFile(const std::string filename) {

    bool result = cTextFile::LoadFromFile(filename);
    Reset();
    return result;
}

//**************************[LoadFromString]***********************************
bool cHexfileReader::LoadFromString(const std::string str) {

    bool result = cTextFile::LoadFromString(str);
    Reset();
    return result;
}

//**************************[ReturnLastError]**********************************
std::string cHexfileReader::ReturnLastError() {

    return cTextFile::ReturnLastError();
}

//**************************[HasError]*****************************************
bool cHexfileReader::HasError() const {

    return cTextFile::HasError();
}

//**************************[InterpreteCurrentLine]****************************
bool cHexfileReader::InterpreteCurrentLine() {

    current_data_.clear();
    current_data_pos_ = 0;

    if (end_of_file_) {
        last_error_ = "internal error (" + WEPET_ERROR_STRING + ")";
        return false;
    }
    if ((current_line_pos_ < 0) || (current_line_pos_ >= lines_.size())) {
        last_error_ = "internal error (" + WEPET_ERROR_STRING + ")";
        return false;
    }

    std::string line = lines_[current_line_pos_];

    if (line.size() < 11) {
        last_error_ = "line " + wepet::IntToStr(current_line_pos_) +
          " has less (" + wepet::IntToStr(line.size()) +
          ") than minimum number of characters (11)";
        return false;
    }

    if ((line.size() % 2) == 0) {
        last_error_ = "line " + wepet::IntToStr(current_line_pos_) +
          " has even number of characters (" +
          wepet::IntToStr(line.size()) + ")";
        return false;
    }

    if (line[0] != ':')   {
        last_error_ = "line " + wepet::IntToStr(current_line_pos_) +
          " does not start with character ':' (" + line[0] + ")";
        return false;
    }

    std::vector<uint8_t> data;
    data.resize((line.size() - 1) / 2, 0x00);
    uint8_t crc = 0;
    for (int i = 0; i < data.size(); i++) {
        int temp = wepet::HexToInt(line.substr(2*i + 1,2));
        if ((temp < 0) || (temp > 0xFF)) {
            last_error_ = "line " + wepet::IntToStr(current_line_pos_) +
            " - byte " + wepet::IntToStr(i + 1) + " is not valid (" +
            line.substr(2*i + 1,2) + ")";
            return false;
        }
        data[i] = temp;
        crc+=temp;
    }

    if (crc != 0x00) {
        last_error_ = "line " + wepet::IntToStr(current_line_pos_) +
        " has invalid checksum (" + wepet::IntToStr(crc) + "!=0)";
        return false;
    }

    if (data[0] + 5 != data.size()) {
        last_error_ = "line " + wepet::IntToStr(current_line_pos_) +
        " - given size (" + wepet::IntToStr(data[0]) + "+5)"
        " differs from real size (" + wepet::IntToStr(data.size()) + ")";
        return false;
    }

    current_address_ = ((int)data[1]) << 8 | (int)data[2];

    switch (data[3]) {
        case 0x00: // data
            current_data_.resize(data.size() - 5);
            for (int i = 0; i < current_data_.size(); i++) {
                current_data_[i] = data[i + 4];
            }
            break;

        case 0x01: // end of file
            end_of_file_ = true;
            break;

        case 0x02: //address offset
            offset_address_ = 0;
            for (int i = 4; i < data.size() - 1; i++) {
                offset_address_ = offset_address_ << 8;
                offset_address_|= data[i];
            }
            offset_address_ = offset_address_ << 4;
            break;

        case 0x03: // bootstart
            break;

        default:
            last_error_ = "line " + wepet::IntToStr(current_line_pos_) +
            " - unknown command (" + wepet::IntToStr(data[3]) + ")";
            return false;
            break;
    }
    return true;
}

//*****************************************************************************
//**************************{class cHexfileWriter}*****************************
//*****************************************************************************

//**************************[cHexfileWriter]***********************************
cHexfileWriter::cHexfileWriter(const int max_bytes_per_line) {

    max_bytes_per_line_ = max_bytes_per_line;
    Clear();
}

//**************************[~cHexfileWriter]**********************************
cHexfileWriter::~cHexfileWriter() {
}

//**************************[Clear]********************************************
void cHexfileWriter::Clear() {

    offset_address_   = 0;
    current_address_  = 0;

    lines_.clear();
    current_data_.clear();
}

//**************************[SetData]******************************************
bool cHexfileWriter::SetData(const int address, const uint8_t data) {

    if (address < 0) {
        last_error_ = "negative address (" + wepet::IntToStr(address) + ")";
        return false;
    }

    bool new_line = false;
    bool new_offset = false;

    int temp_datasize = current_data_.size();

    if (temp_datasize >= max_bytes_per_line_) {
        new_line = true;
    }

    int temp_address = address & 0xFFFF;
    if (temp_datasize > 0) {
        if (temp_address != (current_address_ + temp_datasize) & 0xFFFF) {
            new_line = true;
        }
    } else {
       current_address_ = temp_address;
    }

    int temp_offset = address - temp_address;
    if (temp_offset != offset_address_) {
        new_offset = true;
        if (temp_datasize > 0) { new_line = true; }
    }

    if (new_line) {
        lines_.push_back(ReturnCurrentLine());
        current_data_.clear();

        current_address_ = temp_address;
    }
    if (new_offset) {
        offset_address_ = temp_offset;
        StoreOffset();
    }

    current_data_.push_back(data);

    return true;
}

//**************************[SaveToFile]***************************************
bool cHexfileWriter::SaveToFile(const std::string filename) {

    std::string temp_data = SaveToString();
    if (temp_data == "") { return false; }

    return cTextFile::SaveString(filename, temp_data);
}

//**************************[SaveToString]*************************************
std::string cHexfileWriter::SaveToString() {

    std::string result;
    result = cTextFile::SaveToString();
    if (current_data_.size() > 0) {
        result+= ReturnCurrentLine();
        result+= '\n';
    }

    result+= ReturnEndOfFile();
    result+= '\n';

    return result;
}

//**************************[ReturnLastError]**********************************
std::string cHexfileWriter::ReturnLastError() {

    return cTextFile::ReturnLastError();
}

//**************************[HasError]*****************************************
bool cHexfileWriter::HasError() const {

    return cTextFile::HasError();
}

//**************************[ReturnEndOfFile]**********************************
std::string cHexfileWriter::ReturnEndOfFile() {

    return MakeLine(0x0000, 0x01, std::vector<uint8_t>(1,0x00));
}

//**************************[ReturnCurrentLine]********************************
std::string cHexfileWriter::ReturnCurrentLine() {

    return MakeLine(current_address_, 0x00, current_data_);
}

//**************************[StoreOffset]**************************************
void cHexfileWriter::StoreOffset() {

    std::vector<uint8_t> data;
    data.reserve(4);

    int temp_offset = offset_address_ >> 4;
    if (temp_offset > 0xFFFFFF) { data.push_back(temp_offset >> 24); }
    if (temp_offset >   0xFFFF) { data.push_back(temp_offset >> 16); }

    data.push_back(temp_offset >>  8);
    data.push_back(temp_offset      );


    lines_.push_back(MakeLine(0x0000, 0x02, data));
}

//**************************[MakeLine]*****************************************
std::string cHexfileWriter::MakeLine(const int address, const int cmd,
  const std::vector<uint8_t> &data) {

    uint8_t crc = 0;
    std::string result;
    result.reserve(7 + 2 * data.size() + 2);

    result = ':';

    result+= wepet::IntToHex(data.size(), 2);
    crc += data.size();

    result+= wepet::IntToHex(address >> 8, 2);
    crc += address >> 8;
    result+= wepet::IntToHex(address     , 2);
    crc += address     ;

    result+= wepet::IntToHex(cmd, 2);
    crc += cmd;

    for (int i = 0; i < data.size(); i++) {
        crc+= data[i];
        result+= wepet::IntToHex(data[i], 2);
    }

    crc = 1 + (~crc);
    result+= wepet::IntToHex(crc, 2);

    return result;
}


} // namespace wepet {
