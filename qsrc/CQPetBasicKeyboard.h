#ifndef CQPetBasicKeyboard_H
#define CQPetBasicKeyboard_H

#include <QFrame>

class CQPetBasicApp;

class CQPetBasicKeyboard : public QFrame {
  Q_OBJECT

 public:
  CQPetBasicKeyboard(CQPetBasicApp *app);

  CQPetBasicApp *app() const { return app_; }

  void mousePressEvent(QMouseEvent *e) override;

  void paintEvent(QPaintEvent *) override;

 private:
  CQPetBasicApp* app_ { nullptr };
  QPixmap        pixmap_;

  int xl_ { 0 };
  int yt_ { 0 };
  int xr_ { 0 };
  int lw_ { 16 };
  int lh_ { 16 };
  int ix_ { -1 };
  int iy_ { -1 };
};

#endif
