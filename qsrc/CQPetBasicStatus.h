#ifndef CQPetBasicStatus_H
#define CQPetBasicStatus_H

#include <QFrame>

class CQPetBasicApp;
class QLabel;

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
