#include <CPetBasicTerm.h>
#include <CPetBasicUtil.h>
#include <CPetBasic.h>
#include <CReadLine.h>
#include <CUtf8.h>

#include <COSPty.h>
#include <COSRead.h>
#include <COSTimer.h>
#include <CEscape.h>

#include <termios.h>
#include <unistd.h>

CPetBasicTerm::
CPetBasicTerm(CPetBasic *basic) :
 basic_(basic)
{
}

CPetBasicTerm::
~CPetBasicTerm()
{
}

void
CPetBasicTerm::
init()
{
  resize(basic_->numRows(), basic_->numCols());
}

//---

void
CPetBasicTerm::
resize(uint nr, uint nc)
{
  nr_ = nr;
  nc_ = nc;

  auto n = nr_*nc_;

  chars_.resize(n);

  clear();

  update();
}

//---

void
CPetBasicTerm::
loop()
{
  CReadLine readline;

  auto setPrompt = [&](bool ext=false) {
    readline.setPrompt(ext ? "pet+> " : "pet> ");
  };

  setPrompt();

  auto line = readline.readLine();

  while (! readline.eof()) {
    auto len = line.size();

    if (len > 0 && line[len - 1] == '\\') {
      setPrompt(true);

      inputBuffer_ += line.substr(0, len - 1);
    }
    else {
      inputBuffer_ += line;

      if (inputBuffer_ == "")
        break;

      (void) basic_->inputLine(inputBuffer_);

      inputBuffer_ = "";

      setPrompt();

      if (basic_->isStopped())
        break;
    }

    line = readline.readLine();
  }
}

std::string
CPetBasicTerm::
readString(const std::string &prompt) const
{
  CReadLine readline;

  readline.setPrompt(prompt != "" ? prompt + " ? " : "? ");

  return readline.readLine();
}

char
CPetBasicTerm::
readChar() const
{
  CReadLine readline;

  readline.setPrompt("? ");

  auto line = CPetBasicUtil::toUpper(readline.readLine());

  return (line.size() ? line[0] : '\0');
}

//---

void
CPetBasicTerm::
home()
{
  moveTo(0, 0);
}

void
CPetBasicTerm::
moveTo(uint r, uint c)
{
  r_ = r;
  c_ = c;
}

CPetDrawChar
CPetBasicTerm::
getChar(uint r, uint c) const
{
  auto i = encodeCharPos(r, c);

  return chars_[i];
}

void
CPetBasicTerm::
setChar(uint r, uint c, const CPetDrawChar &drawChar)
{
  auto i = encodeCharPos(r, c);

  chars_[i] = drawChar;

  update();
}

//---

void
CPetBasicTerm::
clear()
{
  home();

  for (uint r = 0; r < nr_; ++r) {
    for (uint c = 0; c < nc_; ++c) {
      auto i = encodeCharPos(r, c);

      chars_[i] = CPetDrawChar(' ');
    }
  }

  update();
}

void
CPetBasicTerm::
enter()
{
  cursorDown(/*force*/true);

  cursorLeftFull();

  if (row() >= int(numRows()))
    scrollUp();

  if (isTty())
    std::cout << "\n";
}

void
CPetBasicTerm::
tab()
{
  if (drawChar(' '))
    cursorRight();

  while (col() % 10 != 1) {
    if (drawChar(' '))
      cursorRight();
    else
      break;
  }
}

void
CPetBasicTerm::
cursorUp()
{
  if (r_ > 0)
    --r_;
}

void
CPetBasicTerm::
cursorDown(bool force)
{
  if (force || r_ < int(nr_) - 1) {
    ++r_;
  }
}

void
CPetBasicTerm::
cursorLeft()
{
  if      (c_ > 0)
    --c_;
  else if (r_ > 0) {
    c_ = nc_ - 1;
    --r_;
  }
}

void
CPetBasicTerm::
cursorRight(bool force)
{
  if (! force) {
    if      (c_ < int(nc_) - 1)
      ++c_;
    else if (r_ < int(nr_) - 1) {
      c_ = 0;
      ++r_;
    }
  }
  else
    ++c_;
}

void
CPetBasicTerm::
cursorLeftFull()
{
  c_ = 0;
}

void
CPetBasicTerm::
scrollUp()
{
  uint n = nr_*nc_;

  // move all chars left by nc_
  uint i1 = 0;

  for (uint i2 = nc_; i2 < n; ++i1, ++i2)
    chars_[i1] = chars_[i2];

  // fill last row with spaces
  for ( ; i1 < n; ++i1)
    chars_[i1] = CPetDrawChar(uchar(' '));

  // update mouse pos to previous row
  if (r_ > 0)
    --r_;

  update();
}

//---

void
CPetBasicTerm::
del()
{
  if (c_ > 0)
    setChar(r_, --c_, CPetDrawChar(' '));
}

//---

bool
CPetBasicTerm::
inQuotes() const
{
  // TODO: use buffer
  std::string str;

  int nq = 0;

  for (int c = 0; c < c_; ++c) {
    auto drawChar = getChar(r_, c);

    if (drawChar.c() == '"')
      ++nq;
  }

  return (nq & 1);
}

//---

bool
CPetBasicTerm::
drawChar(const uchar &c)
{
  if (r_ >= int(nr_) || c_ >= int(nc_)) return false;

  setChar(r_, c_, CPetDrawChar(c, 0, basic()->isReverse()));

  if (isTty())
    std::cout << c;

  return true;
}

bool
CPetBasicTerm::
drawChar(const CPetDrawChar &drawChar)
{
  if (r_ >= int(nr_) || c_ >= int(nc_)) return false;

  setChar(r_, c_, drawChar);

  if (isTty()) {
    if (drawChar.utf()) {
      std::string str;

      CUtf8::append(str, drawChar.utf());

      std::cout << str;
    }
    else {
      std::cout << drawChar.c();
    }
  }

  return true;
}

//---

bool
CPetBasicTerm::
drawPoint(long, long, long)
{
  return true;
}

//---

void
CPetBasicTerm::
enterLine()
{
  std::string str;

  int nq = 0;

  for (int c = 0; c < c_; ++c) {
    auto drawChar = getChar(r_, c);

    if (drawChar.c() == '"')
      ++nq;

    if (drawChar.utf() || drawChar.isReverse()) {
      if (nq & 1) {
        auto pet = CPetBasic::drawCharToPet(drawChar);

        uchar c1(pet.c());

        bool handled = true;

        switch (c1) {
          case 147: c1 =  19; break; // HOM
          case 211: c1 = 147; break; // CLS
          case 146: c1 =  18; break; // REV
          case 210: c1 = 146; break; // OFF
          case 145: c1 =  17; break; // CD
          case 209: c1 = 145; break; // CU
          case 157: c1 =  29; break; // CR
          case 221: c1 = 157; break; // CL
          default: handled = false; break;
        }

        if (! handled)
          c1 += 128;

        str += char(c1);
      }
      else
        CUtf8::append(str, drawChar.utf());
    }
    else
      str += drawChar.c();
  }

  cursorDown();
  cursorLeftFull();

  basic()->inputLine(str);
}

//---

void
CPetBasicTerm::
update()
{
}

void
CPetBasicTerm::
delay(long t)
{
  COSTimer::milli_sleep(uint(t));
}
