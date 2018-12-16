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

bool JsonStringParser::SetString(std::string key, std::string val)
{
  bool retVal(false);
  if ( Dom.HasMember(key.c_str())) {
    Dom[key.c_str()].SetString(val.c_str(),val.length());
    retVal = true;
  }
  else {
    std::cerr << "Invalid Key " << key.c_str() << "\n";
  }
  return retVal;
}

bool JsonStringParser::SetInt(std::string key, uint64_t val)
{
  bool retVal(false);
  if ( Dom.HasMember(key.c_str())) {
    Dom[key.c_str()].SetUint64(val);
    retVal = true;
  }
  else {
    std::cerr << "Invalid Key " << key.c_str() << "\n";
  }
  return retVal;
}

std::string JsonStringParser::GetStrigifiedJson()
{
  //printf("\nModified JSON with reformatting:\n");
  rapidjson::StringBuffer sb;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
  Dom.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
  std::cout << "stringify : " << sb.GetString() << "\n";
  return sb.GetString();
}


