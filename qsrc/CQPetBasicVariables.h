#ifndef CQPetBasicVariables_H
#define CQPetBasicVariables_H

#include <QFrame>

class CQPetBasicApp;
class CQTableWidget;

class CQPetBasicVariables : public QFrame {
  Q_OBJECT

 public:
  CQPetBasicVariables(CQPetBasicApp *app);

 private:
  CQPetBasicApp* app_   { nullptr };
  CQTableWidget* table_ { nullptr };
};

#endif
