/*
   MIT License

   Copyright (c) 2018 santhoshachar08@gmail.com

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <algorithm>

#include <sys/stat.h>

// system() cmd related.
#include <stdlib.h>
#include <signal.h>

#include <Wt/WMessageBox.h>
#include <Wt/WAnchor.h>
#include <Wt/WTable.h>
#include <Wt/WLength.h>

#include "ClientUi.hpp"
#include "UrlParser.hpp"
#include "TcpClient.hpp"
#include "JsonFileParser.hpp"
#include "JsonStringParser.hpp"
#include "Utils.hpp"


// Sets up the theme. Wt supports Bootstap (https://getbootstrap.com/)
// Setup Wt to use Boosstrap 3.
void ClientUiApplication::SetupTheme()
{
  useStyleSheet(Wt::WLink("css/styleSheet.css"));
  useStyleSheet(Wt::WLink("resources/themes/bootstrap/3/bootstrap.min.css"));
}

void ClientUiApplication::SetupHeader()
{
  Wt::WContainerWidget *HeaderDiv = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
  HeaderDiv->setId("header");
  Wt::WContainerWidget *HeaderDivTextDiv = HeaderDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  HeaderDivTextDiv->setId("h3");
  // Header text to be displayed
  Wt::WText *HeaderText = HeaderDivTextDiv->addWidget(std::make_unique<Wt::WText>(Wt::WString::fromUTF8("Onfido MNIST model tester")));
}

int ClientUiApplication::GetModelIndex()
{
  std::string selection(ModelComboBox->currentText().toUTF8());
  int val(-1);
  for(int i = 0; i < ModelsVector.size(); ++i) {
    if (selection.compare(ModelsVector[i]) == 0) {
      val = i;
      break;
    }
  }
  return val;
}

void ClientUiApplication::ParseResponse(std::string resp)
{
  std::string sessIdKey("SessionId");
  std::string statusKey("Status");
  std::string predKey("Prediction");

  std::cout << " resp = " << resp.c_str() << "\n";
  std::string err("ERROR");
  // Handle the error when connection to TCP socket is having some problem.
  if(resp.find(err) != std::string::npos) {
    Wt::WMessageBox::show("Information", resp.c_str(), Wt::StandardButton::Ok);
    return;
  }
  else {
    std::unique_ptr<JsonStringParser> respParser =  std::make_unique<JsonStringParser>(resp);;
    std::string sessId(respParser->GetString(sessIdKey));
    std::string status(respParser->GetString(statusKey));
    uint64_t prediction(respParser->GetUInt64(predKey));
   if (sessId.compare(sessionId()) == 0) {
      if (status.compare("200 OK") == 0) {
        if ( prediction < 10) {
          std::string msg = "Predicted value = " + std::to_string(prediction);
          Wt::WMessageBox::show("Information", msg.c_str() , Wt::StandardButton::Ok);
        }
        else {
          std::string msg("Invaid prediction by the selected model");
          Wt::WMessageBox::show("Information", msg.c_str(), Wt::StandardButton::Ok);
        }
      }
      else {
        std::string msg("Got Status ");
        msg += status + " from server";
        Wt::WMessageBox::show("Information", msg.c_str(), Wt::StandardButton::Ok);
      }
    }
    else {
      Wt::WMessageBox::show("Information", "Invalid Session ID." , Wt::StandardButton::Ok);
    }
  }
}

void ClientUiApplication::OnPredictButtonPressed()
{
  std::string serverIp("localhost");
  int port(5678);
  std::string jsonFile("/opt/onfido/data/Request.json");
  std::string sessIdKey("SessionId");

  JsonFileParser parser(jsonFile);
  TcpClient client(serverIp, port);

  std::string sessIdVal(sessionId());
  std::string modelIdKey("ModelId");
  std::string imgUrlKey("ImageUrl");

  int modelIndex(GetModelIndex());

  //std::cout << "Model Index = " << modelIndex << "\n";
  if (modelIndex >= 0 ) {
    auto url(SearchLineEdit->text().toUTF8());
    parser.SetString(sessIdKey, sessIdVal);
    parser.SetString(imgUrlKey, url);
    if (parser.SetUInt64(modelIdKey, modelIndex)) {
      std::string req(parser.GetStrigifiedJson());
      std::string resp(client.Connect(req));
      //std::cout << "Before parser resp = " << resp.c_str() << "\n";
      ParseResponse(resp);
    }
    else {
      Wt::WMessageBox::show("Information", "Internal Error 1." , Wt::StandardButton::Ok);
      return;
    }
  }
  else {
    Wt::WMessageBox::show("Information", "Internal Error 2." , Wt::StandardButton::Ok);
    return;
  }
}

void ClientUiApplication::SetupComboSelection(Wt::WContainerWidget *mainLeft)
{
  GetModelsFromJson();
  Wt::WContainerWidget *rowDiv = mainLeft->addWidget(std::make_unique<Wt::WContainerWidget>());
  rowDiv->setStyleClass("row");

  Wt::WContainerWidget *columnLeftDiv = rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  columnLeftDiv->setStyleClass("col-md-3  col-xs-1 col-sm-1");

  Wt::WContainerWidget *columnMiddleDiv = rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  columnMiddleDiv->setStyleClass("col-md-6  col-xs-3 col-sm-3");

  Wt::WContainerWidget *comboMainDiv = columnMiddleDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  comboMainDiv->setStyleClass("dropdown");

  Wt::WContainerWidget *dropDownBtnDiv= comboMainDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  dropDownBtnDiv->setStyleClass("btn dropdown-toggle btn-block");

  ModelComboBox = dropDownBtnDiv->addWidget(std::make_unique<Wt::WComboBox>());
  ModelComboBox->setStyleClass(Wt::WString::fromUTF8("btn btn-default with-label btn-block"));

  ModelComboBox->addItem("Pick one Model");
  // Here we add the drop down menu items dynamically.
  for (auto &v: ModelsVector) {
    ModelComboBox->addItem(v.c_str());
  }

  Wt::WContainerWidget *ButtonDiv = columnMiddleDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  ButtonDiv->setStyleClass("btn btn-block");
  PredictButton = ButtonDiv->addWidget(std::make_unique<Wt::WPushButton>("Predict"));
  PredictButton->setStyleClass(Wt::WString::fromUTF8("btn btn-success with-label btn-block"));

  ModelComboBox->changed().connect(this, &ClientUiApplication::OnComboButtonClicked);
  Wt::WContainerWidget *ColumnRightDiv= rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  ColumnRightDiv->setStyleClass("col-md-8  col-xs-1 col-sm-1");

  std::string selection(ModelComboBox->currentText().toUTF8());
  //std::cout << "selection = " << selection << "\n";
  PredictButton->clicked().connect(this, &ClientUiApplication::OnPredictButtonPressed);
  PredictButton->setEnabled(false);
  MainComboDiv->hide();
}

void ClientUiApplication::OnComboButtonClicked()
{
//  std::cout << "OnComboButtonClicked\n";
  std::string selection(ModelComboBox->currentText().toUTF8());
  auto pos = selection.find("Pick");
  if (pos == std::string::npos) {
    PredictButton->setEnabled(true);
  }
  else {
    PredictButton->setEnabled(false);
  }
}


void ClientUiApplication::SetupImageSearchBar(Wt::WContainerWidget *mainLeft)
{
  Wt::WContainerWidget *rowDiv = mainLeft->addWidget(std::make_unique<Wt::WContainerWidget>());
  rowDiv->setStyleClass("row");

  Wt::WContainerWidget *columnLeftDiv = rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  columnLeftDiv->setStyleClass("col-md-3  col-xs-1 col-sm-1");

  Wt::WContainerWidget *columnMiddleDiv = rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  columnMiddleDiv->setStyleClass("col-md-6  col-xs-3 col-sm-3");

  Wt::WContainerWidget *SearchDiv = columnMiddleDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  SearchDiv->setStyleClass("input-group");
  Wt::WContainerWidget *SearchDivLineEditDiv = SearchDiv->addWidget(std::make_unique<Wt::WContainerWidget>());

  SearchLineEdit = SearchDivLineEditDiv->addWidget(
      std::make_unique<Wt::WLineEdit>(Wt::WString::fromUTF8("Give me a digit image URL")));
  SearchLineEdit->setStyleClass("form-control");

  // As soon as the page loads; mouse/cursor should be focused on search window.
  SearchLineEdit->setFocus();

  Wt::WContainerWidget *ButtonDiv = SearchDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  ButtonDiv->setStyleClass("input-group-btn");
  SearchButton = ButtonDiv->addWidget(std::make_unique<Wt::WPushButton>("Search"));
  SearchButton->setStyleClass(Wt::WString::fromUTF8("btn btn-success with-label"));

  Wt::WContainerWidget *ColumnRightDiv= rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  ColumnRightDiv->setStyleClass("col-md-8  col-xs-1 col-sm-1");

  SearchLineEdit->enterPressed().connect
    (std::bind(&ClientUiApplication::OnSearchButtonPressed, this));

  SearchButton->clicked().connect(this, &ClientUiApplication::OnSearchButtonPressed);
}


void ClientUiApplication::SetupFooter()
{
  FooterDiv = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
  FooterDiv->setId("footer");
  Wt::WContainerWidget *FooterDivTextDiv = FooterDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  // Footer Text to display
  Wt::WText *Text = FooterDivTextDiv->addWidget(
      std::make_unique<Wt::WText>(
        Wt::WString::fromUTF8("© Onfido MNIST model tester. All Rights Reserved.")));
}

void ClientUiApplication::SetupImageWindow(Wt::WContainerWidget *mainImgDiv, std::string &url)
{
  if (!url.empty()) {

    Wt::WContainerWidget *rowDiv = mainImgDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    rowDiv->setStyleClass("row");

    Wt::WContainerWidget *columnLeftDiv = rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    columnLeftDiv->setStyleClass("col-md-3  col-xs-1 col-sm-1");

    Wt::WContainerWidget *columnMiddleDiv = rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    columnMiddleDiv->setStyleClass("col-md-6  col-xs-3 col-sm-3");

    Wt::WContainerWidget *ImageDiv = columnMiddleDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    ImageDiv->setStyleClass("input-group");
    Wt::WContainerWidget *SearchDivLineEditDiv = ImageDiv->addWidget(std::make_unique<Wt::WContainerWidget>());

    Image = ImageDiv->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(url.c_str())));
    Image->setStyleClass("img-responsive");
    Image->setAlternateText("Selected Digit Image");

    Wt::WContainerWidget *ColumnRightDiv= rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    ColumnRightDiv->setStyleClass("col-md-8  col-xs-1 col-sm-1");
  }
}

// Event and button click handlers.
void ClientUiApplication::OnSearchButtonPressed()
{
  auto url(SearchLineEdit->text().toUTF8());
  std::string sessId(sessionId());
  std::string cmd("curl --head --silent --fail " + url);
  Utils cmdUtil;
  int ret = cmdUtil.ExecuteCommand(cmd);
  if (ret == 0) {
    // Sanity check on the URL
    // since we are interested in image url,
    // we look for url with .jpeg/.jpg/.png extensions at the
    // end of the url.
    // Ex: http://images.google.com/abcd/efgh?sky=true&sky.jpg
    UrlParser parse(url);
    if (parse.IsValidUrl()) {
      std::cout << "URL: " << url.c_str() << "\n";
      root()->removeWidget(MainImageDiv);
      root()->removeWidget(FooterDiv);
      MainImageDiv = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
      SetupImageWindow(MainImageDiv, url);
      MainComboDiv->show();
      SetupFooter();
    }
    else {
      Wt::WMessageBox::show("Information", "Please give Valid URL with .jpg or .jpeg or .png extension.", Wt::StandardButton::Ok);
    }
  }
  else {
    Wt::WMessageBox::show("Information", "Please give Valid URL with .jpg or .jpeg or .png extension.", Wt::StandardButton::Ok);
  }
}

// GetModelsFromJson API will read the Models.json file,
// and stores the model's name present in the json file in to a vector.
bool ClientUiApplication::GetModelsFromJson()
{
  JsonFileParser parser("/opt/onfido/data/Models.json");
  std::string key("Models");
  if (parser.IsArray(key)) {
    ModelsVector = parser.GetArrayString(key);
    if (ModelsVector.empty()) {
      return false;
    }
    return true;
  }
  else {
    return false;
  }
}


ClientUiApplication::ClientUiApplication(const Wt::WEnvironment& env)
  : WApplication(env)
{
  setTitle("Onfido MNIST model Test suite"); // application title
  SetupTheme();
  SetupHeader();
  MainSearchDiv = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
  MainSearchDiv->setId("main_left");
  MainImageDiv = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
  MainComboDiv = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
  SetupImageSearchBar(MainSearchDiv);
  SetupComboSelection(MainComboDiv);
  std::string dummyUrl("");
  SetupImageWindow(MainImageDiv, dummyUrl);
  SetupFooter();
}

ClientUiApplication::~ClientUiApplication()
{
  // Empty;
}
