#ifndef CQPetBasicTerm_H
#define CQPetBasicTerm_H

#include <QWidget>

class CQPetBasicApp;
class CQPetBasic;

class CQPetBasicTerm : public QWidget {
  Q_OBJECT

 public:
  CQPetBasicTerm(CQPetBasicApp *app);
 ~CQPetBasicTerm();

  //---

  CQPetBasicApp *app() const { return app_; }

  CQPetBasic *basic() const { return basic_; }

  uint row() const { return r_; }
  uint col() const { return c_; }

  //---

  void resize(uint nr, uint nc);

  void moveTo(uint r, uint c);

  void drawString(const std::string &str);

  uchar getChar(uint r, uint c, ulong &utf, bool &reverse) const;
  void setChar(uint r, uint c, uchar value, ulong utf, bool reverse);

  uint encodeCharPos(uint r, uint c) const { return r*nc_ + c; }
  void decodeCharPos(uint pos, uint &r, uint &c) const { r = pos/nc_; c = pos - r*nc_; }

  //---

  void home();
  void clear();

  void cursorUp();
  void cursorDown();
  void cursorLeft();
  void cursorRight(bool nl=true);

  void cursorLeftFull();

  void del();
  void inst();

  void enter();

  //---

  void paintEvent(QPaintEvent *e) override;

  void paintChar(QPainter *painter, double x, double y, uchar c) const;
  void paintUtfChar(QPainter *painter, double x, double y, ulong utf) const;

  void mousePressEvent(QMouseEvent *e) override;

  QSize sizeHint() const override;

 private:
  bool drawChar1(const uchar &c);

  void scrollUp();

 private Q_SLOTS:
  void cursorTimeout();

 private:
  struct CharData {
    uchar c       { 0 };
    ulong utf     { 0 };
    bool  reverse { false };

    CharData() { }

    CharData(uchar c1, bool reverse1) : c(c1), reverse(reverse1) { }
    CharData(ulong utf1, bool reverse1) : utf(utf1), reverse(reverse1) { }
  };

  using Chars = std::vector<CharData>;

  CQPetBasicApp* app_   { nullptr };
  CQPetBasic*    basic_ { nullptr };

  QTimer *cursorTimer_ { nullptr };
  bool    cursorBlink_ { false };

  Chars chars_;
  uint  nr_      { 0 };
  uint  nc_      { 0 };
  uint  r_       { 0 };
  uint  c_       { 0 };
  bool  reverse_ { false };

  mutable double cw_ { 8.0 };
  mutable double ch_ { 8.0 };
  mutable double ca_ { 8.0 };
};

#endif
