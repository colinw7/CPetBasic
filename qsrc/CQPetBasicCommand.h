#ifndef CQPetBasicCommand_H
#define CQPetBasicCommand_H

#include <CQCommand.h>

class CQPetBasicApp;
class CQPetBasicCommandTerm;

class CQPetBasicCommandScroll : public CQCommand::ScrollArea {
  Q_OBJECT

 public:
  CQPetBasicCommandScroll(CQPetBasicApp *app);

  CQPetBasicApp *app() const { return app_; }

  CQPetBasicCommandTerm *command() const { return command_; }

  QSize sizeHint() const override;

 private Q_SLOTS:
  void executeCommandSlot(const QString &cmd);

  void keyPressSlot(const QString &cmd);

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

  QSize sizeHint() const override;

 private:
  CQPetBasicCommandScroll* scroll_ { nullptr };
};

#endif
