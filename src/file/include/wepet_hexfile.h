/******************************************************************************
*                                                                             *
* wepet_hexfile.h                                                             *
* ===============                                                             *
*                                                                             *
* Version: 1.0.4                                                              *
* Date   : 19.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_file/                                 *
******************************************************************************/

#ifndef __WEPET_HEXFILE_H
#define __WEPET_HEXFILE_H

// local headers

// wepet headers
#include "wepet_textfile.h"

// standard headers
#include <string>
#include <vector>

#include <stdint.h>

// additional headers



namespace wepet {

//*****************************************************************************
//**************************{class cHexfileReader}*****************************
//*****************************************************************************
class cHexfileReader : protected cTextFile {
  public:
    cHexfileReader();
    cHexfileReader(const std::string filename);
    ~cHexfileReader(void);

    void Reset(void);
    bool operator ++(void);

    bool GetData(int &address, uint8_t &data);
    bool IsEndOfFile(void) const;

    bool LoadFromFile(const std::string filename);
    bool LoadFromString(const std::string str);

    std::string ReturnLastError(void);
    bool HasError(void) const;

  private:
    bool InterpreteCurrentLine(void);

    int current_address_;
    int offset_address_;
    int current_line_pos_;
    int current_data_pos_;
    bool end_of_file_;

    std::vector<uint8_t> current_data_;
};

//*****************************************************************************
//**************************{class cHexfileWriter}*****************************
//*****************************************************************************
class cHexfileWriter : protected cTextFile {
  public:
    cHexfileWriter(const int max_bytes_per_line = 16);
    ~cHexfileWriter(void);

    void Clear(void);

    bool SetData(const int address, const uint8_t data);

    bool SaveToFile(const std::string filename);
    std::string SaveToString(void);

    std::string ReturnLastError(void);
    bool HasError(void) const;

  private:
    std::string ReturnEndOfFile(void);
    std::string ReturnCurrentLine(void);
    void StoreOffset(void);
    std::string MakeLine(const int address, const int cmd, const std::vector<uint8_t> &data);

    int max_bytes_per_line_;
    int current_address_;
    int offset_address_;

    std::vector<uint8_t> current_data_;
};

} // namespace wepet {
#endif // #ifndef __WEPET_HEXFILE_H
