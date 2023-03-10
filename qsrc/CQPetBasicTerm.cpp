#include <CQPetBasicTerm.h>
#include <CQPetBasicApp.h>
#include <CQPetBasic.h>
#include <CPetBasicUtil.h>

#include <CQUtil.h>
#include <CUtf8.h>

#include <QApplication>
#include <QTimer>
#include <QEventLoop>
#include <QMouseEvent>
#include <QPainter>
#include <QTime>

CQPetBasicTerm::
CQPetBasicTerm(CQPetBasicApp *app) :
 QWidget(app), CPetBasicTerm(app->basic()), app_(app)
{
}

CQPetBasicTerm::
~CQPetBasicTerm()
{
}

void
CQPetBasicTerm::
init()
{
  setObjectName("term");

#if 0
  auto font = qApp->font();
  font.setPointSizeF(32);
#else
  auto font = QFont("Courier", 32);
#endif
  setFont(font);

  setFocusPolicy(Qt::StrongFocus);

  //---

  // cursor blink timer
  cursorTimer_ = new QTimer(this);

  CQUtil::defConnect(cursorTimer_, this, SLOT(cursorTimeout()));

  cursorTimer_->start(1000);

  //---

  // cursor blink timer
  updateTimer_ = new QTimer(this);

  CQUtil::defConnect(updateTimer_, this, SLOT(updateTimeout()));

  updateTimer_->start(250);

  //---

  // loop timer
  loopData_.loopTimer = new QTimer(this);
  loopData_.loopTimer->setSingleShot(true);

  CQUtil::defConnect(loopData_.loopTimer, this, SLOT(loopTimeout()));
}

CQPetBasic *
CQPetBasicTerm::
basic() const
{
  return app_->basic();
}

void
CQPetBasicTerm::
resize(uint nr, uint nc)
{
  CPetBasicTerm::resize(nr, nc);

  setFixedSize(sizeHint());
}

//---

char
CQPetBasicTerm::
readChar() const
{
  auto *th = const_cast<CQPetBasicTerm *>(this);

  if (! th->loopData_.eventLoop)
    th->loopData_.eventLoop = new QEventLoop;

  // wait for key to be pressed (with timeout)
  th->loopData_.looping  = true;
  th->loopData_.loopChar = true;
  th->loopData_.loopC    = '\0';

  loopData_.loopTimer->start(250);

  loopData_.eventLoop->exec();

  loopData_.loopTimer->stop();

  // loopC contains read char
  auto c = loopData_.loopC;
  if (! c) return '\0';

  // map to upper case
  c = std::toupper(c);

  return c;
}

std::string
CQPetBasicTerm::
readString(const std::string &prompt) const
{
  auto *th = const_cast<CQPetBasicTerm *>(this);

  // output prompt
  basic_->printString(prompt + "? ");

  if (! th->loopData_.eventLoop)
    th->loopData_.eventLoop = new QEventLoop;

  // wait for string to be entered (no timeout)
  th->loopData_.looping  = true;
  th->loopData_.loopChar = false;
  th->loopData_.loopStr  = "";

  loopData_.eventLoop->exec();

  // loopStr contains read string
  auto str = th->loopData_.loopStr;

  // map to upper case
  str = CPetBasicUtil::toUpper(str);

  return str;
}

void
CQPetBasicTerm::
enterLoopStr(const std::string &str)
{
  loopData_.looping = false;

  inputBuffer_ += str;

  if (isLoopChar()) {
    char c = '\0';

    if (! inputBuffer_.empty()) {
      c = inputBuffer_[0];

      inputBuffer_ = inputBuffer_.substr(1);
    }

    loopData_.loopC    = c;
    loopData_.loopChar = false;
  }
  else {
    loopData_.loopStr = inputBuffer_;

    inputBuffer_ = "";
  }

  loopData_.eventLoop->exit(0);
}

void
CQPetBasicTerm::
addLoopStr(const std::string &str)
{
  inputBuffer_ += str;
}

