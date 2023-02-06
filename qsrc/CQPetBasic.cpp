#include <CQPetBasic.h>
#include <CPetBasic.h>
#include <CQUtil.h>
#include <CUtf8.h>

#include <QApplication>
#include <QPainter>
#include <QEventLoop>
#include <QLabel>
#include <QTimer>
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

CQPetBasicApp::
CQPetBasicApp(QWidget *parent) :
 QWidget(parent)
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  term_ = new CQPetBasicTerm(this);

  layout->addWidget(term_);

  command_ = new CQPetBasicCommandScroll(this);

  command_->init();

  connect(command_, SIGNAL(executeCommand(const QString &)),
          this, SLOT(executeCommand(const QString &)));
  connect(command_, SIGNAL(keyPress(const QString &)),
          this, SLOT(keyPress(const QString &)));

  layout->addWidget(command_);

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
errorMsg(const QString &msg)
{
  std::cerr << msg.toStdString() << "\n";
}

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

//---

CQPetBasicTerm::
CQPetBasicTerm(CQPetBasicApp *app) :
 QWidget(app), app_(app)
{
  setObjectName("perTerm");

  auto font = QFont("Courier", 32);
  setFont(font);

  basic_ = new CQPetBasic(this);

  cursorTimer_ = new QTimer(this);

  CQUtil::defConnect(cursorTimer_, this, SLOT(cursorTimeout()));

  cursorTimer_->start(1000);

  resize(basic_->numRows(), basic_->numCols());
}

CQPetBasicTerm::
~CQPetBasicTerm()
{
  delete basic_;
}

void
CQPetBasicTerm::
resize(uint nr, uint nc)
{
  nr_ = nr;
  nc_ = nc;

  auto n = nr_*nc_;

  chars_.resize(n);

  setFixedSize(sizeHint());

  clear();
}

void
CQPetBasicTerm::
moveTo(uint r, uint c)
{
  r_ = r;
  c_ = c;
}

void
CQPetBasicTerm::
drawString(const std::string &str)
{
  int i   = 0;
  int len = str.length();

  while (i < len) {
    auto c = str[i];

    // handle new line
    if      (c == '\n') {
      ++r_;

      c_ = 0;

      if (r_ >= nr_)
        scrollUp();
    }

    // handle tab
    else if (c == '\t') {
      if (drawChar1(' '))
        cursorRight();

      while (c_ % 10 != 1) {
        if (drawChar1(' '))
          cursorRight();
        else
          break;
      }
    }

    // handle embedded escape codes (TODO: better encoding ?)
    else if (c == '\023') { // HOM
      home();
    }
    else if (c == '\223') { // CLS
      clear();
    }
    else if (c == '\022') { // REV
      reverse_ = true;
    }
    else if (c == '\222') { // OFF
      reverse_ = false;
    }
    else if (c == '\221') { // CU
      cursorUp();
    }
    else if (c == '\021') { // CD
      cursorDown();
    }
    else if (c == '\235') { // CL
      cursorLeft();
    }
    else if (c == '\035') { // CR
      cursorRight();
    }

    else {
      // map to upper case
      if (islower(c))
        c = toupper(c);

      if (drawChar1(c))
        cursorRight(/*nl*/false);
    }

    ++i;
  }

  update();
}

//---

uchar
CQPetBasicTerm::
getChar(uint r, uint c, ulong &utf, bool &reverse) const
{
  auto i = encodeCharPos(r, c);

  auto c1 = chars_[i].c;
  utf     = chars_[i].utf;
  reverse = chars_[i].reverse;

  return c1;
}

void
CQPetBasicTerm::
setChar(uint r, uint c, uchar value, ulong utf, bool reverse)
{
  auto i = encodeCharPos(r, c);

  chars_[i].c       = value;
  chars_[i].utf     = utf;
  chars_[i].reverse = reverse;

  update();
}

bool
CQPetBasicTerm::
drawChar1(const uchar &c)
{
//if (! isHandledChar(c))
//  app_->errorMsg(QString("Unhandled '%1'").arg(c));

  if (r_ >= nr_ || c_ >= nc_) return false;

  auto i = encodeCharPos(r_, c_);

  chars_[i] = CharData(c, reverse_);

  return true;
}

void
CQPetBasicTerm::
scrollUp()
{
  uint n = nr_*nc_;

  // move all chars left by nc_
  uint i1 = 0;

  for (uint i2 = nc_; i2 < n; ++i1, ++i2)
    chars_[i1] = chars_[i2];

  // fill last row with spaces
  for ( ; i1 < n; ++i1)
    chars_[i1] = CharData(uchar(' '), false);

  // update mouse pos to previous row
  if (r_ > 0)
    --r_;
}

