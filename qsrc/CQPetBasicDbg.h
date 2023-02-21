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

  Q_PROPERTY(QColor bgColor       READ bgColor       WRITE setBgColor)
  Q_PROPERTY(QColor fgColor       READ fgColor       WRITE setFgColor)
  Q_PROPERTY(QColor keywordColor  READ keywordColor  WRITE setKeywordColor)
  Q_PROPERTY(QColor stringColor   READ stringColor   WRITE setStringColor )
  Q_PROPERTY(QColor operatorColor READ operatorColor WRITE setOperatorColor)
  Q_PROPERTY(QColor markColor     READ markColor     WRITE setMarkColor)
  Q_PROPERTY(QColor currentColor  READ currentColor  WRITE setCurrentColor)

 public:
  CQPetBasicDbg(CQPetBasic *basic);

  CQPetBasic *basic() const { return basic_; }

  CQPetBasicFileView *file() const { return file_; }

  //---

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c) { bgColor_ = c; }

  const QColor &fgColor() const { return fgColor_; }
  void setFgColor(const QColor &c) { fgColor_ = c; }

  const QColor &keywordColor() const { return keywordColor_; }
  void setKeywordColor(const QColor &v) { keywordColor_ = v; }

  const QColor &stringColor() const { return stringColor_; }
  void setStringColor(const QColor &c) { stringColor_ = c; }

  const QColor &operatorColor() const { return operatorColor_; }
  void setOperatorColor(const QColor &c) { operatorColor_ = c; }

  const QColor &markColor() const { return markColor_; }
  void setMarkColor(const QColor &c) { markColor_ = c; }

  const QColor &currentColor() const { return currentColor_; }
  void setCurrentColor(const QColor &c) { currentColor_ = c; }

  //---

  void scrollVisible();

 private Q_SLOTS:
  void updateFileOffset();
  void updateFileSize(const QSize &);

  void playSlot ();
  void pauseSlot();
  void stepSlot ();

 private:
  CQPetBasic* basic_ { nullptr };

  QColor bgColor_       { 240, 240, 240 };
  QColor fgColor_       { 0, 0, 0 };
  QColor keywordColor_  { 56, 88, 158 };
  QColor stringColor_   { 60, 122, 62 };
  QColor operatorColor_ { 140, 60, 60 };
  QColor markColor_     { 255, 0, 0 };
  QColor currentColor_  { 150, 150, 200 };

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
  CQPetBasicFileView(CQPetBasicDbg *dbg);

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

  CQPetBasicDbg *dbg_ { nullptr };

  QPoint offset_;
  uint   maxLineLen_ { 80 };

  Markers markers_;

  mutable int tw_ { 8 };
  mutable int th_ { 8 };
  mutable int ta_ { 8 };
};

#endif
