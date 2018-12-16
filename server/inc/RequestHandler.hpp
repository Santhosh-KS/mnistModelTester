#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <string>
#include "NotCopyable.hpp"
#include "JsonStringParser.hpp"

#include <string>
#include <vector>

class RequestHandler : public NotCopyable
{
  private:
    std::vector<std::string> ModelsVector;
    std::string ModelFile;
    std::string RespTemplateFile;
    std::string ResponseTemplate;

    struct ResponseElements {
      std::string SessionId;
      std::string StatusCode;
      uint64_t PredictionValue;
    };

    bool GetSupportedModels();
    inline bool HaveModelsToWork() { return ModelsVector.empty(); }
    std::string CreateResponse(ResponseElements &elements);
    void GetResponseTemplate();
  public:
    RequestHandler(std::string &modelFile, std::string &respFile);
    ~RequestHandler();
    std::string ProcessRequest(std::string req);
};

#endif // REQUEST_HANDLER_HPP
