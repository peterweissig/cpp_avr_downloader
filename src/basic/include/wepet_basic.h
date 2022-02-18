/******************************************************************************
*                                                                             *
* wepet_basic.h                                                               *
* =============                                                               *
*                                                                             *
* Version: 1.1.6                                                              *
* Date   : 18.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_basic/                                *
******************************************************************************/

#ifndef __WEPET_BASIC_H
#define __WEPET_BASIC_H

// local headers

// wepet headers

// standard headers
#include <string>

// additional headers



namespace wepet {

#define WEPET_ERROR_STRING std::string(__FILE__) + ":" + \
  wepet::IntToStr(__LINE__)

// Converts an integer to a hexadecimal number
// e.g. IntToHex(249) == "F9"
std::string IntToHex(const unsigned int value, const int digits = 2);

// Converts a hexadecimal number to an integer
// e.g. IntToHex("A5") == 165
int HexToInt(const std::string hex);

// Returns an right justified string
// e.g. IntToStr(25, 4) == "  25"
std::string IntToStr(const int value, const int digits = 0);

// Converts a string to an integer
// Possible formats:
//   ordinary integers
//   binary   numbers ("0b")
//   octalnumbers ("0o")
//   hexadecimals numbers ("0x")
// e.g. StrToInt("-0b1010", value) == true; value == -10
bool StrToInt(const std::string text, int &value);

// Returns a string equivalent of the boolean value
// e.g. BoolToStr(false) == "false"
std::string BoolToStr(const bool value);

// Converts all single letters to lower case
// e.g. LowerCase("Hello World!") == "hello world!"
std::string LowerCase(const std::string text);

// Converts all single letters to upper case
// e.g. UpperCase("Hello World!") == "HELLO WORLD!"
std::string UpperCase(const std::string text);

// Paraphrases and eventually trims a string for display
// e.g. StrOut("This is a very long text.", 15) == "\"This is [...]\""
std::string StrOut(const std::string text, const int length);

} // namespace wepet {
#endif // #ifndef __WEPET_BASIC_H