void
CQPetBasicTerm::
loopTimeout()
{
  if (isLooping()) {
    loopData_.looping = false;

    loopData_.eventLoop->exit(0);
  }
}

//---

#if 0
void
CQPetBasicTerm::
drawString(const std::string &str)
{
  // ASCII with embedded escape codes for special chars
  int i   = 0;
  int len = str.length();

  //---

  while (i < len) {
    auto c  = str[i];
    auto c1 = uchar(c);

    // handle new line
    if      (c == '\n') {
      cursorDown(/*force*/true);

      cursorLeftFull();

      if (row() >= numRows())
        scrollUp();
    }

    // handle tab
    else if (c == '\t') {
      if (drawChar(' '))
        cursorRight();

      while (c_ % 10 != 1) {
        if (drawChar(' '))
          cursorRight();
        else
          break;
      }
    }

    else {
      // handle embedded escape codes
      bool handled = true;

      switch (c1) {
        case  19: home(); break;
        case 147: clear(); break;
        case 255: basic()->setStopped(true); break;
        case  18: basic()->setReverse(true); break;
        case 146: basic()->setReverse(false); break;
        case 145: cursorUp(); break;
        case  17: cursorDown(); break;
        case 157: cursorLeft(); break;
        case  29: cursorRight(); break;
        default:
          handled = false;
          break;
      }

      if (! handled && c1 >= 128) {
        c1 -= 128;

        CPetDrawChar drawChar(c1);

        auto pet = CPetBasic::drawCharToPet(drawChar);

        pet.shift();

        drawChar = CPetBasic::petToDrawChar(pet);

        drawChar.setReverse(basic()->isReverse());

        setChar(row(), c_, drawChar);

        cursorRight(/*force*/true);

        handled = true;
      }

      // handle normal char
      if (! handled) {
        // map to upper case
        if (islower(c))
          c = toupper(c);

        if (drawChar(c))
          cursorRight(/*force*/true);
      }
    }

    ++i;
  }

  update();
}
#endif

//---

bool
CQPetBasicTerm::
drawPoint(long x, long y, long c)
{
  if (image_)
    image_->setPixel(x, y, qRgba(c, c, c, 255));

  needsUpdate_ = true;

  return true;
}

//---

void
CQPetBasicTerm::
update()
{
  QWidget::update();
}

void
CQPetBasicTerm::
delay(long t)
{
  auto dieTime = QTime::currentTime().addMSecs(4*t);

  while (QTime::currentTime() < dieTime)
    qApp->processEvents(QEventLoop::AllEvents, 10);
}

//---

void
CQPetBasicTerm::
cursorTimeout()
{
  cursorBlink_ = ! cursorBlink_;

  updateTimeout();
}

void
CQPetBasicTerm::
updateTimeout()
{
  update();

  needsUpdate_ = false;
}

//---

