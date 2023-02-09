#include <CQPetBasicTerm.h>
#include <CQPetBasicApp.h>
#include <CQPetBasic.h>

#include <CQUtil.h>
#include <CUtf8.h>

#include <QTimer>
#include <QMouseEvent>
#include <QPainter>

CQPetBasicTerm::
CQPetBasicTerm(CQPetBasicApp *app) :
 QWidget(app), app_(app)
{
  setObjectName("perTerm");

#if 0
  auto font = qApp->font();
  font.setPointSizeF(32);
#else
  auto font = QFont("Courier", 32);
#endif
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
  // ASCII with embedded escape codes for special chars
  int i   = 0;
  int len = str.length();

  //---

  while (i < len) {
    auto c  = str[i];
    auto c1 = uchar(c);

    // handle new line
    if      (c == '\n') {
      ++r_;

      cursorLeftFull();

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

    // handle embedded escape codes
    else if (c1 < 8 || (c1 >= 16 && c1 < 20) || c1 >= 128) {
      if      (c1 ==  1) home();
      else if (c1 ==  2) clear();
    //else if (c1 ==  3) stop();
    //else if (c1 ==  4) pi();
    //else if (c1 ==  5) ins();
      else if (c1 ==  6) reverse_ = true;
      else if (c1 ==  7) reverse_ = false;
      else if (c1 == 16) cursorUp();
      else if (c1 == 17) cursorDown();
      else if (c1 == 18) cursorLeft();
      else if (c1 == 19) cursorRight();
      else {
        auto petsci = CPetBasic::decodeEmbedded(c1);

        bool reverse; ulong utf;
        auto ascii = CPetBasic::petToAscii(petsci, utf, reverse);

        setChar(r_, c_, ascii, utf, reverse);

        cursorRight(/*nl*/false);
      }
    }

    // handle normal char
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
cursorLeftFull()
{
  c_ = 0;
}

void
CQPetBasicTerm::
del()
{
  if (c_ > 0)
    setChar(r_, --c_, ' ', 0, false);
}

void
CQPetBasicTerm::
inst()
{
}

void
CQPetBasicTerm::
enter()
{
  std::string str;

  for (uint c = 0; c < c_; ++c) {
    ulong utf; bool reverse;
    auto c1 = getChar(r_, c, utf, reverse);

    if (utf)
      CUtf8::append(str, utf);
    else
      str += c1;
  }

  cursorDown();
  cursorLeftFull();

  basic_->inputLine(str);
}

//---

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
  QFontMetricsF fm(font());

  cw_ = fm.horizontalAdvance("X");
  ch_ = fm.height();
  ca_ = fm.ascent();

  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);

  double y = 0;

  for (uint r = 0; r < nr_; ++r) {
    double x = 0;

    for (uint c = 0; c < nc_; ++c) {
      bool reverse; ulong utf;
      auto ic = getChar(r, c, utf, reverse);

      if (reverse) {
        painter.fillRect(QRect(x, y, cw_, ch_), Qt::white);

        painter.setPen(Qt::black);
      }
      else {
        painter.setPen(Qt::white);
      }

      if (r == r_ && c == c_) {
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

      if (utf > 0)
        paintUtfChar(&painter, x, y, utf);
      else
        paintChar(&painter, x, y, ic);

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

  bool reverse; ulong utf;
  auto ascii = getChar(r, c, utf, reverse);

  auto petsci = CPetBasic::asciiToPet(ascii, utf, reverse);

  if (utf > 0)
    std::cerr << std::hex << utf;
  else
    std::cerr << int(ascii);

  std::cerr << " (" << int(petsci) << ")";

  std::cerr << (reverse ? " Rev" : "") << "\n";
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
