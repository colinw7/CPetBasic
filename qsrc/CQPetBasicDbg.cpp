#include <CQPetBasicDbg.h>
#include <CQPetBasic.h>
#include <CQScrollArea.h>

#include <CQPixmapButton.h>
#include <CQPixmapCache.h>
#include <CQUtil.h>

#include <QPainter>
#include <QApplication>
#include <QToolButton>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <cmath>

#include <svg/play_svg.h>
#include <svg/pause_svg.h>
#include <svg/play_one_svg.h>

CQPetBasicDbg::
CQPetBasicDbg(CQPetBasic *basic) :
 basic_(basic)
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  area_ = new CQScrollArea;
  file_ = new CQPetBasicFileView(basic_);

  area_->setWidget(file_);

  connect(area_, SIGNAL(updateArea()), this, SLOT(updateFileOffset()));
  connect(file_, SIGNAL(updateSize(const QSize &)), this, SLOT(updateFileSize(const QSize &)));

  layout->addWidget(area_);

  area_->setSize(file_->dataSize());

  auto *toolbarFrame  = CQUtil::makeWidget<QFrame>(this, "toolbar");
  auto *toolbarLayout = CQUtil::makeLayout<QHBoxLayout>(toolbarFrame, 2, 2);

  toolbarFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  auto is = QSize(48, 48);

  playButton_  = new CQPixmapButton(CQPixmapCacheInst->getIcon("PLAY"    ), is);
  pauseButton_ = new CQPixmapButton(CQPixmapCacheInst->getIcon("PAUSE"   ), is);
  stepButton_  = new CQPixmapButton(CQPixmapCacheInst->getIcon("PLAY_ONE"), is);

  playButton_ ->setToolTip("Run");
  pauseButton_->setToolTip("Stop");
  stepButton_ ->setToolTip("Step");

  connect(playButton_ , SIGNAL(clicked()), this, SLOT(playSlot ()));
  connect(pauseButton_, SIGNAL(clicked()), this, SLOT(pauseSlot()));
  connect(stepButton_ , SIGNAL(clicked()), this, SLOT(stepSlot ()));

  toolbarLayout->addWidget(playButton_ );
  toolbarLayout->addWidget(pauseButton_);
  toolbarLayout->addWidget(stepButton_ );
  toolbarLayout->addStretch();

  layout->addWidget(toolbarFrame);
}

void
CQPetBasicDbg::
updateFileOffset()
{
  file_->setOffset(area_->getOffset());

  file_->update();
}

void
CQPetBasicDbg::
scrollVisible()
{
  int lineInd = basic_->lineInd();

  auto pos = file_->lineIndPos(lineInd);

  area_->ensureVisible(pos.x(), pos.y(), 100, 100);
}

void
CQPetBasicDbg::
updateFileSize(const QSize &size)
{
  area_->setSize(size);

  area_->update();
}

void
CQPetBasicDbg::
playSlot()
{
  auto lineNum = file_->markerLineNum();

  if (lineNum > 0)
    basic_->contRunTo(lineNum);
  else
    basic_->contRun();
}

void
CQPetBasicDbg::
pauseSlot()
{
  basic_->setStopped(true);
}

void
CQPetBasicDbg::
stepSlot()
{
  basic_->step();
}

//---

CQPetBasicFileView::
CQPetBasicFileView(CQPetBasic *basic) :
 basic_(basic)
{
  setObjectName("fileView");

  auto font = QFont("Courier", 24);
  setFont(font);
}

int
CQPetBasicFileView::
markerLineNum() const
{
  if (markers_.empty())
    return -1;

  return *markers_.begin();
}

QPoint
CQPetBasicFileView::
lineIndPos(int lineInd) const
{
  return QPoint(0, th_*lineInd);
}

void
CQPetBasicFileView::
mouseDoubleClickEvent(QMouseEvent *e)
{
  int r = (e->y() - offset_.y())/th_;
  int c = (e->x() - offset_.x())/tw_;
  if (r < 0 || c < 0) return;

  auto *lineData = basic_->getLineIndData(uint(r));
  if (! lineData) return;

  markers_.clear();

  markers_.insert(lineData->lineN);

  update();
}

void
CQPetBasicFileView::
paintEvent(QPaintEvent *)
{
  auto currentLineNum = basic_->currentLineNum();

  QPainter painter(this);

  painter.fillRect(rect(), Qt::white);

  QFontMetrics fm(font());

  tw_ = fm.horizontalAdvance("X");
  th_ = fm.height();
  ta_ = fm.ascent();

  auto nl = basic_->maxLine();

  uint lw = uint(std::log10(nl) + 1);

  int y = offset_.y();

  maxLineLen_ = lw + 1;

  for (const auto &pl : basic_->getLines()) {
    const auto &lineData = pl.second;

    int x = offset_.x();

    auto lineNumStr = QString::number(lineData.lineN);

    while (uint(lineNumStr.size()) < lw)
      lineNumStr = " " + lineNumStr;

    auto marked = (markers_.find(lineData.lineN) != markers_.end());

    if (marked)
      painter.setPen(Qt::green);
    else
      painter.setPen(Qt::black);

    painter.drawText(x, y + ta_, lineNumStr);

    x += (lw + 1)*tw_;

    //---

    auto isCurrent = (currentLineNum > 0 && lineData.lineN == uint(currentLineNum));

    painter.setPen(isCurrent ? Qt::red : Qt::black);

    auto lineStr = basic_->lineToString(lineData, /*highlight*/false);

    painter.drawText(x, y + ta_, QString::fromStdString(lineStr));

    y += th_;

    maxLineLen_ = std::max(maxLineLen_, uint(lw + 1 + lineStr.size()));
  }
}

void
CQPetBasicFileView::
resizeEvent(QResizeEvent *)
{
  Q_EMIT updateSize(dataSize());
}

QSize
CQPetBasicFileView::
dataSize() const
{
  QFontMetrics fm(font());

  tw_ = fm.horizontalAdvance("X");
  th_ = fm.height();

  auto nl = basic_->numLines();

  return QSize(80*tw_, nl*th_);
}

QSize
CQPetBasicFileView::
sizeHint() const
{
  QFontMetrics fm(font());

  tw_ = fm.horizontalAdvance("X");
  th_ = fm.height();

  return QSize(80*tw_, 40*th_);
}
