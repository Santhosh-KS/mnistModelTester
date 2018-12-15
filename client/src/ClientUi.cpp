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


int ClientUiApplication::ExecuteCommand(std::string &cmd)
{
  std::cout << "Executing CMD: " << cmd.c_str() << "\n";
  int ret = system(cmd.c_str());
  if (WIFSIGNALED(ret)
      && (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT)) {
    std::cerr << "ERROR: Failed to execute command " << cmd.c_str() << "\n";
  }
  return ret;
}

/*
bool ClientUiApplication::CheckFileExists(std::string &file)
{
  struct stat buffer;
  bool val(false);
  if ((stat(file.c_str(), &buffer) == 0)) {
    val = true;
    std::cout << "File " << file.c_str() << " Found.\n";
  }
  else {
    std::cout << "File " << file.c_str() << " Not Found.\n";
  }
  return val;
}
*/

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

// SetupComboBox API, sets up the Dropdown box for the model selection.
void ClientUiApplication::SetupComboBox(Wt::WContainerWidget *mainLeft)
{
  GetModelsFromJson();
  Wt::WContainerWidget *rowDiv = mainLeft->addWidget(std::make_unique<Wt::WContainerWidget>());
  rowDiv->setStyleClass("row");

  Wt::WContainerWidget *columnLeftDiv = rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  columnLeftDiv->setStyleClass("col-md-3  col-xs-1 col-sm-1");

  auto text = columnLeftDiv->addWidget(std::make_unique<Wt::WText>("Please select a Model:"));

  Wt::WContainerWidget *comboDiv = rowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  comboDiv->setStyleClass("dropdown");

  Wt::WContainerWidget *btnDiv= comboDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  btnDiv->setStyleClass("btn dropdown-toggle");

  ModelComboBox = comboDiv->addWidget(std::make_unique<Wt::WComboBox>());
  ModelComboBox->setStyleClass(Wt::WString::fromUTF8("btn btn-default with-label"));

  // Here we add the drop down menu items dynamically.
  for (auto &v: ModelsVector) {
    ModelComboBox->addItem(v.c_str());
  }

  auto out = comboDiv->addWidget(Wt::cpp14::make_unique<Wt::WText>());
  out->addStyleClass("help-block");
  ModelComboBox->changed().connect([=] {out->setText(Wt::WString("You selected {1}.").arg(ModelComboBox->currentText()));});

//  Wt::WContainerWidget *buttonDiv = comboDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
//  buttonDiv->setStyleClass("input-group-btn");
  PredictButton = comboDiv->addWidget(std::make_unique<Wt::WPushButton>("Predict"));
  PredictButton->setStyleClass(Wt::WString::fromUTF8("btn btn-success with-label"));

  PredictButton->clicked().connect(this, &ClientUiApplication::OnPredictButtonPressed);

  MainComboDiv->hide();
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


  //std::string dummy("{}");
  std::cout << " resp = " << resp.c_str() << "\n";
  std::unique_ptr<JsonStringParser> respParser =  std::make_unique<JsonStringParser>(resp);;
  //respParser = std::make_unique<JsonStringParser>(resp);
  std::cout << " It is fine here\n";
  std::string sessId(respParser->GetString(sessIdKey));
  std::string status(respParser->GetString(statusKey));
  uint64_t prediction(respParser->GetUInt64(predKey));
//  if (sessId.compare(sessionId()) == 0) {
    if (status.compare("200 OK") == 0) {
      if ( prediction < 10) {
        std::string msg = "Predicted value = " + std::to_string(prediction);
        Wt::WMessageBox::show("Information", msg.c_str() , Wt::StandardButton::Ok);
      }
      else {
        Wt::WMessageBox::show("Information", "Invalid Prediction" , Wt::StandardButton::Ok);
      }
    }
    else {
      Wt::WMessageBox::show("Information", "Status not ok!" , Wt::StandardButton::Ok);
    }
 // }
/*  else {
    Wt::WMessageBox::show("Information", "Invalid Session ID." , Wt::StandardButton::Ok);
  }*/
}

void ClientUiApplication::OnPredictButtonPressed()
{
  std::string serverIp("localhost");
  int port(5678);
  std::string jsonFile("/opt/onfido/Request.json");
  std::string sessIdKey("SessionId");

  JsonFileParser parser(jsonFile);
  TcpClient client(serverIp, port);

  std::string sessIdVal(sessionId());
  std::string modelIdKey("ModelId");

  int modelIndex(GetModelIndex());

  if (modelIndex >= 0 ) {
    parser.SetString(sessIdKey, sessIdVal);
    if (parser.SetUInt64(modelIdKey, modelIndex)) {
      std::string req(parser.GetStrigifiedJson());
      std::string resp(client.Connect(req));
      std::cout << "Before parser resp = " << resp.c_str() << "\n";
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

  /*SearchLineEdit = SearchDivLineEditDiv->addWidget(
      std::make_unique<Wt::WLineEdit>(Wt::WString::fromUTF8("Give me a digit image URL")));*/
  SearchLineEdit = SearchDivLineEditDiv->addWidget(
      std::make_unique<Wt::WLineEdit>(Wt::WString::fromUTF8("https://3qeqpr26caki16dnhd19sv6by6v-wpengine.netdna-ssl.com/wp-content/uploads/2016/05/Examples-from-the-MNIST-dataset.png")));
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
    Image->setAlternateText("Selected Digit Image");

    Wt::WContainerWidget *rowDiv1 = mainImgDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    rowDiv1->setStyleClass("row");

/*    Wt::WContainerWidget *columnLeftDiv1 = rowDiv1->addWidget(std::make_unique<Wt::WContainerWidget>());
    columnLeftDiv->setStyleClass("col-md-3  col-xs-1 col-sm-1");

    Wt::WContainerWidget *columnMiddleDiv1 = rowDiv1->addWidget(std::make_unique<Wt::WContainerWidget>());
    columnMiddleDiv1->setStyleClass("col-md-6  col-xs-3 col-sm-3");

    Wt::WContainerWidget *comboDiv = columnMiddleDiv1->addWidget(std::make_unique<Wt::WContainerWidget>());
    comboDiv->setStyleClass("dropdown");
*/
  }
}

// Event and button click handlers.
void ClientUiApplication::OnSearchButtonPressed()
{
  auto url(SearchLineEdit->text().toUTF8());
  std::string sessId(sessionId());
  std::string cmd("curl --head --silent --fail " + url);
  int ret = ExecuteCommand(cmd);
  if (ret == 0) {
    // Sanity check on the URL
    UrlParser parse(url);
    if (parse.IsValidUrl()) {
      std::cout << "Valid URL\n";
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
  JsonFileParser parser("/opt/onfido/Models.json");
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
  std::string dummyUrl("");
  SetupImageWindow(MainImageDiv, dummyUrl);
  SetupComboBox(MainComboDiv);
  SetupFooter();
}

ClientUiApplication::~ClientUiApplication()
{
  // Empty;
}
