#include <CQPetBasic.h>
#include <CQPetBasicTerm.h>
#include <CQPetBasicApp.h>
#include <CPetBasic.h>
#include <CQUtil.h>

#include <QApplication>
#include <QEventLoop>
#include <QTabWidget>

CQPetBasic::
CQPetBasic(CQPetBasicApp *app) :
 app_(app)
{
  setReplaceEmbedded(true);
}

void
CQPetBasic::
resize(uint nr, uint nc)
{
  CPetBasic::resize(nr, nc);

  auto *term = app_->term();

  term->resize(nr, nc);
}

void
CQPetBasic::
setReverse(bool b)
{
  CPetBasic::setReverse(b);

  app_->updateInterface();
}

void
CQPetBasic::
setShift(bool b)
{
  CPetBasic::setShift(b);

  app_->updateInterface();
}

void
CQPetBasic::
notifyRunLine(uint n) const
{
  app_->setStatusMsg(QString("Line %1").arg(n));
}

void
CQPetBasic::
notifyLinesChanged()
{
  app_->notifyLinesChanged();
}

void
CQPetBasic::
notifyLineNumChanged()
{
  app_->notifyLineNumChanged();
}

void
CQPetBasic::
notifyVariablesChanged()
{
  app_->notifyVariablesChanged();
}
