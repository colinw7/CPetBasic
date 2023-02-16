#include <CQPetBasicKeyboard.h>
#include <CQPetBasicApp.h>
#include <CQPetBasicTerm.h>
#include <CQPetBasic.h>

#include <QMouseEvent>
#include <QPainter>

#include <images/pet_keyboard_png.h>

CQPetBasicKeyboard::
CQPetBasicKeyboard(CQPetBasicApp *app) :
 QFrame(app), app_(app)
{
  pixmap_.loadFromData(pet_keyboard_data, PET_KEYBOARD_DATA_LEN);

  setFixedSize(pixmap_.size());

  //---

  xl_ = 24;
  yt_ = 21;
  xr_ = 1187;
  lw_ = 97;
  lh_ = 97;
}

void
CQPetBasicKeyboard::
mousePressEvent(QMouseEvent *e)
{
//std::cerr << e->x() << " " << e->y() << "\n";

  ix_ = (e->x() - xl_)/lw_;
  iy_ = (e->y() - yt_)/lh_;

  static std::string kmap =
   "!|#$%'& ()~|||||"
   "QWERTYUIOP^|789/"
   "ASDFGHJKL:||456*"
   "ZXCVBNM,;?||123+"
   "||@[]||<>|||0.-=";

  kmap[1] = '\"';
  kmap[7] = '\\';

  //---

  auto *term  = app_->term();
  auto *basic = term->basic();

  auto sendChar = [&](const CPetDrawChar &drawChar1) {
    if (term->isLooping()) {
      auto c = char(drawChar1.c());

      auto s = std::string(&c, 1);

      if (term->isLoopChar()) {
        term->enterLoopStr(s);
        return;
      }

      term->addLoopStr(s);
    }

    auto drawChar2 = drawChar1;
    drawChar2.setReverse(basic->isReverse());
    term->setChar(term->row(), term->col(), drawChar2);
    term->cursorRight();
    term->update();
  };

  auto sendCtrlChar = [&](uchar c1) {
    CPetsciChar pet1(c1);
    auto drawChar2 = CPetBasic::petToDrawChar(pet1);
    term->setChar(term->row(), term->col(), drawChar2);
    term->cursorRight();
    term->update();
  };

  //---

  int i = iy_*16 + ix_;

  if (i == 64 || i == 74) { // LSHIFT/RSHIFT
    basic->setShift(! basic->isShift());
    update();
    return;
  }

  if (i == 65) { // REV/OFF
    if (! term->inQuotes()) {
      basic->setReverse(! basic->isShift());
      update();
    }
    else {
      sendCtrlChar(! basic->isShift() ? 146 : 210);
    }
    return;
  }

  if (i == 12) { // HOM/CLS
    if (! term->inQuotes()) {
      if (! basic->isShift())
        term->home();
      else
        term->clear();
      term->update(); // needed
    }
    else {
      sendCtrlChar(! basic->isShift() ? 147 : 211);
    }
    return;
  }

  if (i == 13) { // CD/CU
    if (! term->inQuotes()) {
      if (! basic->isShift())
        term->cursorDown();
      else
        term->cursorUp();
      term->update();
    }
    else {
      sendCtrlChar(! basic->isShift() ? 145 : 209);
    }
    return;
  }

  if (i == 14) { // CR/CL
    if (! term->inQuotes()) {
      if (! basic->isShift())
        term->cursorRight();
      else
        term->cursorLeft();
      term->update();
    }
    else {
      sendCtrlChar(! basic->isShift() ? 157 : 221);
    }
    return;
  }

  if (i == 15) { // DEL/INST
    if (! basic->isShift())
      term->del();
    else
      term->inst();
    term->update();
    return;
  }

  if (i == 42 || i == 58) { //RETURN
    if (term->isLooping())
      term->enterLoopStr("");
    else {
      term->enter(); term->update();
    }
    return;
  }

  if (i == 73) { // STOP/RUN
    if (! basic->isShift())
      basic->setStopped(true);
    else
      basic->run();
    term->update();
    return;
  }

  //---

  if      (i == 69 || i == 70) {
    sendChar(CPetDrawChar(' '));
  }
  else {
    uchar c = 0;

    if (i >= 0 && i < int(kmap.size())) {
      c = kmap[i];

      if (c == '|')
        c = 0;
    }

    if (c == 0)
      return;

    CPetDrawChar drawChar;

    drawChar.setC(c);

    if (basic->isShift()) {
      auto pet = CPetBasic::drawCharToPet(drawChar);

      pet.shift();

      drawChar = CPetBasic::petToDrawChar(pet);
    }

    sendChar(drawChar);
  }
}

void
CQPetBasicKeyboard::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.drawPixmap(QPoint(0,0), pixmap_);

  auto *term  = app_->term();
  auto *basic = term->basic();

  auto highlightKey = [&](int i) {
    int iy = i/16;
    int ix = i - iy*16;

    QColor c(100, 100, 100, 100);

    painter.fillRect(QRect(xl_ + ix*lw_, yt_ + iy*lh_, lw_, lh_), c);
  };

  if (basic->isShift()) {
    highlightKey(64);
    highlightKey(74);
  }

  if (basic->isReverse())
    highlightKey(65);
}
