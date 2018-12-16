#include "RequestHandler.hpp"
#include "JsonStringParser.hpp"
#include "JsonFileParser.hpp"

#include <memory>

RequestHandler::RequestHandler(std::string &modelFile, std::string &respFile):
  ModelFile(modelFile),
  RespTemplateFile(respFile)
{
  GetSupportedModels();
}

RequestHandler::~RequestHandler()
{
  // Empty.
}

bool RequestHandler::GetSupportedModels()
{
  JsonFileParser parser(ModelFile);
  std::string key("Models");
  if (parser.IsArray(key)) {
    ModelsVector = parser.GetArrayString(key);
  }
  return ModelsVector.empty();
}

std::string RequestHandler::CreateResponse(ResponseElements &elements)
{
  std::string sessIdKey("SessionId");
  std::string statusKey("Status");
  std::string predKey("Prediction");

  JsonFileParser parser(RespTemplateFile);
  parser.SetString(sessIdKey, elements.SessionId);
  parser.SetString(statusKey, elements.StatusCode);
  parser.SetUInt64(predKey, elements.PredictionValue);

  return parser.GetStrigifiedJson();
}

std::string RequestHandler::ProcessRequest(std::string req)
{
  std::string sessIdKey("SessionId");
  std::string modelIdKey("ModelId");
  std::string imgUrlKey("ImageUrl");

  std::unique_ptr<JsonStringParser> reqParser =  std::make_unique<JsonStringParser>(req);

  std::string sessId(reqParser->GetString(sessIdKey));
  std::string imgUrl(reqParser->GetString(imgUrlKey));
  uint64_t modelId(reqParser->GetUInt64(modelIdKey));

  ResponseElements elements;
  elements.SessionId = sessId;
  elements.StatusCode = "200 OK";
  elements.PredictionValue = 9;
  return CreateResponse(elements);
  /*
  if (modelId > ModelsVector.size()) {
    // Models.json file mismatch.
  }
  else {
  }
  */
}
