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

  auto is = QSize(48, 48);

  playButton_  = new CQPixmapButton(CQPixmapCacheInst->getIcon("PLAY"    ), is);
  pauseButton_ = new CQPixmapButton(CQPixmapCacheInst->getIcon("PAUSE"   ), is);
  stepButton_  = new CQPixmapButton(CQPixmapCacheInst->getIcon("PLAY_ONE"), is);

  playButton_ ->setToolTip("Play");
  pauseButton_->setToolTip("Pause");
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
updateFileSize(const QSize &size)
{
  area_->setSize(size);

  area_->update();
}

void
CQPetBasicDbg::
playSlot()
{
}

void
CQPetBasicDbg::
pauseSlot()
{
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

void
CQPetBasicFileView::
paintEvent(QPaintEvent *)
{
  auto currentLineNum = basic_->currentLineNum();

  QPainter painter(this);

  painter.fillRect(rect(), Qt::white);

  QFontMetrics fm(font());

  auto tw = fm.horizontalAdvance("X");
  auto th = fm.height();
  auto ta = fm.ascent();

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

    painter.setPen(Qt::black);

    painter.drawText(x, y + ta, lineNumStr);

    x += (lw + 1)*tw;

    //---

    auto isCurrent = (currentLineNum > 0 && lineData.lineN == uint(currentLineNum));

    painter.setPen(isCurrent ? Qt::red : Qt::black);

    auto lineStr = basic_->lineToString(lineData, /*highlight*/false);

    painter.drawText(x, y + ta, QString::fromStdString(lineStr));

    y += th;

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

  auto tw = fm.horizontalAdvance("X");
  auto th = fm.height();

  auto nl = basic_->numLines();

  return QSize(80*tw, nl*th);
}

QSize
CQPetBasicFileView::
sizeHint() const
{
  QFontMetrics fm(font());

  auto tw = fm.horizontalAdvance("X");
  auto th = fm.height();

  return QSize(80*tw, 40*th);
}
