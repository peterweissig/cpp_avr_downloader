/******************************************************************************
*                                                                             *
* wepet_textfile.h                                                            *
* ================                                                            *
*                                                                             *
* Version: 1.0.8                                                              *
* Date   : 19.02.22                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_avr_downloader                        *
******************************************************************************/

#ifndef __WEPET_TEXTFILE_H
#define __WEPET_TEXTFILE_H

// local headers

// wepet headers

// standard headers
#include <string>
#include <vector>

// additional headers



namespace wepet {

//*****************************************************************************
//**************************{class cTextFile}**********************************
//*****************************************************************************
class cTextFile {
  public:
    cTextFile(void);
    cTextFile(const std::string filename);
    ~cTextFile(void);

    void Clear(void);

    int Size(void) const;
    std::string GetLine(const int number);
    bool SetLine(const int number, const std::string line);
    void AddLine(const std::string line);

    std::string SaveToString(void) const;
    bool LoadFromString(const std::string &str);

    bool SaveToFile(const std::string filename);
    bool LoadFromFile(const std::string filename);
    bool CheckIfFileExists(const std::string filename) const;

    std::string ReturnLastError(void);
    bool HasError(void) const;

  protected:
    bool SaveString(const std::string &filename, const std::string &data);
    bool LoadString(const std::string &filename, std::string &data);
    std::vector <std::string> lines_;
    std::string last_error_;
};

} // namespace wepet {
#endif // #ifndef __WEPET_TEXTFILE_H
