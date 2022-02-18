/******************************************************************************
*                                                                             *
* wepet_basic.cpp                                                             *
* ===============                                                             *
*                                                                             *
* Version: 1.1.6                                                              *
* Date   : 18.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_basic/                                *
******************************************************************************/

// local headers
#include "wepet_basic.h"

// wepet headers

// standard headers

// additional headers



//**************************[IntToHex]*****************************************
std::string wepet::IntToHex(const unsigned int value, const int digits) {

    std::string result;

    unsigned int value_ = value;
    int max_digits = digits >= 0 ? digits : -digits;

    while (value_) {
        int temp = value_ % 16;
        value_ = value_ >> 4;

        if (temp < 10) {
            result = char(48 + temp) + result;
        } else {
            result = char(55 + temp) + result;
        }

        if (max_digits > 0) {
            max_digits--;
            if (max_digits < 1) { break; }
        }
    }

    while (max_digits) {
        result = '0' + result;

        max_digits--;
    }
    if (digits < 0) {
        result = "0x" + result;
    }

    return result;
}

//**************************[HexToInt]*****************************************
int wepet::HexToInt(const std::string hex) {

    int result = 0;

    for (int a = 0; a < hex.size(); a++) {
        char c = hex[a];
        result = result << 4;

        if (('0' <= c) && (c <= '9')) {
            result+= ((int) c) - ((int) '0')     ;

        } else if (('A' <= c) && (c <= 'F')) {
            result+= ((int) c) - ((int) 'A') + 10;

        } else if (('a' <= c) && (c <= 'f')) {
            result+= ((int) c) - ((int) 'a') + 10;

        } else {
            return -1;
        }
    }

    return result;
}

//**************************[IntToStr]*****************************************
std::string wepet::IntToStr(const int value, const int digits) {

    std::string result;

    int min_digits = digits >= 0 ? digits : -digits;

    unsigned int value_;

    bool minus = false;
    if (value >= 0) {
        value_ = value;
    } else {
        minus = true;
        value_ = -value;
        min_digits--;
    }

    while (value_) {
        result = char(48 + value_ % 10) + result;
        value_/= 10;

        min_digits--;
    }
    if (result == "") { result = '0'; min_digits--; }

    if (digits < 0) {
        while (min_digits > 0) {
            result = '0' + result;
            min_digits--;
        }
        if (minus) {
            result = '-' + result;
        }
    } else {
        if (minus) {
            result = '-' + result;
        }
        while (min_digits > 0) {
            result = ' ' + result;
            min_digits--;
        }
    }

    return result;
}

//**************************[StrToInt]*****************************************
bool wepet::StrToInt(std::string text, int &value) {

    int result = 0;
    bool minus = false;
    int pos = 0;
    int base = 10;

    if (text.length() < 1) { return false;}

    if (text[0] == '-') {
        minus = true;
        pos = 1;
    }

    if (text.length() >= pos + 2) {
        if (text[pos + 1] == 'b') { base =  2; }
        if (text[pos + 1] == 'o') { base =  8; }
        if (text[pos + 1] == 'x') { base = 16; }

        if (base != 10) {
            if (text[pos] != '0') { return false; }
            pos+= 2;
        }
    }

    while (pos < text.length()) {
        char c = text[pos++];

        if ((c == 32) || (c == 13) || (c == 10)) { continue; }
        if ((c < '0') || (c > 'f')) { return false; }

        int d;
        if      (c < 'A') { d = c - '0'     ;}
        else if (c < 'a') { d = c - 'A' + 10;}
        else              { d = c - 'a' + 10;}
        if (d >= base) { return false; }

        result*= base;
        result+= d;
    }

    if (minus) {
        result = -result;
    }
    value = result;
    return true;
}

//**************************[BoolToStr]****************************************
std::string wepet::BoolToStr(const bool value) {

    if (value) {
        return "true";
    } else {
        return "false";
    }
}

//**************************[LowerCase]****************************************
std::string wepet::LowerCase(const std::string text) {

    std::string result = text;

    for (int i = 0; i < text.size(); i++) {
        char c = text[i];
        if (('A' <= c) && (c <= 'Z')) {
            c = c - 'A' + 'a';
            result[i] = c;
        }
    }
    return result;
}

//**************************[UpperCase]****************************************
std::string wepet::UpperCase(const std::string text) {

    std::string result = text;

    for (int i = 0; i < text.size(); i++) {
        char c = text[i];
        if (('a' <= c) && (c <= 'z')) {
            c = c - 'a' + 'A';
            result[i] = c;
        }
    }
    return result;
}

//**************************[UpperCase]****************************************
std::string wepet::StrOut(const std::string text, const int length) {
    std::string result;
    int maxi;

    if ((length <= 0) || (text.size() <= length - 2)){
        maxi = text.size();
    } else {
        maxi = length - 7;
    }

    for (int i = 0; i < maxi; i++) {
        char c = text[i];
        if (c < 32) {c = '.';}
        result+= c;
    }

    if (maxi < text.size()) {
        result = "\"" + result + "[...]\"";
    } else {
        result = "\"" + result + "\"";
    }
    return result;
}
