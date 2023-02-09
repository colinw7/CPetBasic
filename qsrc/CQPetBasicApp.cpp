#include <CQPetBasicApp.h>
#include <CQPetBasicTerm.h>
#include <CQPetBasicDbg.h>
#include <CQPetBasicVariables.h>
#include <CQPetBasicKeyboard.h>
#include <CQPetBasicCommand.h>
#include <CQPetBasicStatus.h>
#include <CQPetBasic.h>

#include <CQUtil.h>

#include <QApplication>
#include <QEventLoop>

CQPetBasicApp::
CQPetBasicApp(QWidget *parent) :
 QWidget(parent)
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //---

  auto *topFrame  = CQUtil::makeWidget<QFrame>(this, "topFrame");
  auto *topLayout = CQUtil::makeLayout<QHBoxLayout>(topFrame, 2, 2);

  layout->addWidget(topFrame);

  //---

  // add pet output terminal
  term_ = new CQPetBasicTerm(this);

  topLayout->addWidget(term_);

  //---

  // add debug window
  dbg_ = new CQPetBasicDbg(term_->basic());

  topLayout->addWidget(dbg_);

  variables_ = new CQPetBasicVariables(this);

  topLayout->addWidget(variables_);

  //---

  auto *tab = CQUtil::makeWidget<QTabWidget>(this, "tab");

  layout->addWidget(tab);

  //---

  // add keyword
  keyboard_ = new CQPetBasicKeyboard(this);

  tab->addTab(keyboard_, "Keyboard");

  //---

  // add console
  command_ = new CQPetBasicCommandScroll(this);

  command_->init();

  connect(command_, SIGNAL(executeCommand(const QString &)),
          this, SLOT(executeCommand(const QString &)));
  connect(command_, SIGNAL(keyPress(const QString &)),
          this, SLOT(keyPress(const QString &)));

  tab->addTab(command_, "Console");

  //---

  // add status
  status_ = new CQPetBasicStatus(this);

  layout->addWidget(status_);
}

CQPetBasic *
CQPetBasicApp::
basic() const
{
  return term_->basic();
}

void
CQPetBasicApp::
executeCommand(const QString &cmd)
{
  if (looping_) {
    looping_  = false;

    if (! loopChar_) {
      loopStr_ = cmd;
      loopC_   = '\0';
    }
    else {
      loopStr_ = "";
      loopC_   = (cmd.length() ? cmd[0].toLatin1() : '\0');

      loopChar_ = false;
    }

    loop_->exit(0);
  }
  else {
    auto *basic = this->basic();

    basic->inputLine(cmd.toStdString());
  }
}

void
CQPetBasicApp::
keyPress(const QString &str)
{
  if (looping_ & loopChar_) {
    command_->command()->clearEntry();

    looping_  = false;
    loopStr_  = "";
    loopC_    = (str.length() ? str[0].toLatin1() : '\0');

    loopChar_ = false;

    loop_->exit(0);
  }
}

QString
CQPetBasicApp::
getString()
{
  if (! loop_)
    loop_ = new QEventLoop;

  looping_  = true;
  loopChar_ = false;

  loop_->exec();

  return loopStr_;
}

uchar
CQPetBasicApp::
getChar()
{
  if (! loop_)
    loop_ = new QEventLoop;

  looping_  = true;
  loopChar_ = true;

  loop_->exec();

  return loopC_;
}

void
CQPetBasicApp::
setStatusMsg(const QString &msg)
{
  status_->setText(msg);

  qApp->processEvents();
}

void
CQPetBasicApp::
notifyLinesChanged()
{
  dbg_->file()->update();
}

void
CQPetBasicApp::
notifyLineNumChanged()
{
  dbg_->file()->update();
}

void
CQPetBasicApp::
errorMsg(const QString &msg)
{
  std::cerr << msg.toStdString() << "\n";
}
