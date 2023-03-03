#include <CPetBasicRawTerm.h>
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

CPetBasicRawTerm::
CPetBasicRawTerm(CPetBasic *basic) :
 CPetBasicTerm(basic)
{
}

CPetBasicRawTerm::
~CPetBasicRawTerm()
{
}

void
CPetBasicRawTerm::
init()
{
  setRaw(true);

  CPetBasicTerm::init();
}

void
CPetBasicRawTerm::
setRaw(bool b)
{
  if (b) {
    if (! raw_) {
      delete ios_;

      ios_ = new struct termios;

      COSPty::set_raw(STDOUT_FILENO, ios_);

      // alt screen ?
      COSRead::write(STDOUT_FILENO, CEscape::DECSET(1049));

      // show/blink cursor (hide ? DECRST)
      COSRead::write(STDOUT_FILENO, CEscape::DECSET(25));
      COSRead::write(STDOUT_FILENO, CEscape::DECSET(12));

      (void) CEscape::getWindowCharSize(&screenRows_, &screenCols_);

      raw_ = true;
    }
  }
  else {
    if (raw_) {
      if (tcsetattr(STDOUT_FILENO, TCSAFLUSH, ios_) < 0) {
        std::cerr << "Set RAW false failed\n";
        return;
      }

      COSRead::write(STDOUT_FILENO, CEscape::DECRST(1049));

      // show/blink cursor
      COSRead::write(STDOUT_FILENO, CEscape::DECSET(25));
      COSRead::write(STDOUT_FILENO, CEscape::DECSET(12));

      COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

      delete ios_;

      ios_ = nullptr;

      raw_ = false;
    }
  }
}

//---

void
CPetBasicRawTerm::
loop()
{
  if (! isRaw())
    return CPetBasicTerm::loop();

  //std::cerr << "raw loop\n";

  auto *th = const_cast<CPetBasicRawTerm *>(this);

  home();

  while (true) {
    state_ = State::LOOP;

    if (! COSRead::wait_read(STDIN_FILENO, 0, 100)) continue;

    std::string buffer;

    if (! COSRead::read(STDIN_FILENO, buffer)) continue;

    uint len = uint(buffer.size());
    if (len == 0) continue;

    for (auto &c : buffer) {
      drawChar(c);

      cursorRight();
    }

    //std::cerr << buffer << " : " << int(buffer[len - 1]) << "\n";

    uint pos = 0;

    while (pos < len) {
      if      (buffer[pos] == '\\')
        ++pos;
      else if (buffer[pos] == 13) {
        enter();

        //std::cerr << "inputLine: " << th->inputBuffer_ << "\n";

        (void) basic_->inputLine(th->inputBuffer_);

        th->inputBuffer_ = "";

        break;
      }
      else if (buffer[pos] == 3) { // Ctrl+C
        basic_->setStopped(true);
        break;
      }
      else
        th->inputBuffer_ += buffer[pos++];
    }

    if (basic_->isStopped())
      break;
  }

  state_ = State::NONE;

  setRaw(false);
}

std::string
CPetBasicRawTerm::
readString(const std::string &prompt) const
{
  if (! isRaw())
    return CPetBasicTerm::readString(prompt);

  // output prompt
  basic_->printString(prompt + "? ");

  auto *th = const_cast<CPetBasicRawTerm *>(this);

  while (true) {
    th->state_ = State::READ_STRING;

    if (! COSRead::wait_read(STDIN_FILENO, 0, 100)) continue;

    std::string buffer;

    if (! COSRead::read(STDIN_FILENO, buffer)) continue;

    uint len = uint(buffer.size());
    if (len == 0) continue;

    for (auto &c : buffer) {
      th->drawChar(c);

      th->cursorRight();
    }

    uint pos = 0;

    while (pos < len) {
      if      (buffer[pos] == '\\')
        ++pos;
      else if (buffer[pos] == 13) {
        auto buffer1 = th->inputBuffer_;

        th->inputBuffer_ = buffer.substr(pos + 1);

        th->state_ = State::NONE;

        return buffer1;
      }
      else if (buffer[pos] == 3) { // Ctrl+C
        basic_->setStopped(true);
        break;
      }
      else
        th->inputBuffer_ += buffer[pos++];
    }

    if (basic_->isStopped())
      break;
  }

  th->state_ = State::NONE;

  return "";
}

char
CPetBasicRawTerm::
readChar() const
{
  if (! isRaw())
    return CPetBasicTerm::readChar();

  auto *th = const_cast<CPetBasicRawTerm *>(this);

  //---

  th->state_ = State::READ_CHAR;

  char c = '\0';

  for (uint i = 0; i < 10; ++i) {
    // read pending key input
    if (! COSRead::wait_read(STDIN_FILENO, 0, 100)) {
      COSTimer::milli_sleep(uint(100));
      continue;
    }

    std::string buffer;

    if (! COSRead::read(STDIN_FILENO, buffer)) {
      COSTimer::milli_sleep(uint(100));
      continue;
    }

    uint len = uint(buffer.size());
    if (len == 0) {
      COSTimer::milli_sleep(uint(100));
      continue;
    }

    //---

    // add to buffer
    th->inputBuffer_ += buffer;

    break;
  }

  // pull char from buffer if any
  if (th->inputBuffer_.size()) {
    c = th->inputBuffer_[0];

    th->inputBuffer_ = th->inputBuffer_.substr(1);
  }

  th->state_ = State::NONE;

  // stop on control C
  if (c == 3) {
    basic_->setStopped(true);
    return 0;
  }

  // fix case
  c = char(std::toupper(c));

  return c;
}

