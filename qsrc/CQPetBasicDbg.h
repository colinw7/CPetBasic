#ifndef CQPetBasicDbg_H
#define CQPetBasicDbg_H

#include <QFrame>
#include <set>

class CQPetBasic;
class CQPetBasicFileView;

class CQScrollArea;
class CQPixmapButton;

class CQPetBasicDbg : public QFrame {
  Q_OBJECT

 public:
  CQPetBasicDbg(CQPetBasic *basic);

  CQPetBasicFileView *file() const { return file_; }

  void scrollVisible();

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

  int markerLineNum() const;

  QPoint lineIndPos(int lineInd) const;

  void resizeEvent(QResizeEvent *) override;

  void mouseDoubleClickEvent(QMouseEvent *e) override;

  void paintEvent(QPaintEvent *) override;

  QSize dataSize() const;

  QSize sizeHint() const override;

 Q_SIGNALS:
  void updateSize(const QSize &);

 private:
  using Markers = std::set<int>;

  CQPetBasic *basic_      { nullptr };
  QPoint      offset_;
  uint        maxLineLen_ { 80 };

  Markers markers_;

  mutable int tw_ { 8 };
  mutable int th_ { 8 };
  mutable int ta_ { 8 };
};

#endif
