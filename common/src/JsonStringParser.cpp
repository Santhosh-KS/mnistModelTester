#include "JsonStringParser.hpp"
#include <iostream>

JsonStringParser::JsonStringParser(std::string &str)
{
  Dom.Parse(str.c_str());
}

JsonStringParser::~JsonStringParser()
{
  // Empty
}

std::string JsonStringParser::GetString(std::string key)
{
  std::string retVal("");
  if ( Dom.HasMember(key.c_str())) {
    retVal = Dom[key.c_str()].GetString();
  }
  else {
    std::cerr << "Invalid Key " << key.c_str() << "\n";
  }
  return retVal;
}

uint64_t JsonStringParser::GetUInt64(std::string key)
{
  uint64_t retVal;
  if ( Dom.HasMember(key.c_str())) {
    retVal = Dom[key.c_str()].GetUint64();
  }
  else {
    std::cerr << "Invalid Key " << key.c_str() << "\n";
  }
  return retVal;
}
