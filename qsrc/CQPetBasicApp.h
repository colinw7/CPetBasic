#ifndef CQPetBasicApp_H
#define CQPetBasicApp_H

#include <QWidget>

class CQPetBasicTerm;
class CQPetBasicKeyboard;
class CQPetBasicCommandScroll;
class CQPetBasicStatus;
class CQPetBasicDbg;
class CQPetBasicVariables;
class CQPetBasic;

class QEventLoop;

class CQPetBasicApp : public QWidget {
  Q_OBJECT

 public:
  CQPetBasicApp(QWidget *parent=nullptr);

  CQPetBasicTerm *term() const { return term_; }

  CQPetBasic *basic() const;

  QString getString();
  uchar getChar();

  void setStatusMsg(const QString &msg);

  void notifyLinesChanged();
  void notifyLineNumChanged();

  void errorMsg(const QString &msg);

 public slots:
  void executeCommand(const QString &cmd);
  void keyPress(const QString &cmd);

 private:
  CQPetBasicTerm*          term_      { nullptr };
  CQPetBasicKeyboard*      keyboard_  { nullptr };
  CQPetBasicCommandScroll* command_   { nullptr };
  CQPetBasicStatus*        status_    { nullptr };
  CQPetBasicDbg*           dbg_       { nullptr };
  CQPetBasicVariables*     variables_ { nullptr };

  QEventLoop* loop_     { nullptr };
  bool        looping_  { false };
  bool        loopChar_ { false };
  QString     loopStr_;
  uchar       loopC_    { 0 };
};

#endif