void
CQPetBasicTerm::
keyPressEvent(QKeyEvent *e)
{
  auto key = e->key();
  auto mod = e->modifiers();

  if (e->key() == Qt::Key_Shift) {
    basic()->setShift(true);
    return;
  }

  bool isShift = basic()->isShift();
  bool isCtrl  = (mod & Qt::ControlModifier);

  auto sendChar = [&](uchar c) {
    CPetDrawChar drawChar(c, 0, basic()->isReverse());

    if (isShift) {
      auto pet = CPetBasic::drawCharToPet(drawChar);

      pet.shift();

      drawChar = CPetBasic::petToDrawChar(pet);
    }

    if (isLooping()) {
      auto c1 = char(c);
      auto s  = std::string(&c1, 1);

      if (isLoopChar()) {
        enterLoopStr(s);
        return;
      }

      addLoopStr(s);
    }

    setChar(row(), col(), drawChar);
    cursorRight();
    update();
  };

  if (key >= Qt::Key_A && key <= Qt::Key_Z) {
    auto c = uchar('A' + (key - Qt::Key_A));

    return sendChar(c);
  }

  isShift = isCtrl;

  if (key >= Qt::Key_0 && key <= Qt::Key_9) {
    auto c = uchar('0' + (key - Qt::Key_0));

    return sendChar(c);
  }

  switch (key) {
    case Qt::Key_Exclam:       return sendChar('!');
    case Qt::Key_QuoteDbl:     return sendChar('"');
    case Qt::Key_NumberSign:   return sendChar('#');
    case Qt::Key_Dollar:       return sendChar('$');
    case Qt::Key_Percent:      return sendChar('%');
    case Qt::Key_Apostrophe:   return sendChar('\'');
    case Qt::Key_Ampersand:    return sendChar('&');
    case Qt::Key_Backslash:    return sendChar('\\');
    case Qt::Key_ParenLeft:    return sendChar('(');
    case Qt::Key_ParenRight:   return sendChar(')');
    case Qt::Key_AsciiTilde:   return sendChar('~'); // left arrow
    case Qt::Key_AsciiCircum:  return sendChar('^'); // up arrow
    case Qt::Key_Colon:        return sendChar(':');
    case Qt::Key_Comma:        return sendChar(',');
    case Qt::Key_Semicolon:    return sendChar(';');
    case Qt::Key_Question:     return sendChar('?');
    case Qt::Key_At:           return sendChar('@');
    case Qt::Key_BracketLeft:  return sendChar('[');
    case Qt::Key_BracketRight: return sendChar(']');
    case Qt::Key_Space:        return sendChar(' ');
    case Qt::Key_Less:         return sendChar('<');
    case Qt::Key_Greater:      return sendChar('>');
    case Qt::Key_Slash:        return sendChar('/');
    case Qt::Key_Asterisk:     return sendChar('*');
    case Qt::Key_Plus:         return sendChar('+');
    case Qt::Key_Period:       return sendChar('.');
    case Qt::Key_Minus:        return sendChar('-');
    case Qt::Key_Equal:        return sendChar('=');

    case Qt::Key_Backspace: { del(); update(); return; }

    case Qt::Key_Return:
    case Qt::Key_Enter: {
      if (isLooping())
        enterLoopStr("");
      else {
        enterLine(); update();
      }
      return;
    }

    case Qt::Key_Home: {
      if (! inQuotes()) {
        home();
      }
      else {
        CPetsciChar pet1(! isShift ? 147 : 211);

        auto drawChar1 = CPetBasic::petToDrawChar(pet1);
        setChar(row(), col(), drawChar1);
        cursorRight();
      }
      update();
      return;
    }

    case Qt::Key_Left:  { cursorLeft (); update(); return; }
    case Qt::Key_Up:    { cursorUp   (); update(); return; }
    case Qt::Key_Right: { cursorRight(); update(); return; }
    case Qt::Key_Down:  { cursorDown (); update(); return; }
  }
}

void
CQPetBasicTerm::
keyReleaseEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Shift)
    basic()->setShift(false);
}

void
CQPetBasicTerm::
resizeEvent(QResizeEvent *)
{
  delete image_;

  image_ = new QImage(width(), height(), QImage::Format_ARGB32);

  image_->fill(QColor(0, 0, 0, 0).rgba());
}

void
CQPetBasicTerm::
paintEvent(QPaintEvent *)
{
  QFontMetricsF fm(font());

  cw_ = fm.horizontalAdvance("X");
  ch_ = fm.height();
  ca_ = fm.ascent();

  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);

  //---

  if (image_)
    painter.drawImage(0, 0, *image_);

  //---

  double y = 0;

  for (uint r = 0; r < nr_; ++r) {
    double x = 0;

    for (uint c = 0; c < nc_; ++c) {
      auto drawChar = getChar(r, c);

      if (drawChar.isReverse()) {
        painter.fillRect(QRect(x, y, cw_, ch_), Qt::white);

        painter.setPen(Qt::black);
      }
      else {
        painter.setPen(Qt::white);
      }

      if (int(r) == r_ && int(c) == c_) {
        if (cursorBlink_)
          painter.fillRect(QRect(x, y, cw_, ch_), Qt::yellow);
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

      if (drawChar.utf() > 0)
        paintUtfChar(&painter, x, y, drawChar.utf());
      else
        paintChar(&painter, x, y, drawChar.c());

      x += cw_;
    }

    y += ch_;
  }
}

