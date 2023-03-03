#ifndef CQPetBasicVariables_H
#define CQPetBasicVariables_H

#include <CQModelView.h>
#include <QFrame>

class CQPetBasicApp;
class CQPetBasicVariablesList;
class CQModelView;
class CQDataModel;

class CQPetBasicVariables : public QFrame {
  Q_OBJECT

 public:
  CQPetBasicVariables(CQPetBasicApp *app);

  CQPetBasicApp *app() const { return app_; }

  void reload();

  void showEvent(QShowEvent *) override;

  QSize sizeHint() const override;

 private:
  CQPetBasicApp*           app_         { nullptr };
  CQPetBasicVariablesList* view_        { nullptr };
  CQDataModel*             model_       { nullptr };
  bool                     needsReload_ { true };
};

class CQPetBasicVariablesList : public CQModelView {
  Q_OBJECT

 public:
  CQPetBasicVariablesList(CQPetBasicVariables *variables);

  void updateModel();

 private:
  CQPetBasicVariables* variables_ { nullptr };
  CQDataModel*         model_     { nullptr };
};

#endif
