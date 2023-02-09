#include <CQPetBasic.h>
#include <CQPetBasicTerm.h>
#include <CQPetBasicApp.h>
#include <CPetBasic.h>
#include <CQUtil.h>

#include <QApplication>
#include <QEventLoop>
#include <QTabWidget>
#include <QTime>

#if 0
namespace {

bool isHandledChar(uchar c) {
  if (c >= 32 && c <= 127) return true;

  if (c == 0  ) return true;
  if (c == 173) return true;
  if (c == 214) return true;
  if (c == 216) return true;
  if (c == 219) return true;

  return false;
}

}
#endif

//---

CQPetBasic::
CQPetBasic(CQPetBasicTerm *term) :
 term_(term)
{
  setReplaceEmbedded(true);
}

void
CQPetBasic::
printString(const std::string &s) const
{
//std::cerr << CPetBasic::decodeEmbeddedStr(s) << "\n";
  term_->drawString(s);
}

char
CQPetBasic::
getChar() const
{
  auto c = term_->app()->getChar();
  if (! c) return '\0';

  c = std::toupper(c);

  return c;
}

std::string
CQPetBasic::
getString(const std::string &prompt) const
{
  printString(prompt + "? ");

  auto str = term_->app()->getString();

  str = str.toUpper();

  return str.toStdString();
}

void
CQPetBasic::
resize(uint nr, uint nc)
{
  CPetBasic::resize(nr, nc);

  term_->resize(nr, nc);
}

bool
CQPetBasic::
getScreenMemory(uint r, uint c, uchar &petsci) const
{
  // screen memory is ascii, need to return petsci
  bool reverse; ulong utf;
  auto ascii = term_->getChar(r, c, utf, reverse);

  petsci = asciiToPet(ascii, utf, reverse);

  return true;
}

void
CQPetBasic::
setScreenMemory(uint r, uint c, uchar petsci)
{
  // value is in petsci, screen memory is ascii
  bool reverse; ulong utf;
  auto ascii = petToAscii(petsci, utf, reverse);

//if (! isHandledChar(value))
//  term_->app()->errorMsg(QString("Unhandled '%1'").arg(value));

  term_->setChar(r, c, ascii, utf, reverse);
}

void
CQPetBasic::
delay()
{
  auto dieTime = QTime::currentTime().addMSecs(10);

  while (QTime::currentTime() < dieTime)
    qApp->processEvents(QEventLoop::AllEvents, 10);
}

void
CQPetBasic::
notifyRunLine(uint n) const
{
  term_->app()->setStatusMsg(QString("Line %1").arg(n));
}

void
CQPetBasic::
notifyLinesChanged()
{
  term_->app()->notifyLinesChanged();
}

void
CQPetBasic::
notifyLineNumChanged()
{
  term_->app()->notifyLineNumChanged();
}