//---

void
CQPetBasicTerm::
home()
{
  r_ = 0;
  c_ = 0;
}

void
CQPetBasicTerm::
clear()
{
  home();

  for (uint i = 0, r = 0; r < nr_; ++r)
    for (uint c = 0; c < nc_; ++c, ++i)
      setChar(r, c, ' ', 0, false);
}

void
CQPetBasicTerm::
cursorUp()
{
  if (r_ > 0)
    --r_;
}

void
CQPetBasicTerm::
cursorDown()
{
  if (r_ < nr_ - 1)
    ++r_;
}

void
CQPetBasicTerm::
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
CQPetBasicTerm::
cursorRight(bool nl)
{
  if (nl) {
    if      (c_ < nc_ - 1)
      ++c_;
    else if (r_ < nr_ - 1) {
      c_ = 0;
      ++r_;
    }
  }
  else
    ++c_;
}

void
CQPetBasicTerm::
cursorTimeout()
{
  cursorBlink_ = ! cursorBlink_;

  update();
}

void
CQPetBasicTerm::
paintEvent(QPaintEvent *)
{
  QFontMetrics fm(font());

  auto cw = fm.horizontalAdvance("X");
  auto ch = fm.height();

  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);

  QString str(" ");

  int y  = 0;
  int yt = fm.ascent();

  for (uint r = 0; r < nr_; ++r) {
    int x = 0;

    for (uint c = 0; c < nc_; ++c) {
      bool reverse; ulong utf;
      auto ic = getChar(r, c, utf, reverse);

      if (reverse) {
        painter.fillRect(QRect(x, y, cw, ch), Qt::white);

        painter.setPen(Qt::black);
      }
      else {
        painter.setPen(Qt::white);
      }

      if (r == r_ && c == c_) {
        if (cursorBlink_)
          painter.fillRect(QRect(x, y, cw, ch), Qt::yellow);
      }

      // Valley
      //  214 : Border
      //  219 : Safe Castle
      //   78 : Path up
      //   77 : Path down
      //  216 : Woods
      //  173 : Swamps
      //   87 : Tower
      //   81 : Character

      // Woods
      //   96 : Border
      //   88 : Trees
      //  224 : Lake
      //  230 : Vounims
      //   81 : Character

      // Swamps
      //   96 : Border
      //   45 : Tufts
      //  224 : Lake
      //  230 : Y Nagioth
      //   81 : Character

      // Tower
      //  160 : Border
      //  160 : Walls
      //  102 : Stairs
      //  104 : Doorway
      //   42 : Treasures
      //   81 : Character

#if 0
      if      (ic == 173) // SWAMPS: L SHAPE, MID-TOP TO MID-RIGHT
        painter.drawText(x, yt, "\u2517");
      else if (ic == 214) // BORDER: CROSS X
        painter.drawText(x, yt, "\u2715");
      else if (ic == 216) // WOODS: GRAVE CROSS
        painter.drawText(x, yt, "+");
      else if (ic == 219) // SAFE CASTLE: PLUS +
        painter.drawText(x, yt, "\u254B");
#endif

      if (utf > 0) {
        std::string s;
        CUtf8::append(s, utf);
        painter.drawText(x, yt, QString::fromStdString(s));
      }
      else {
        str[0] = ic;

        painter.drawText(x, yt, str);
      }

      x += cw;
    }

    y  += ch;
    yt += ch;
  }
}

QSize
CQPetBasicTerm::
sizeHint() const
{
  QFontMetrics fm(font());

  auto cw = fm.horizontalAdvance("X");
  auto ch = fm.height();

  return QSize(nc_*cw, nr_*ch);
}

//---

CQPetBasicCommandScroll::
CQPetBasicCommandScroll(CQPetBasicApp *app) :
 CQCommand::ScrollArea(app), app_(app)
{
}

CQCommand::CommandWidget *
CQPetBasicCommandScroll::
createCommandWidget() const
{
  auto *th = const_cast<CQPetBasicCommandScroll *>(this);

  th->command_ = new CQPetBasicCommandTerm(th);

  return command_;
}

//---

CQPetBasicCommandTerm::
CQPetBasicCommandTerm(CQPetBasicCommandScroll *scroll) :
 CQCommand::CommandWidget(scroll), scroll_(scroll)
{
}

//---

CQPetBasicStatus::
CQPetBasicStatus(CQPetBasicApp *app) :
 QFrame(app), app_(app)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 2, 2);

  text_ = new QLabel;

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
