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

  bool isLooping() const { return loopData_.looping; }
  bool isLoopChar() const { return loopData_.loopChar; }

  char readChar() const override;

  std::string readString(const std::string &prompt) const override;

  void enterLoopStr(const std::string &s);
  void addLoopStr(const std::string &s);

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

  struct LoopData {
    QTimer*     loopTimer { nullptr };
    QEventLoop* eventLoop { nullptr };
    bool        looping   { false };
    bool        loopChar  { false };
    std::string loopStr;
    uchar       loopC     { 0 };
  };

  LoopData loopData_;

  mutable double cw_ { 8.0 };
  mutable double ch_ { 8.0 };
  mutable double ca_ { 8.0 };
};

#endif