void
CQPetBasicTerm::
paintChar(QPainter *painter, double x, double y, uchar c) const
{
  static QString str(" ");
  str[0] = c;
  painter->drawText(x, y + ca_, str);
}

void
CQPetBasicTerm::
paintUtfChar(QPainter *painter, double x, double y, ulong utf) const
{
  if (utf < 0x1fb00) {
    std::string s;
    CUtf8::append(s, utf);
    painter->drawText(x, y + ca_, QString::fromStdString(s));
  }
  else {
    auto pw = cw_/8.0;
    auto ph = ch_/8.0;

    auto fillRect = [&](const QRectF &r) {
      painter->fillRect(r, painter->pen().color());
    };

    if      (utf >= 0x1fb70 && utf <= 0x1fb75) {
      auto pos = utf - 0x1fb70 + 1;

      fillRect(QRectF(x + pos*pw, y, pw, ch_));
    }
    else if (utf >= 0x1fb76 && utf <= 0x1fb7b) {
      auto pos = utf - 0x1fb76 + 1;

      fillRect(QRectF(x, y + pos*ph, cw_, ph));
    }
    else if (utf >= 0x1fb7c && utf <= 0x1fb7f) {
      auto pos = utf - 0x1fb7c;
      bool ll = (pos == 0 || pos == 1);
      bool tl = (pos == 1 || pos == 2);

      if (ll)
        fillRect(QRectF(x           , y, pw, ch_));
      else
        fillRect(QRectF(x + cw_ - pw, y, pw, ch_));

      if (tl)
        fillRect(QRectF(x, y           , cw_, ph));
      else
        fillRect(QRectF(x, y + ch_ - ph, cw_, ph));
    }
    else if (utf >= 0x1fb82 && utf <= 0x1fb86) {
      auto h = utf - 0x1fb80; if (h >= 4) ++h;

      fillRect(QRectF(x, y, cw_, h*ph));
    }
    else if (utf >= 0x1fb87 && utf <= 0x1fb8b) {
      auto w = utf - 0x1fb85; if (w >= 4) ++w;

      fillRect(QRectF(x + (8 - w)*pw, y, w*pw, ch_));
    }
    else if (utf == 0x1fb8c) {
      pw /= 2.0;
      ph /= 2.0;

      for (int iy = 0; iy < 16; ++iy) {
        for (int ix = 0; ix < 8; ++ix) {
          if (! ((ix + iy) & 1))
            fillRect(QRectF(x + ix*pw, y + iy*ph, pw, ph));
        }
      }
    }
    else if (utf == 0x1fb8f) {
      pw /= 2.0;
      ph /= 2.0;

      for (int iy = 8; iy < 16; ++iy) {
        for (int ix = 0; ix < 16; ++ix) {
          if (! ((ix + iy) & 1))
            fillRect(QRectF(x + ix*pw, y + iy*ph, pw, ph));
        }
      }
    }
    else {
      std::cerr << "Unhandled: " << std::hex << utf << "\n";
    }
  }
}

void
CQPetBasicTerm::
mousePressEvent(QMouseEvent *e)
{
  QFontMetricsF fm(font());

  int r = e->y()/ch_;
  int c = e->x()/cw_;

  auto drawChar = getChar(r, c);

  auto petsci = CPetBasic::drawCharToPet(drawChar);

  if (drawChar.utf() > 0)
    std::cerr << std::hex << drawChar.utf();
  else
    std::cerr << int(drawChar.c());

  std::cerr << " (" << int(petsci.c()) << ")";

  std::cerr << (drawChar.isReverse() ? " Rev" : "") << "\n";
}

QSize
CQPetBasicTerm::
sizeHint() const
{
  QFontMetricsF fm(font());

  cw_ = fm.horizontalAdvance("X");
  ch_ = fm.height();

  return QSize(int(nc_*cw_), int(nr_*ch_));
}
