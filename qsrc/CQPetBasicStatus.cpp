#include <CQPetBasicStatus.h>
#include <CQPetBasicApp.h>
#include <CQPetBasic.h>

#include <CQPixmapButton.h>
#include <CQUtil.h>

#include <QLabel>

#include <svg/keyboard_svg.h>
#include <svg/console_svg.h>
#include <svg/debug_svg.h>

CQPetBasicStatus::
CQPetBasicStatus(CQPetBasicApp *app) :
 QFrame(app), app_(app)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 2, 2);

  //---

  text_ = CQUtil::makeWidget<QLabel>("text");

  layout->addWidget(text_);

  //---

  auto is = QSize(64, 32);

  //---

  auto *keyboardButton = new CQPixmapButton(CQPixmapCacheInst->getIcon("KEYBOARD"), is);

  CQUtil::defConnect(dynamic_cast<QToolButton*>(keyboardButton), this, SLOT(keyboardSlot()));

  layout->addWidget(keyboardButton);

  //---

  auto *consoleButton = new CQPixmapButton(CQPixmapCacheInst->getIcon("CONSOLE"), is);

  CQUtil::defConnect(dynamic_cast<QToolButton*>(consoleButton), this, SLOT(consoleSlot()));

  layout->addWidget(consoleButton);

  //---

  auto *debugButton = new CQPixmapButton(CQPixmapCacheInst->getIcon("DEBUG"), is);

  CQUtil::defConnect(dynamic_cast<QToolButton*>(debugButton), this, SLOT(debugSlot()));

  layout->addWidget(debugButton);

  //---

  auto *stopButton = CQUtil::makeLabelWidget<QPushButton>("Stop", "stop");
  stopButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  layout->addWidget(stopButton);

  CQUtil::defConnect(stopButton, this, SLOT(stopSlot()));
}

void
CQPetBasicStatus::
setText(const QString &text)
{
  text_->setText(text);
}

void
CQPetBasicStatus::
stopSlot()
{
  app_->basic()->setStopped(true);
}

void
CQPetBasicStatus::
keyboardSlot()
{
  app_->setKeyboardVisible(! app_->isKeyboardVisible());
}

void
CQPetBasicStatus::
consoleSlot()
{
  app_->setConsoleVisible(! app_->isConsoleVisible());
}

void
CQPetBasicStatus::
debugSlot()
{
  app_->setDebugVisible(! app_->isDebugVisible());
}
