#include <CQPetBasicVariables.h>

#include <CQTableWidget.h>
#include <CQUtil.h>

CQPetBasicVariables::
CQPetBasicVariables(CQPetBasicApp *app) :
 app_(app)
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  auto *table = CQUtil::makeWidget<CQTableWidget>("table");

  layout->addWidget(table);
}
