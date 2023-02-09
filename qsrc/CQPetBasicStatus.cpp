#include <CQPetBasicStatus.h>
#include <CQPetBasicApp.h>
#include <CQPetBasic.h>

#include <CQUtil.h>

#include <QLabel>

CQPetBasicStatus::
CQPetBasicStatus(CQPetBasicApp *app) :
 QFrame(app), app_(app)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 2, 2);

  text_ = CQUtil::makeWidget<QLabel>("text");

  layout->addWidget(text_);

  auto *stopButton = CQUtil::makeLabelWidget<QPushButton>("Stop", "stop");
  stopButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  CQUtil::defConnect(stopButton, this, SLOT(stopSlot()));

  layout->addWidget(stopButton);
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
