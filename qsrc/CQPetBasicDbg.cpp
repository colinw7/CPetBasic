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
  setObjectName("debug");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  area_ = new CQScrollArea;
  file_ = new CQPetBasicFileView(this);

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
CQPetBasicFileView(CQPetBasicDbg *dbg) :
 dbg_(dbg)
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

  auto *basic = dbg_->basic();

  auto *lineData = basic->getLineIndData(uint(r));
  if (! lineData) return;

  markers_.clear();

  markers_.insert(lineData->lineN);

  update();
}

void
CQPetBasicFileView::
paintEvent(QPaintEvent *)
{
  auto *basic = dbg_->basic();

  auto currentLineNum = basic->currentLineNum();

  QPainter painter(this);

  painter.fillRect(rect(), Qt::white);

  QFontMetrics fm(font());

  tw_ = fm.horizontalAdvance("X");
  th_ = fm.height();
  ta_ = fm.ascent();

  auto nl = basic->maxLine();

  uint lw = uint(std::log10(nl) + 1);

  int y = offset_.y();

  maxLineLen_ = lw + 1;

  for (const auto &pl : basic->getLines()) {
    const auto &lineData = pl.second;

    auto isCurrent = (currentLineNum > 0 && lineData.lineN == uint(currentLineNum));

    //---

    int x = offset_.x();

    //---

    auto lineNumStr = QString::number(lineData.lineN);

    while (uint(lineNumStr.size()) < lw)
      lineNumStr = " " + lineNumStr;

    auto marked = (markers_.find(lineData.lineN) != markers_.end());

    if      (marked)
      painter.setPen(dbg_->markColor());
    else if (isCurrent) {
      painter.fillRect(QRect(x, y, tw_*lw, th_), dbg_->currentColor());

      painter.setPen(dbg_->bgColor());
    }
    else
      painter.setPen(dbg_->fgColor());

    painter.drawText(x, y + ta_, lineNumStr);

    x += (lw + 1)*tw_;

    //---

    std::string lineStr;
    QColor      currentPen;
    bool        needsSpace = false;

    auto drawString = [&](const std::string &str, const QColor &c) {
      if (currentPen != c) {
        currentPen = c;

        painter.setPen(currentPen);
      }

      lineStr += str;

      painter.drawText(x, y + ta_, QString::fromStdString(str));

      x += fm.horizontalAdvance(QString::fromStdString(str));
    };

    auto addSpace = [&]() {
      if (needsSpace) {
        drawString(" ", dbg_->fgColor());

        needsSpace = false;
      }
    };

    //---

    uint is = 0;

    for (const auto &statement : lineData.statements) {
      if (is > 0)
        drawString(":", dbg_->fgColor());

      for (auto *token : statement.tokens) {
        if      (token->type() == CPetBasic::TokenType::STRING) {
          addSpace();

          drawString("\"" + CPetBasic::decodeEmbeddedStr(token->toString()) + "\"",
                     dbg_->stringColor());
        }
        else if (token->type() == CPetBasic::TokenType::KEYWORD) {
          addSpace();

          drawString(token->exprString(), dbg_->keywordColor());

          if (token->toString() != "")
            drawString(" " + token->toString(), dbg_->fgColor());
          else
            needsSpace = true;
        }
        else if (token->type() == CPetBasic::TokenType::OPERATOR) {
          addSpace();

          auto str = token->toString();

          drawString(str, dbg_->operatorColor());

          if (isalpha(str[0]))
            needsSpace = true;
        }
        else {
          auto str = token->toString();

          if (! str.empty() && str[0] != ' ')
            addSpace();

          drawString(token->toString(), dbg_->fgColor());
        }
      }

      ++is;
    }

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
  auto *basic = dbg_->basic();

  QFontMetrics fm(font());

  tw_ = fm.horizontalAdvance("X");
  th_ = fm.height();

  auto nl = basic->numLines();

  return QSize(maxLineLen_*tw_, nl*th_);
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
