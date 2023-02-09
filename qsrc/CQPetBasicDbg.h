#ifndef CQPetBasicDbg_H
#define CQPetBasicDbg_H

#include <QFrame>

class CQPetBasic;
class CQPetBasicFileView;

class CQScrollArea;
class CQPixmapButton;

class CQPetBasicDbg : public QFrame {
  Q_OBJECT

 public:
  CQPetBasicDbg(CQPetBasic *basic);

  CQPetBasicFileView *file() const { return file_; }

 private Q_SLOTS:
  void updateFileOffset();
  void updateFileSize(const QSize &);

  void playSlot ();
  void pauseSlot();
  void stepSlot ();

 private:
  CQPetBasic*         basic_       { nullptr };
  CQScrollArea*       area_        { nullptr };
  CQPetBasicFileView* file_        { nullptr };
  CQPixmapButton*     playButton_  { nullptr };
  CQPixmapButton*     pauseButton_ { nullptr };
  CQPixmapButton*     stepButton_  { nullptr };
};

//---

class CQPetBasicFileView : public QFrame {
  Q_OBJECT

 public:
  CQPetBasicFileView(CQPetBasic *basic);

  void setOffset(const QPoint &o) { offset_ = o; }

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  QSize dataSize() const;

  QSize sizeHint() const override;

 Q_SIGNALS:
  void updateSize(const QSize &);

 private:
  CQPetBasic *basic_      { nullptr };
  QPoint      offset_;
  uint        maxLineLen_ { 80 };
};

#endif
