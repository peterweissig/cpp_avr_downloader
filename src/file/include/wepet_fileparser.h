/******************************************************************************
*                                                                             *
* wepet_fileparser.h                                                          *
* ==================                                                          *
*                                                                             *
* Version: 1.0.3                                                              *
* Date   : 30.01.19                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_file/                                 *
******************************************************************************/

#ifndef __WEPET_FILEPARSER_H
#define __WEPET_FILEPARSER_H

// local headers

// wepet headers

// standard headers
#include <string>
#include <vector>

// additional headers



namespace wepet {

//*****************************************************************************
//**************************{class cFileParserElement}*************************
//*****************************************************************************
class cFileParserElement {
public:
    cFileParserElement(const std::string key = "");
    const cFileParserElement& operator = (
      const cFileParserElement& other);
    const std::string operator [] (const unsigned int index) const;

    void clear(void);

    bool setKey(const std::string key);
    const std::string getKey  (void) const;
    const bool   isValid (void) const;
    const bool   isEmpty (void) const;
    const bool   isVector(void) const;
    const unsigned int size (void) const;

    bool setValue(const std::string &value);
    bool setValue(const int    &value);
    bool setValue(const double &value);
    bool setValue(const bool   &value);
    bool setValue(const std::vector<std::string> &value);
    bool setValue(const std::vector<int   > &value);
    bool setValue(const std::vector<double> &value);
    bool setValue(const std::vector<bool  > &value);

    bool getValue(std::string &value) const;
    bool getValue(int    &value) const;
    bool getValue(double &value) const;
    bool getValue(bool   &value) const;
    bool getValue(std::vector<std::string> &value) const;
    bool getValue(std::vector<int   > &value) const;
    bool getValue(std::vector<double> &value) const;
    bool getValue(std::vector<bool  > &value) const;

    std::string intToStr   (const int    &value) const;
    std::string doubleToStr(const double &value) const;
    std::string boolToStr  (const bool   &value) const;
    bool strToInt   (const std::string &str, int    &value) const;
    bool strToDouble(const std::string &str, double &value) const;
    bool strToBool  (const std::string &str, bool   &value) const;

private:
    std::string key_;
    bool is_vector_;

    std::vector<std::string> strings_;
};

//*****************************************************************************
//**************************{class cFileParser}********************************
//*****************************************************************************
class cFileParser {
  public:
    cFileParser(int key_length = -1, char separator = '=');
      /// length >=0 fixed; < 0 dynamic
      /// seperator symbol between key and value

    void clear(void);
    const cFileParser& operator = (const cFileParser &other);
    const cFileParserElement& operator [] (
      const unsigned int index) const;

    std::string saveToString(void) const;
    bool loadFromString(const std::string &str);
    bool saveToFile(const std::string filename);
    bool loadFromFile(const std::string filename);

    cFileParserElement& findKey (const std::string key);
    const cFileParserElement& findKey (const std::string key) const;
    bool checkKey (const std::string key) const;

    cFileParserElement& createKey (const std::string key);
    bool deleteKey (const std::string key);

    int getCount(void) const;

    bool setValue(const std::string key, const std::string &value);
    bool setValue(const std::string key, const int    &value);
    bool setValue(const std::string key, const double &value);
    bool setValue(const std::string key, const bool   &value);
    bool setValue(const std::string key, const std::vector<std::string> &value);
    bool setValue(const std::string key, const std::vector<int   > &value);
    bool setValue(const std::string key, const std::vector<double> &value);
    bool setValue(const std::string key, const std::vector<bool  > &value);

    bool getValue(const std::string key, std::string &value) const;
    bool getValue(const std::string key, int    &value) const;
    bool getValue(const std::string key, double &value) const;
    bool getValue(const std::string key, bool   &value) const;
    bool getValue(const std::string key, std::vector<std::string> &value) const;
    bool getValue(const std::string key, std::vector<int   > &value) const;
    bool getValue(const std::string key, std::vector<double> &value) const;
    bool getValue(const std::string key, std::vector<bool  > &value) const;

    void extendString(std::string &str, const unsigned int length) const;

    std::string ReturnLastError(void);
    bool HasError(void) const;

  private:
    int  key_length_;
    char separator_ ;

    std::vector<cFileParserElement> elems_;

    std::string last_error_;
};

} // namespace wepet {
#endif //ifndef __WEPET_FILEPARSER_H
