#ifndef CQPetBasic_H
#define CQPetBasic_H

#include <CQCommand.h>
#include <CPetBasic.h>
#include <QWidget>

class CQPetBasic;
class CQPetBasicTerm;
class CQPetBasicCommandScroll;
class CQPetBasicCommandTerm;
class CQPetBasicStatus;

class QEventLoop;
class QLabel;
class QTimer;

//---

class CQPetBasicApp : public QWidget {
  Q_OBJECT

 public:
  CQPetBasicApp(QWidget *parent=nullptr);

  CQPetBasicTerm *term() const { return term_; }

  CQPetBasic *basic() const;

  QString getString();
  uchar getChar();

  void setStatusMsg(const QString &msg);

  void errorMsg(const QString &msg);

 public slots:
  void executeCommand(const QString &cmd);
  void keyPress(const QString &cmd);

 private:
  CQPetBasicTerm*          term_    { nullptr };
  CQPetBasicCommandScroll* command_ { nullptr };
  CQPetBasicStatus*        status_  { nullptr };

  QEventLoop* loop_     { nullptr };
  bool        looping_  { false };
  bool        loopChar_ { false };
  QString     loopStr_;
  uchar       loopC_    { 0 };
};

//---

class CQPetBasic : public CPetBasic {
 public:
  CQPetBasic(CQPetBasicTerm *term);

  void printString(const std::string &s) const override;

  char getChar() const override;

  std::string getString(const std::string &prompt) const override;

  void resize(uint nr, uint nc) override;

  bool getScreenMemory(uint r, uint c, uchar &value) const override;
  void setScreenMemory(uint r, uint c, uchar value) override;

  void delay() override;

  void notifyRunLine(uint n) const override;

 private:
  CQPetBasicTerm *term_ { nullptr };
};

//---

class CQPetBasicTerm : public QWidget {
  Q_OBJECT

 public:
  CQPetBasicTerm(CQPetBasicApp *app);
 ~CQPetBasicTerm();

  //---

  CQPetBasicApp *app() const { return app_; }

  CQPetBasic *basic() const { return basic_; }

  //---

  void resize(uint nr, uint nc);

  void moveTo(uint r, uint c);

  void drawString(const std::string &str);

  uchar getChar(uint r, uint c, ulong &utf, bool &reverse) const;
  void setChar(uint r, uint c, uchar value, ulong utf, bool reverse);

  uint encodeCharPos(uint r, uint c) const { return r*nc_ + c; }
  void decodeCharPos(uint pos, uint &r, uint &c) const { r = pos/nc_; c = pos - r*nc_; }

  //---

  void paintEvent(QPaintEvent *e) override;

  QSize sizeHint() const override;

 private:
  bool drawChar1(const uchar &c);

  void home();
  void clear();

  void cursorUp();
  void cursorDown();
  void cursorLeft();
  void cursorRight(bool nl=true);

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
};

//---

class CQPetBasicCommandScroll : public CQCommand::ScrollArea {
  Q_OBJECT

 public:
  CQPetBasicCommandScroll(CQPetBasicApp *app);

  CQPetBasicApp *app() const { return app_; }

  CQPetBasicCommandTerm *command() const { return command_; }

 protected:
  CQCommand::CommandWidget *createCommandWidget() const override;

 private:
  CQPetBasicApp*         app_     { nullptr };
  CQPetBasicCommandTerm* command_ { nullptr };
};

//---

class CQPetBasicCommandTerm : public CQCommand::CommandWidget {
  Q_OBJECT

 public:
  CQPetBasicCommandTerm(CQPetBasicCommandScroll *scroll);

 private:
  CQPetBasicCommandScroll* scroll_ { nullptr };
};

//---

class CQPetBasicStatus : public QFrame {
  Q_OBJECT

 public:
  CQPetBasicStatus(CQPetBasicApp *app);

  void setText(const QString &text);

 private Q_SLOTS:
  void stopSlot();

 private:
  CQPetBasicApp* app_  { nullptr };
  QLabel*        text_ { nullptr };
};

#endif
