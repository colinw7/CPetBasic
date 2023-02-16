#include <CQPetBasicCommand.h>
#include <CQPetBasicApp.h>
#include <CQPetBasicTerm.h>
#include <CQPetBasic.h>

CQPetBasicCommandScroll::
CQPetBasicCommandScroll(CQPetBasicApp *app) :
 CQCommand::ScrollArea(app), app_(app)
{
  connect(this, SIGNAL(executeCommand(const QString &)),
          this, SLOT(executeCommandSlot(const QString &)));
  connect(this, SIGNAL(keyPress(const QString &)),
          this, SLOT(keyPressSlot(const QString &)));
}

CQCommand::CommandWidget *
CQPetBasicCommandScroll::
createCommandWidget() const
{
  auto *th = const_cast<CQPetBasicCommandScroll *>(this);

  th->command_ = new CQPetBasicCommandTerm(th);

  return command_;
}

void
CQPetBasicCommandScroll::
executeCommandSlot(const QString &cmd)
{
  auto *term = app_->term();

  if (term->isLooping()) {
    term->enterLoopStr(cmd.toStdString());
  }
  else {
    auto *basic = app_->basic();

    basic->inputLine(cmd.toStdString());
  }
}

void
CQPetBasicCommandScroll::
keyPressSlot(const QString &str)
{
  auto *term = app_->term();

  if (term->isLooping() && term->isLoopChar() && str.length()) {
    command()->clearEntry();

    term->enterLoopStr(str.toStdString());
  }
}

QSize
CQPetBasicCommandScroll::
sizeHint() const
{
  return QSize(1000, 400);
}

//---

CQPetBasicCommandTerm::
CQPetBasicCommandTerm(CQPetBasicCommandScroll *scroll) :
 CQCommand::CommandWidget(scroll), scroll_(scroll)
{
}

QSize
CQPetBasicCommandTerm::
sizeHint() const
{
  return QSize(1000, 400);
}
