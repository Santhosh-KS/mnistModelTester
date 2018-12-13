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
//#include "TcpClient.hpp"
//#include "TcpServer.hpp"
//#include "JsonFileParser.hpp"

// GetImageFiles related includes.
#include <sys/types.h>
#include <dirent.h>

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

// Sets up the theme. Wt supports Booststap (https://getbootstrap.com/)
// Setup Wt to use Boosstrap 3.
void ClientUiApplication::SetupTheme()
{
  useStyleSheet(Wt::WLink("css/styleSheet.css")); // TODO: Configurable.
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

void ClientUiApplication::SetupImageSearchBar(Wt::WContainerWidget *mainLeft)
{
  Wt::WContainerWidget *RowDiv = mainLeft->addWidget(std::make_unique<Wt::WContainerWidget>());
  RowDiv->setStyleClass("row");

  Wt::WContainerWidget *ColumnLeftDiv = RowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  ColumnLeftDiv->setStyleClass("col-md-3  col-xs-1 col-sm-1");

  Wt::WContainerWidget *ColumnMiddleDiv = RowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  ColumnMiddleDiv->setStyleClass("col-md-6  col-xs-3 col-sm-3");

  Wt::WContainerWidget *SearchDiv = ColumnMiddleDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  SearchDiv->setStyleClass("input-group");
  Wt::WContainerWidget *SearchDivLineEditDiv = SearchDiv->addWidget(std::make_unique<Wt::WContainerWidget>());

  SearchLineEdit = SearchDivLineEditDiv->addWidget(std::make_unique<Wt::WLineEdit>(Wt::WString::fromUTF8("Give me a digit image URL")));
  SearchLineEdit->setStyleClass("form-control");

  // As soon as the page loads; mouse/cursor should be focused on search window.
  SearchLineEdit->setFocus();

  Wt::WContainerWidget *ButtonDiv = SearchDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  ButtonDiv->setStyleClass("input-group-btn");
  SearchButton = ButtonDiv->addWidget(std::make_unique<Wt::WPushButton>("Search"));
  SearchButton->setStyleClass(Wt::WString::fromUTF8("btn btn-success with-label"));

  Wt::WContainerWidget *ColumnRightDiv= RowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
  ColumnRightDiv->setStyleClass("col-md-3  col-xs-1 col-sm-1");

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
  Wt::WText *Text = FooterDivTextDiv->addWidget(std::make_unique<Wt::WText>(Wt::WString::fromUTF8("© Onfido MNIST model tester. All Rights Reserved.")));
}
/*
void ClientUiApplication::SetupModelComboBox()
{
    ModelComboBox = ImageDiv->addWidget(std::make_unique<Wt::WComboBox>());
    ModelComboBox->addItem("Heavy");   // 'Heavy' (index 0) is shown by default.
    ModelComboBox->addItem("Medium");
    ModelComboBox->addItem("Light");
}
*/

void ClientUiApplication::SetupImageWindow(Wt::WContainerWidget *mainImgDiv, std::string &url)
{
  if (!url.empty()) {

    Wt::WContainerWidget *RowDiv = mainImgDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    RowDiv->setStyleClass("row");

    Wt::WContainerWidget *ColumnLeftDiv = RowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    ColumnLeftDiv->setStyleClass("col-md-3  col-xs-1 col-sm-1");

    Wt::WContainerWidget *ColumnMiddleDiv = RowDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    ColumnMiddleDiv->setStyleClass("col-md-6  col-xs-3 col-sm-3");

    Wt::WContainerWidget *ImageDiv = ColumnMiddleDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    ImageDiv->setStyleClass("input-group");
    Wt::WContainerWidget *SearchDivLineEditDiv = ImageDiv->addWidget(std::make_unique<Wt::WContainerWidget>());

    Image = ImageDiv->addWidget(std::make_unique<Wt::WImage>(Wt::WLink(url.c_str())));
    Image->setAlternateText("Selected Digit Image");

    Wt::WContainerWidget *comboDiv = ColumnMiddleDiv->addWidget(std::make_unique<Wt::WContainerWidget>());
    comboDiv->setStyleClass("input-group");
    ModelComboBox = comboDiv->addWidget(std::make_unique<Wt::WComboBox>());
    ModelComboBox->addItem("Heavy");   // 'Heavy' (index 0) is shown by default.
    ModelComboBox->addItem("Medium");
    ModelComboBox->addItem("Light");
    //SetupFooter();
  }
}

// Event and button click handlers.
void ClientUiApplication::OnSearchButtonPressed()
{
  auto url(SearchLineEdit->text().toUTF8());
  // little sanity check on the URL.
  //std::string jsonRequst("{\"URL\":\"youtube.com\"}");
  std::string sessId(sessionId());
  //std::cout << "OnSearchButtonPressed\n";
  std::string cmd("curl --head --silent --fail " + url);
  int ret = ExecuteCommand(cmd);
  //std::cout << " command = " << cmd.c_str() << "\n ret = " << ret << "\n";
  if (ret == 0) {
    UrlParser parse(url);
    // Sanity check on the URL
    if (parse.IsValidUrl()) {
      std::cout << "Valid URL\n";
      root()->removeWidget(MainImageDiv);
      root()->removeWidget(FooterDiv);
      MainImageDiv = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
      SetupImageWindow(MainImageDiv, url);
      SetupFooter();
    }
  }
  else {
    Wt::WMessageBox::show("Information", "Please give Valid URL with .jpg or .jpeg or .png extension.", Wt::StandardButton::Ok);
  }

  /*
  if (url.find("https://www.youtube.com") != std::string::npos
      || url.find("http://www.youtube.com") != std::string::npos)  {
    std::ifstream infile(file);
    std::string line;
    while (std::getline(infile, line)) {
      if (!line.empty()) {
        VideoPlayer->show();
        VideoPlayer->clearSources();
        VideoPlayer->addSource(Wt::WLink(line));
        std::string action("Play");
        //SendVideoAnalysisRequest(action, line);
        break;
      }
    }
  }
  else {
    VideoPlayer->hide();
    Wt::WMessageBox::show("Information", "Please give me a digit Image URL.", Wt::StandardButton::Ok);
  }
  Wt::WMessageBox::show("Information", "Please give me a digit Image URL.", Wt::StandardButton::Ok);
  */
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
  SetupImageSearchBar(MainSearchDiv);
  std::string dummyUrl("");
  SetupImageWindow(MainImageDiv, dummyUrl);
  SetupFooter();
}

ClientUiApplication::~ClientUiApplication()
{
  // Empty;
}
