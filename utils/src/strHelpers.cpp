#include <sstream>

#include "strHelpers.h"

template <>
float strtoT<float>(const char* str, char** endPtr) {
  return (float)strtod(str, endPtr);
}

template <>
double strtoT<double>(const char* str, char** endPtr) {
  return strtod(str, endPtr);
}

template <>
int strtoT<int>(const char* str, char** endPtr) {
  return (int)strtol(str, endPtr, 10);
}

template <>
long strtoT<long>(const char* str, char** endPtr) {
  return strtol(str, endPtr, 10);
}

std::vector<std::string>& split(const std::string& s, char delim,
                                std::vector<std::string>& elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

// Replace first occurance of from in str to to
bool replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}