//---

void
CPetBasicRawTerm::
moveTo(uint r, uint c)
{
  CPetBasicTerm::moveTo(r, c);

  if (isRaw())
    COSRead::write(STDOUT_FILENO, CEscape::CUP(r_ + 1, c_ + 1));
}

//---

void
CPetBasicRawTerm::
clear()
{
  CPetBasicTerm::clear();

  if (isRaw())
    COSRead::write(STDOUT_FILENO, CEscape::ED(2)); // all
}

void
CPetBasicRawTerm::
enter()
{
  cursorDown(/*force*/true);

  cursorLeftFull();

  if (row() >= int(numRows()))
    scrollUp();

  if (! isRaw())
    std::cout << "\n";
}

void
CPetBasicRawTerm::
cursorUp()
{
  CPetBasicTerm::cursorUp();

  if (r_ > 0)
    --r_;

  if (isRaw())
    COSRead::write(STDOUT_FILENO, CEscape::CUU());
}

void
CPetBasicRawTerm::
cursorDown(bool force)
{
  CPetBasicTerm::cursorDown(force);

  if (isRaw())
    COSRead::write(STDOUT_FILENO, CEscape::CUD());
}

void
CPetBasicRawTerm::
cursorLeft()
{
  CPetBasicTerm::cursorLeft();

  if (isRaw())
    COSRead::write(STDOUT_FILENO, CEscape::CUB());
}

void
CPetBasicRawTerm::
cursorRight(bool force)
{
  CPetBasicTerm::cursorRight(force);

  if (isRaw())
    COSRead::write(STDOUT_FILENO, CEscape::CUF());
}

void
CPetBasicRawTerm::
cursorLeftFull()
{
  CPetBasicTerm::cursorLeftFull();

  if (isRaw())
    COSRead::write(STDOUT_FILENO, CEscape::CHA());
}

//---

bool
CPetBasicRawTerm::
drawChar(const uchar &c)
{
  if (! CPetBasicTerm::drawChar(c))
    return false;

  if (isRaw()) {
    COSRead::write(STDOUT_FILENO, CEscape::CUP(r_ + 1, c_ + 1));
    COSRead::write(STDOUT_FILENO, c);
  }
  else
    std::cout << c;

  return true;
}

bool
CPetBasicRawTerm::
drawChar(const CPetDrawChar &drawChar)
{
  if (r_ >= int(nr_) || c_ >= int(nc_)) return false;

  setChar(r_, c_, drawChar);

  if (isRaw())
    COSRead::write(STDOUT_FILENO, CEscape::CUP(r_ + 1, c_ + 1));

  if (drawChar.utf()) {
    std::string str;

    CUtf8::append(str, drawChar.utf());

    if (isRaw())
      COSRead::write(STDOUT_FILENO, str);
    else
      std::cout << str;
  }
  else {
    if (isRaw())
      COSRead::write(STDOUT_FILENO, drawChar.c());
    else
      std::cout << drawChar.c();
  }

  update();

  return true;
}

//---

void
CPetBasicRawTerm::
update()
{
  if (isRaw()) {
    ++redrawCount_;

    COSRead::write(STDOUT_FILENO, CEscape::ED(2)); // all

    for (uint r = 0; r < nr_; ++r) {
      for (uint c = 0; c < nc_; ++c) {
        bool isCursor = (int(r) == r_ && int(c) == c_);

        if (isCursor)
          COSRead::write(STDOUT_FILENO, CEscape::SGR(43));

        auto drawChar = getChar(r, c);

        if ((drawChar.isSet() && drawChar.c() != ' ') || isCursor) {
          COSRead::write(STDOUT_FILENO, CEscape::CUP(r + 1, c + 1));

          if (drawChar.utf()) {
            std::string str;

            CUtf8::append(str, drawChar.utf());

            COSRead::write(STDOUT_FILENO, str);
          }
          else {
            COSRead::write(STDOUT_FILENO, drawChar.c());
          }
        }

        if (isCursor)
          COSRead::write(STDOUT_FILENO, CEscape::SGR(0));
      }
    }

    //---

    COSRead::write(STDOUT_FILENO, CEscape::CUP(screenRows_, 1));

    std::string status;

    status += "R:" + std::to_string(r_) + " C:" + std::to_string(c_);
    status += " DRAW: " + std::to_string(redrawCount_);
    status += " BUF: '" + inputBuffer_ + "'";

    auto stateStr = [&]() -> std::string {
      switch (state_) {
        case State::LOOP: return "Loop";
        case State::READ_STRING: return "ReadStr";
        case State::READ_CHAR: return "ReadChar";
        default: return "NONE";
      }
    };

    status += " DELAY: " + std::to_string(delay_);

    status += " STATE: " + stateStr();

    COSRead::write(STDOUT_FILENO, status);

    //---

    COSRead::write(STDOUT_FILENO, CEscape::CUP(r_ + 1, c_ + 1));
  }
}

void
CPetBasicRawTerm::
delay(long t)
{
  delay_ = t;

  while (delay_ > 0) {
    COSTimer::milli_sleep(uint(1));

    update();

    --delay_;
  }
}
