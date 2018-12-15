#ifndef JSON_STRING_PARSER_HPP
#define JSON_STRING_PARSER_HPP

#include "rapidjson/document.h"
#include "NotCopyable.hpp"

class  JsonStringParser: public NotCopyable
{
  private:
    rapidjson::Document Dom;
    JsonStringParser() = delete;
  public:
    explicit JsonStringParser(std::string &str);
    ~JsonStringParser();
    std::string GetString(std::string key);
    uint64_t GetUInt64(std::string key);
};

#endif // JSON_STRING_PARSER_HPP
