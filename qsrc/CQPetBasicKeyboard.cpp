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
}

void
CQPetBasicKeyboard::
mousePressEvent(QMouseEvent *e)
{
//std::cerr << e->x() << " " << e->y() << "\n";

  xl_ = 24;
  yt_ = 21;
  xr_ = 1187;
  lw_ = 97;
  lh_ = 97;

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

  int i = iy_*16 + ix_;

  if (i == 64 || i == 74) {
    shift_ = ! shift_;
    update();
    return;
  }

  if (i == 65) {
    reverse_ = ! shift_;
    update();
    return;
  }

  auto *term = app_->term();

  if (i == 12) {
    if (! shift_)
      term->home();
    else
      term->clear();
    term->update();
    return;
  }

  if (i == 13) {
    if (! shift_)
      term->cursorDown();
    else
      term->cursorUp();
    term->update();
    return;
  }

  if (i == 14) {
    if (! shift_)
      term->cursorRight();
    else
      term->cursorLeft();
    term->update();
    return;
  }

  if (i == 15) {
    if (! shift_)
      term->del();
    else
      term->inst();
    term->update();
    return;
  }

  if (i == 42 || i == 58) {
    term->enter();
    term->update();
    return;
  }

  if (i == 73) {
    //term->stop();
    //term->update();
    return;
  }

  std::string s;

  if      (i == 69 || i == 70) {
    term->setChar(term->row(), term->col(), ' ', 0, reverse_);

    term->cursorRight();
  }
  else {
    uchar c   = 0;
    ulong utf = 0;

    if (i >= 0 && i < int(kmap.size())) {
      c = kmap[i];

      if (c == '|')
        c = 0;
    }

    if (c == 0)
      return;

    if (shift_) {
      auto pet = CPetBasic::asciiToPet(c, 0, /*reverse*/false);

      pet += 64;

      bool reverse;
      c = CPetBasic::petToAscii(pet, utf, reverse);
    }

    term->setChar(term->row(), term->col(), c, utf, reverse_);

    term->cursorRight();
  }

//std::cerr << ix_ << " " << iy_ << " " << i << " '" << s << "'\n";

  update();
}

void
CQPetBasicKeyboard::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.drawPixmap(QPoint(0,0), pixmap_);

  auto highlightKey = [&](int i) {
    int iy = i/16;
    int ix = i - iy*16;

    QColor c(100, 100, 100, 100);

    painter.fillRect(QRect(xl_ + ix*lw_, yt_ + iy*lh_, lw_, lh_), c);
  };

  if (shift_) {
    highlightKey(64);
    highlightKey(74);
  }

  if (reverse_)
    highlightKey(65);
}
