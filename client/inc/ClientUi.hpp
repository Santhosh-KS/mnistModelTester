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


#ifndef CLIENT_UI_WT_HPP
#define CLIENT_UI_WT_HPP


#include <Wt/WApplication.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WImage.h>
#include <Wt/WComboBox.h>
#include <memory>

class ClientUiApplication : public Wt::WApplication
{
private:
  Wt::WLineEdit *SearchLineEdit;
  Wt::WPushButton *SearchButton;
  Wt::WContainerWidget *MainSearchDiv;
  Wt::WContainerWidget *FooterDiv;
  Wt::WContainerWidget *MainImageDiv;
  Wt::WImage *Image;
  Wt::WComboBox *ModelComboBox;
  void SetupTheme();
  void SetupHeader();
  void SetupMainBody();
  void SetupImageSearchBar(Wt::WContainerWidget *mainLeft);
  void SetupFooter();
  void SetupImageWindow(Wt::WContainerWidget *mainImgDiv, std::string &url);
  void OnSearchButtonPressed();
  bool CheckFileExists(std::string &file);
  int ExecuteCommand(std::string &cmd);
public:
  ClientUiApplication(const Wt::WEnvironment& env);
  ~ClientUiApplication();
};

#endif // CLIENT_UI_WT_HPP
