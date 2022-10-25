//csv_readrow.h
#include <istream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

//added to deal with BOM potentially in string (negative ascii characters
static int csv_isspace(int i)
{
    if (i < 0 || i > 255)
        return 1;
    return std::isspace(i);
}
// trim from start
//static inline std::string &ltrim(std::string &s) {
//        s.erase(s.begin(), std::find_if(s.begin(), s.end(), csv_isspace));
 //       return s;
//}
static inline std::string& ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(csv_isspace))));
    return s;
}// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(csv_isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

std::vector<std::string> csv_read_row(std::istream &in, char delimiter);
std::vector<std::string> csv_read_row(std::string &in, char delimiter);

int getColIndex(std::string colName, std::vector<std::string> colNames);
//std::string getColData(int colIndex