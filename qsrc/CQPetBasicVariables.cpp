#include <CQPetBasicVariables.h>
#include <CQPetBasicApp.h>
#include <CQPetBasic.h>
#include <CExpr.h>

#include <CQDataModel.h>
#include <CQUtil.h>

CQPetBasicVariables::
CQPetBasicVariables(CQPetBasicApp *app) :
 app_(app)
{
  setObjectName("variables");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  view_ = new CQPetBasicVariablesList(this);

  layout->addWidget(view_);
}

void
CQPetBasicVariables::
reload()
{
  if (! isVisible())
    needsReload_ = true;
  else
    view_->updateModel();
}

void
CQPetBasicVariables::
showEvent(QShowEvent *)
{
  if (needsReload_) {
    needsReload_ = false;

    view_->updateModel();
  }
}

QSize
CQPetBasicVariables::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(40*fm.horizontalAdvance("X"), 20*fm.height());
}

//---

CQPetBasicVariablesList::
CQPetBasicVariablesList(CQPetBasicVariables *variables) :
 variables_(variables)
{
  setObjectName("list");

  model_ = new CQDataModel;

  updateModel();

  setModel(model_);
}

void
CQPetBasicVariablesList::
updateModel()
{
  auto *basic = variables_->app()->basic();

  std::vector<std::string> names, arrayNames;

  basic->getVariableNames(names, arrayNames);

  auto nr1 = names.size();
  auto nr2 = arrayNames.size();

  model_->resizeModel(2, nr1 + nr2);

  model_->setHeaderData(0, Qt::Horizontal, "Name");
  model_->setHeaderData(1, Qt::Horizontal, "Value");

  uint r = 0;

  for (uint r1 = 0; r1 < nr1; ++r1, ++r) {
    auto ind0 = model_->index(r, 0, QModelIndex());
    model_->setData(ind0, QString::fromStdString(names[r1]));

    auto value = basic->getVariableValue(names[r1]);

    QVariant vvalue;

    std::string s;
    if (value && value->getStringValue(s))
      vvalue = QString::fromStdString(s);

    auto ind1 = model_->index(r, 1, QModelIndex());
    model_->setData(ind1, vvalue);
  }

  for (uint r2 = 0; r2 < nr2; ++r2, ++r) {
    auto ind0 = model_->index(r, 0, QModelIndex());
    model_->setData(ind0, QString::fromStdString(arrayNames[r2]));

    QVariant vvalue;

    CPetBasic::Inds inds; inds.push_back(0);

    auto value = basic->getVariableValue(arrayNames[r2], inds);

    std::string s;
    if (value && value->getStringValue(s))
      vvalue = QString::fromStdString(s);

    auto ind1 = model_->index(r, 1, QModelIndex());
    model_->setData(ind1, vvalue);
  }
}
