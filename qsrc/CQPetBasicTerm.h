#ifndef CQPetBasicTerm_H
#define CQPetBasicTerm_H

#include <CPetBasic.h>
#include <CPetBasicTerm.h>
#include <QWidget>

class CQPetBasicApp;
class CQPetBasic;

class QTimer;
class QEventLoop;

class CQPetBasicTerm : public QWidget, public CPetBasicTerm {
  Q_OBJECT

 public:
  CQPetBasicTerm(CQPetBasicApp *app);
 ~CQPetBasicTerm();

  void init() override;

  //---

  CQPetBasicApp *app() const { return app_; }

  CQPetBasic *basic() const;

  //---

  bool isTty() const override { return false; }
  bool isRaw() const override { return true; }

  uint row() const override { return r_; }
  uint col() const override { return c_; }

  uint numRows() const override { return nr_; }
  uint numCols() const override { return nc_; }

  //---

  void resize(uint nr, uint nc) override;

  //---

  bool isLooping() const { return looping_; }
  bool isLoopChar() const { return loopChar_; }
  QString loopStr() const { return loopStr_; }

  char readChar() const override;

  std::string readString(const std::string &prompt) const override;

  void setLoopChar(char c);
  void addLoopChar(char c);
  void setLoopStr(const QString &str);

  //---

  void inst() { }

  void update() override;

  void delay(long t) override;

  //---

  void keyPressEvent(QKeyEvent *e) override;
  void keyReleaseEvent(QKeyEvent *e) override;

  void paintEvent(QPaintEvent *e) override;

  void paintChar(QPainter *painter, double x, double y, uchar c) const;
  void paintUtfChar(QPainter *painter, double x, double y, ulong utf) const;

  void mousePressEvent(QMouseEvent *e) override;

  QSize sizeHint() const override;

 private Q_SLOTS:
  void cursorTimeout();

  void loopTimeout();

 private:
  CQPetBasicApp* app_ { nullptr };

  QTimer *cursorTimer_ { nullptr };
  bool    cursorBlink_ { false };

  QTimer*     loopTimer_ { nullptr };
  QEventLoop* loop_      { nullptr };
  bool        looping_   { false };
  bool        loopChar_  { false };
  QString     loopStr_;
  uchar       loopC_     { 0 };

  mutable double cw_ { 8.0 };
  mutable double ch_ { 8.0 };
  mutable double ca_ { 8.0 };
};

#endif
