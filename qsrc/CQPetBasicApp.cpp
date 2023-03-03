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

  // create basic class
  basic_ = new CQPetBasic(this);

  //---

  // add pet output terminal
  auto *termFrame  = CQUtil::makeWidget<QFrame>(this, "topFrame");
  auto *termLayout = CQUtil::makeLayout<QVBoxLayout>(termFrame, 2, 2);

  topLayout->addWidget(termFrame);

  term_ = new CQPetBasicTerm(this);

  term_->init();

  basic_->setTerm(term_);

  termLayout->addWidget(term_);
  termLayout->addStretch(1);

  //---

  debugTab_ = CQUtil::makeWidget<QTabWidget>(this, "debugTab");

  topLayout->addWidget(debugTab_);

  // add debug window
  dbg_ = new CQPetBasicDbg(term_->basic());

  debugTab_->addTab(dbg_, "Source");

  variables_ = new CQPetBasicVariables(this);

  debugTab_->addTab(variables_, "Variables");

  debugTab_->setVisible(false);

  //---

  // add keyword
  keyboard_ = new CQPetBasicKeyboard(this);

  layout->addWidget(keyboard_);

  keyboard_->setVisible(false);

  //---

  // add console
  command_ = new CQPetBasicCommandScroll(this);

  command_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  command_->init();

  layout->addWidget(command_);

  command_->setVisible(false);

  //---

  // add status
  status_ = new CQPetBasicStatus(this);

  layout->addWidget(status_);
}

bool
CQPetBasicApp::
isKeyboardVisible() const
{
  return keyboard_->isVisible();
}

void
CQPetBasicApp::
setKeyboardVisible(bool b)
{
  keyboard_->setVisible(b);
}

bool
CQPetBasicApp::
isConsoleVisible() const
{
  return command_->isVisible();
}

void
CQPetBasicApp::
setConsoleVisible(bool b)
{
  command_->setVisible(b);
}

bool
CQPetBasicApp::
isDebugVisible() const
{
  return debugTab_->isVisible();
}

void
CQPetBasicApp::
setDebugVisible(bool b)
{
  debugTab_->setVisible(b);
}

void
CQPetBasicApp::
setStatusMsg(const QString & /*msg*/)
{
  //status_->setText(msg);

  qApp->processEvents();
}

void
CQPetBasicApp::
updateInterface()
{
  // called when reverse changed
  keyboard_->update();
}

void
CQPetBasicApp::
notifyLinesChanged()
{
  dbg_->file()->requestUpdate();
}

void
CQPetBasicApp::
notifyLineNumChanged()
{
  dbg_->scrollVisible();

  dbg_->file()->requestUpdate();
}

void
CQPetBasicApp::
notifyVariablesChanged()
{
  variables_->reload();
}

void
CQPetBasicApp::
errorMsg(const QString &msg)
{
  std::cerr << msg.toStdString() << "\n";
}
