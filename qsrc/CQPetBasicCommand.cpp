#include <CQPetBasicCommand.h>
#include <CQPetBasicApp.h>
#include <CQPetBasic.h>

CQPetBasicCommandScroll::
CQPetBasicCommandScroll(CQPetBasicApp *app) :
 CQCommand::ScrollArea(app), app_(app)
{
}

CQCommand::CommandWidget *
CQPetBasicCommandScroll::
createCommandWidget() const
{
  auto *th = const_cast<CQPetBasicCommandScroll *>(this);

  th->command_ = new CQPetBasicCommandTerm(th);

  return command_;
}

//---

CQPetBasicCommandTerm::
CQPetBasicCommandTerm(CQPetBasicCommandScroll *scroll) :
 CQCommand::CommandWidget(scroll), scroll_(scroll)
{
}
