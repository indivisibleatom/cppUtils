#ifndef _STR_HELPERS_H_
#define _STR_HELPERS_H_

#include <string>
#include <vector>

//Convert string to type T. Partially specialized in strHelpers.cpp
template <class T>
T strtoT(const char* str, char** endPgettr);

//Split a vector of strings at delimeter into the passed in elems vector. Also
//returns a reference to the elems vector for chaining
std::vector<std::string>& split(const std::string& s, char delim,
                                std::vector<std::string>& elems);

//Inplace replace the first occurrance of a string's from characters to
//characters. Returns true / false depending on if the replace occurred or not
bool replace(std::string& str, const std::string& from, const std::string& to);

#endif  //_STR_HELPERS_H_
