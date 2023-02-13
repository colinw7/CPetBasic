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

class QTabWidget;
class QTimer;
class QEventLoop;

class CQPetBasicApp : public QWidget {
  Q_OBJECT

 public:
  CQPetBasicApp(QWidget *parent=nullptr);

  CQPetBasic *basic() const { return basic_; }

  CQPetBasicTerm *term() const { return term_; }

  QString getString();
  uchar getChar();

  bool isKeyboardVisible() const;
  void setKeyboardVisible(bool b);

  bool isConsoleVisible() const;
  void setConsoleVisible(bool b);

  bool isDebugVisible() const;
  void setDebugVisible(bool b);

  void setStatusMsg(const QString &msg);

  void updateInterface();

  void notifyLinesChanged();
  void notifyLineNumChanged();

  void notifyVariablesChanged();

  void errorMsg(const QString &msg);

 private:
  CQPetBasic*              basic_     { nullptr };
  CQPetBasicTerm*          term_      { nullptr };
  CQPetBasicKeyboard*      keyboard_  { nullptr };
  CQPetBasicCommandScroll* command_   { nullptr };
  QTabWidget*              debugTab_  { nullptr };
  CQPetBasicDbg*           dbg_       { nullptr };
  CQPetBasicVariables*     variables_ { nullptr };
  CQPetBasicStatus*        status_    { nullptr };
};

#endif
