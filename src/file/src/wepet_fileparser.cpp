/******************************************************************************
*                                                                             *
* wepet_fileparser.cpp                                                        *
* ====================                                                        *
*                                                                             *
* Version: 1.0.3                                                              *
* Date   : 30.01.19                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/cpp_file/                                 *
******************************************************************************/

// local headers
#include "wepet_fileparser.h"

// wepet headers

// standard headers
#include <sstream>
#include <fstream>

// additional headers



namespace wepet {

//*****************************************************************************
//**************************{class cFileParserElement}*************************
//*****************************************************************************


//**************************[cFileParserElement]*******************************
cFileParserElement::cFileParserElement(const std::string key) {

    is_vector_ = false;
    setKey(key);
}

//**************************[operator = ]**************************************
const cFileParserElement& cFileParserElement::operator = (
  const cFileParserElement& other) {

    key_       = other.key_;
    is_vector_ = other.is_vector_;
    strings_   = other.strings_;

    return *this;
}

//**************************[operator [] ]*************************************
const std::string cFileParserElement::operator [] (
  const unsigned int index) const {

    if (strings_.size() <= index) {
        return "";
    } else {
        return strings_[index];
    }
}
//**************************[clear]********************************************
void cFileParserElement::clear(void) {

    key_ = "";
    is_vector_ = false;
    strings_.clear();
}

//**************************[setKey]*******************************************
bool cFileParserElement::setKey(const std::string key) {

    if (key == "") {
        return false;
    }

    if (key.find_first_of("\" ,[]#") == std::string::npos) {
        key_ = key;
        return true;
    } else {
        key_ = "";
        return false;
    }
}

//**************************[getKey]*******************************************
const std::string cFileParserElement::getKey(void) const {

    return key_;
}

//**************************[isValid]******************************************
const bool cFileParserElement::isValid(void) const {

    return key_.size() > 0;
}

//**************************[isEmpty]******************************************
const bool cFileParserElement::isEmpty(void) const {

    return strings_.size() < 1;
}

//**************************[isVector]*****************************************
const bool cFileParserElement::isVector(void) const {

    return is_vector_;
}

//**************************[size]*********************************************
const unsigned int cFileParserElement::size(void) const {

    return strings_.size();
}

//**************************[setValue]*****************************************
bool cFileParserElement::setValue(const std::string &value) {

    is_vector_ = false;
    strings_.clear();
    if (value.find('"') == std::string::npos) {
        strings_.push_back(value);
        return true;
    }

    return false;
}

//**************************[setValue]*****************************************
bool cFileParserElement::setValue(const int &value) {

    return setValue(intToStr(value));
}

//**************************[setValue]*****************************************
bool cFileParserElement::setValue(const double &value) {

    return setValue(doubleToStr(value));
}

//**************************[setValue]*****************************************
bool cFileParserElement::setValue(const bool &value) {

    return setValue(boolToStr(value));
}

//**************************[setValue]*****************************************
bool cFileParserElement::setValue(const std::vector<std::string> &value) {

    is_vector_ = true;

    for (unsigned int i = 0; i < value.size(); i++) {
        if (value[i].find('"') >= 0) {
          strings_.clear();
          return false;
        }
    }

    strings_ = value;
    return true;
}

//**************************[setValue]*****************************************
bool cFileParserElement::setValue(const std::vector<int> &value) {

    strings_.resize(value.size());
    for (unsigned int i = 0; i < value.size(); i++) {
        std::string temp;
        temp = intToStr(value[i]);
        if (temp.find('"') == std::string::npos) {
            strings_[i] = temp;
        } else {
            strings_.clear();
            return false;
        }
    }

    return true;
}

//**************************[setValue]*****************************************
bool cFileParserElement::setValue(const std::vector<double> &value) {

    strings_.resize(value.size());
    for (unsigned int i = 0; i < value.size(); i++) {
        std::string temp;
        temp = doubleToStr(value[i]);
        if (temp.find('"') == std::string::npos) {
            strings_[i] = temp;
        } else {
            strings_.clear();
            return false;
        }
    }

    return true;
}

//**************************[setValue]*****************************************
bool cFileParserElement::setValue(const std::vector<bool> &value) {

    strings_.resize(value.size());
    for (unsigned int i = 0; i < value.size(); i++) {
        std::string temp;
        temp = boolToStr(value[i]);
        if (temp.find('"') == std::string::npos) {
            strings_[i] = temp;
        } else {
            strings_.clear();
            return false;
        }
    }

    return true;
}

//**************************[getValue]*****************************************
bool cFileParserElement::getValue(std::string &value) const {

    if (strings_.size() < 1) { return false; }

    value = strings_[0];
    return true;
}

//**************************[getValue]*****************************************
bool cFileParserElement::getValue(int &value) const {

    if (strings_.size() < 1) { return false; }
    return strToInt(strings_[0], value);
}

//**************************[getValue]*****************************************
bool cFileParserElement::getValue(double &value) const {

    if (strings_.size() < 1) { return false; }
    return strToDouble(strings_[0], value);
}

//**************************[getValue]*****************************************
bool cFileParserElement::getValue(bool &value) const {

    if (strings_.size() < 1) { return false; }
    return strToBool(strings_[0], value);
}

//**************************[getValue]*****************************************
bool cFileParserElement::getValue(std::vector<std::string> &value) const {

    value = strings_;
    return true;
}

//**************************[getValue]*****************************************
bool cFileParserElement::getValue(std::vector<int> &value) const {

    value.resize(strings_.size());
    for (unsigned int i = 0; i < value.size(); i++) {
        if (! strToInt(strings_[i], value[i])) {
            value.resize(0);
            return false;
        }
    }

    return true;
}

//**************************[getValue]*****************************************
bool cFileParserElement::getValue(std::vector<double> &value) const {

    value.resize(strings_.size());
    for (unsigned int i = 0; i < value.size(); i++) {
        if (! strToDouble(strings_[i], value[i])) {
            value.resize(0);
            return false;
        }
    }

    return true;
}

//**************************[getValue]*****************************************
bool cFileParserElement::getValue(std::vector<bool> &value) const {

    value.resize(strings_.size());
    for (unsigned int i = 0; i < value.size(); i++) {
        bool temp;
        if (! strToBool(strings_[i], temp)) {
            value.resize(0);
            return false;
        }
        value[i] = temp;
    }

    return true;
}

//**************************[intToStr]*****************************************
std::string cFileParserElement::intToStr(const int &value) const {

    std::stringstream ss;
    ss << value;
    return ss.str();
}

//**************************[doubleToStr]**************************************
std::string cFileParserElement::doubleToStr(const double &value) const {

    std::stringstream ss;
    ss << value;
    return ss.str();
}

//**************************[boolToStr]****************************************
std::string cFileParserElement::boolToStr(const bool &value) const {

    if (value) {
        return "true";
    } else {
        return "false";
    }
}

//**************************[strToInt]*****************************************
bool cFileParserElement::strToInt(const std::string &str,
  int &value) const {

    std::stringstream ss(str);
    try {
      ss >> value;
    } catch (std::exception& e) {
        return false;
    }
    return true;
}

//**************************[strToDouble]**************************************
bool cFileParserElement::strToDouble(const std::string &str,
  double &value) const {

    std::stringstream ss(str);
    try {
        ss >> value;
    } catch (std::exception& e) {
        return false;
    }
    return true;
}

//**************************[strToBool]****************************************
bool cFileParserElement::strToBool(const std::string &str,
  bool &value) const {


    if ((str == "true") || (str == "1")) {
        value = true;
        return true;
    }

    if ((str == "false") || (str == "0") || (str == "")) {
        value = false;
        return true;
    }

    return false;
}

//*****************************************************************************
//**************************{class cFileParser}********************************
//*****************************************************************************

//**************************[cFileParser]**************************************
cFileParser::cFileParser(int key_length, char separator) {

    last_error_ = "";
    key_length_ = key_length;
    separator_  = '=';

    switch (separator) {
        case ('\n'):
            last_error_ = "invalid separator '\\n'";
            break;
        case ('\r'):
            last_error_ = "invalid separator '\\r'";
            break;
        case ('"'):
        case (' '):
        case (','):
        case ('['):
        case (']'):
            last_error_ = "invalid separator '";
            last_error_+= separator;
            last_error_+= '\'';
            break;
        default   :
            separator_ = separator;
    }
}

//**************************[clear]********************************************
void cFileParser::clear() {

    last_error_ = "";
    elems_.resize(0);
}

//**************************[operator = ]**************************************
const cFileParser& cFileParser::operator = (
  const cFileParser& other) {

    key_length_ = other.key_length_;
    separator_  = other.separator_ ;
    last_error_ = other.last_error_;
    elems_      = other.elems_     ;

    return *this;
}

//**************************[operator [] ]*************************************
const cFileParserElement& cFileParser::operator [] (
  const unsigned int index) const {

    if ((index < 0) || (index >= elems_.size())) {
        return *((cFileParserElement*) NULL);
    }

    return elems_[index];
}

//**************************[saveToString]*************************************
std::string cFileParser::saveToString(void) const {

    int current_length = 0;
    if (key_length_ >= 0) {
        current_length = key_length_;
    } else {
        for (unsigned int i = 0; i < elems_.size(); i++) {
            int temp = elems_[i].getKey().size();
            if (temp > current_length) {
                current_length = temp;
            }
        }
    }

    std::string result;
    std::string temp;
    for (unsigned int i = 0; i < elems_.size(); i++) {

        temp = elems_[i].getKey();
        extendString(temp, current_length);

        temp+= ' ';
        temp+= separator_;
        if (elems_[i].isVector()) {
            temp+= '[';
        }

        for (unsigned int j = 0; j < elems_[i].size(); j++) {
            if (j > 0) {
                temp+= ',';
            }
            temp+= '"';
            temp+= elems_[i][j];
            temp+= '"';
        }

        if (elems_[i].isVector()) {
            temp+= ']';
        }
        result+= temp;
        result+= '\r';
        result+= '\n';
    }

    return result;
}

//**************************[loadFromString]***********************************
bool cFileParser::loadFromString(const std::string &str) {

    clear();

    int pos_max = str.size() - 1;

    int level = 0; //  0: start of line   - looking for key
                   //  1: within key      - looking for end of key
                   //  2: after key       - looking for separator
                   //  3: after separator - looking for vector or scalar
                   //  4: within scalar   - looking for end of scalar
                   //  5: within vector   - looking for scalar (vector)
                   //  6: within scalar   - looking for end of scalar (vector)
                   //  7: after scalar    - looking for komma (vector)
                   //  8: need scalar     - looking for scalar (vector)
                   // 99: no more data    - looking for end-of-line

    int start_s = 0;
    std::vector<std::string> ss;

    cFileParserElement elem;

    for (int pos = 0; pos <= pos_max; pos++) {

        char c = str[pos];

        if ((c == '\n') || (c == '\r')) {
            if ((level == 0) || (level == 3) || (level == 99)) {
                if ((level > 2) && (elem.isValid())) {
                    elems_.push_back(elem);
                }
                level = 0;
                start_s = pos+1;
                continue;
            }

            last_error_ = "unexpected end-of-line";
            if (level > 1) {
                last_error_+= " for key \"";
                last_error_+= elem.getKey();
                last_error_+='"';
            }

            return false;
        }

        if (c == ' ') {
            if (level == 1) {
                level = 2;

                if (! elem.setKey(str.substr(start_s, pos - start_s))) {
                    last_error_ = "invalid key \"";
                    last_error_+= str.substr(start_s, pos - start_s);
                    last_error_+='"';
                    return false;
                }
            }
            continue;
        } else {
            switch (level) {
                case  0:
                    level = 1;
                    start_s = pos;
                    continue;
                case  2:
                    level = 3;
                    continue;
                case  3:
                    if (c == '[') {
                        level = 5;
                    } else if (c == '"') {
                        level = 4;
                        start_s = pos+1;
                    } else {
                        last_error_ = "invalid symbol \"";
                        last_error_+= c;
                        last_error_+= "\" for data field of key \"";
                        last_error_+= elem.getKey();
                        last_error_+= '"';
                        return false;
                    }
                    continue;
                case  5:
                    if (c == '"') {
                        level = 6;
                        start_s = pos+1;
                    } else if (c == ']') {
                        level = 99;
                    } else {
                        last_error_ = "invalid symbol \"";
                        last_error_+= c;
                        last_error_+= "\" for data field of key \"";
                        last_error_+= elem.getKey();
                        last_error_+= '"';
                        return false;
                    }
                    continue;
                case  7:
                    if (c == ',') {
                        level = 8;
                    } else if (c == ']') {
                        level = 99;
                    } else {
                        last_error_ = "invalid symbol \"";
                        last_error_+= c;
                        last_error_+= "\" for data field of key \"";
                        last_error_+= elem.getKey();
                        last_error_+= '"';
                        return false;
                    }
                    continue;
                case  8:
                    if (c == '"') {
                        level = 6;
                        start_s = pos+1;
                    } else {
                        last_error_ = "invalid symbol \"";
                        last_error_+= c;
                        last_error_+= "\" for data field of key \"";
                        last_error_+= elem.getKey();
                        last_error_+= '"';
                        return false;
                    }
                    continue;
                case  99:
                    last_error_ = "expected end-of-line instead of symbol \"";
                    last_error_+= c;
                    last_error_+= "\" for key \"";
                    last_error_+= elem.getKey();
                    last_error_+= '"';
                    return false;

            }
        }

        if (c == '"') {
            switch (level) {
                case  4:
                    level = 99;
                    elem.setValue(str.substr(start_s, pos - start_s));
                    continue;
                case  6:
                    level = 7;
                    ss.push_back(str.substr(start_s, pos - start_s));
                    continue;
                default:
                    last_error_ = "unexpected symbol (\")";
                    if (level > 1) {
                        last_error_+= " for key \"";
                        last_error_+= elem.getKey();
                        last_error_+='"';
                    }
                    return false;
            }
        }
    }

    if ((level == 0) || (level == 3) || (level == 99)) {
        return true;
    }

    last_error_ = "unexpected end-of-file";
    if (level > 1) {
        last_error_+= " for key \"";
        last_error_+= elem.getKey();
        last_error_+='"';
    }
    return false;
}

//**************************[saveToFile]***************************************
bool cFileParser::saveToFile(const std::string filename) {

    if (filename == "") {
        last_error_ = "invalid filename \"\"";
        return false;
    }

    std::ofstream ofile(filename.data());
    if (!ofile) {
        last_error_ = "can not open file \"" + filename + "\"";
        return false;
    }
    ofile << saveToString();
    ofile.close();

    return true;
}

//**************************[loadFromFile]*************************************
bool cFileParser::loadFromFile(const std::string filename) {

    if (filename == "") {
        last_error_ = "invalid filename \"\"";
        return false;
    }

    std::ifstream ifile(filename.data());
    if (!ifile) {
        last_error_ = "can not open file \"" + filename + "\"";
        return false;
    }
    std::stringstream buffer;
    buffer << ifile.rdbuf();
    ifile.close();

    return loadFromString(buffer.str());
}

//**************************[findKey]******************************************
cFileParserElement& cFileParser::findKey (const std::string key) {

    if (key == "") { return *((cFileParserElement*) NULL);}

    for (unsigned int i = 0; i < elems_.size(); i++) {
        if (key == elems_[i].getKey()) {
            return elems_[i];
        }
    }

    return *((cFileParserElement*) NULL);
}

//**************************[findKey]******************************************
const cFileParserElement& cFileParser::findKey (
  const std::string key) const {

    if (key == "") { return *((cFileParserElement*) NULL);}

    for (unsigned int i = 0; i < elems_.size(); i++) {
        if (key == elems_[i].getKey()) {
            return elems_[i];
        }
    }

    return *((cFileParserElement*) NULL);
}

//**************************[checkKey]*****************************************
bool cFileParser::checkKey(const std::string key) const {

    return &(findKey(key)) != NULL;
}

//**************************[createKey]****************************************
cFileParserElement& cFileParser::createKey(
  const std::string key) {

    cFileParserElement& found_elem = findKey(key);
    if (&found_elem != NULL) {
        return found_elem;
    }

    cFileParserElement new_elem(key);
    if (! new_elem.isValid()) {
        last_error_ = "invalid key(";
        last_error_+= key;
        last_error_+= ')';
        return *((cFileParserElement*) NULL);
    }

    unsigned int temp = elems_.size();
    elems_.push_back(new_elem);

    if (temp == elems_.size()) {
        return *((cFileParserElement*) NULL);
    }

    return elems_.back();
}

//**************************[getCount]*****************************************
int cFileParser::getCount() const {

    return elems_.size();
}

//**************************[setValue]*****************************************
bool cFileParser::setValue(const std::string key,
  const std::string &value) {

    cFileParserElement* pElem;
    pElem = &(createKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->setValue(value);
}

//**************************[setValue]*****************************************
bool cFileParser::setValue(const std::string key, const int &value) {

    cFileParserElement* pElem;
    pElem = &(createKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->setValue(value);
}

//**************************[setValue]*****************************************
bool cFileParser::setValue(const std::string key, const double &value) {

    cFileParserElement* pElem;
    pElem = &(createKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->setValue(value);
}

//**************************[setValue]*****************************************
bool cFileParser::setValue(const std::string key, const bool &value) {

    cFileParserElement* pElem;
    pElem = &(createKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->setValue(value);
}

//**************************[setValue]*****************************************
bool cFileParser::setValue(const std::string key,
  const std::vector<std::string> &value) {

    cFileParserElement* pElem;
    pElem = &(createKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->setValue(value);
}

//**************************[setValue]*****************************************
bool cFileParser::setValue(const std::string key,
  const std::vector<int> &value) {

    cFileParserElement* pElem;
    pElem = &(createKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->setValue(value);
}

//**************************[setValue]*****************************************
bool cFileParser::setValue(const std::string key,
  const std::vector<double> &value) {

    cFileParserElement* pElem;
    pElem = &(createKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->setValue(value);
}

//**************************[setValue]*****************************************
bool cFileParser::setValue(const std::string key,
  const std::vector<bool> &value) {

    cFileParserElement* pElem;
    pElem = &(createKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->setValue(value);
}

//**************************[extendString]*************************************
void cFileParser::extendString(std::string &str,
  const unsigned int length) const {

    if (length <= str.size()) {
        return;
    }

    str.reserve(length);
    for (unsigned int i = str.size(); i < length; i++) {
        str+= ' ';
    }
}

//**************************[getValue]*****************************************
bool cFileParser::getValue(const std::string key, std::string &value)
  const {

    const cFileParserElement* pElem;
    pElem = &(findKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->getValue(value);
}

//**************************[getValue]*****************************************
bool cFileParser::getValue(const std::string key, int &value)
  const {

    const cFileParserElement* pElem;
    pElem = &(findKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->getValue(value);
}

//**************************[getValue]*****************************************
bool cFileParser::getValue(const std::string key, double &value) const {

    const cFileParserElement* pElem;
    pElem = &(findKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->getValue(value);
}

//**************************[getValue]*****************************************
bool cFileParser::getValue(const std::string key, bool &value) const {

    const cFileParserElement* pElem;
    pElem = &(findKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->getValue(value);
}


//**************************[getValue]*****************************************
bool cFileParser::getValue(const std::string key,
  std::vector<std::string> &value) const {

    const cFileParserElement* pElem;
    pElem = &(findKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->getValue(value);
}

//**************************[getValue]*****************************************
bool cFileParser::getValue(const std::string key, std::vector<int> &value)
  const {

    const cFileParserElement* pElem;
    pElem = &(findKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->getValue(value);
}

//**************************[getValue]*****************************************
bool cFileParser::getValue(const std::string key,
  std::vector<double> &value) const {

    const cFileParserElement* pElem;
    pElem = &(findKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->getValue(value);
}

//**************************[getValue]*****************************************
bool cFileParser::getValue(const std::string key, std::vector<bool> &value)
  const {

    const cFileParserElement* pElem;
    pElem = &(findKey(key));
    if (pElem == NULL) {
        return false;
    }

    return pElem->getValue(value);
}

//**************************[ReturnLastError]**********************************
std::string cFileParser::ReturnLastError(void) {

    std::string result = last_error_;
    last_error_ = "";
    return result;
}

//**************************[HasError]*****************************************
bool cFileParser::HasError(void) const {

    return last_error_ != "";
}

} // namespace wepet {
