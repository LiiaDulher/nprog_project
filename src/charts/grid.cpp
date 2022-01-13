#include "grid.h"
#include "waveform.h"
#include "waveformlegend.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>
#include <algorithm>
#include <cmath>

constexpr int leftSpace{ 35 };
constexpr int bottomSpace{ 15 };

constexpr int legentHeight{ 30 };
constexpr size_t maxLegendInRow{ 4 };

Grid::Grid(const QString &name, const QRectF &area) : _area(area), _name(name)
{
    updateWaveformArea();
}

void Grid::registerWaveform(Waveform *waveform) const
{
    connect(this, &Grid::waveformAreaChanged, waveform,
            [waveform](const QRectF &chartArea) { waveform->setArea(chartArea); });
}

void Grid::registerWaveformLegend(WaveformLegend *legend)
{
    _legends.push_back(legend);
    updateWaveformArea();
}

QRectF Grid::waveformArea() const { return _waveformArea; }

void Grid::setWidth(int width)
{
    _area.setWidth(width);
    updateWaveformArea();
}

void Grid::setRangeX(double min, double max)
{
    _minX = min;
    _maxX = max;
}

void Grid::setRangeY(double min, double max)
{
    _minY = min;
    _maxY = max;
}

void Grid::setUnitX(const QString &unit) { _unitX = unit; }

void Grid::setUnitY(const QString &unit) { _unitY = unit; }

void Grid::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // draw name
    painter->drawRect(
        QRectF(_waveformArea.topLeft() - QPointF(1, 1), _waveformArea.size() + QSizeF(2, 2)));

    painter->setPen(QColor(50, 50, 50, 80));

    QFontMetrics fm(painter->font());
    const QRectF txtRect(-(_area.y() + ((_waveformArea.height() + fm.width(_name) + 5) / 2.0)), 5,
                         fm.width(_name) + 5, fm.height() + 2);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->rotate(-90);
    painter->drawText(txtRect, Qt::AlignCenter, _name);
    painter->restore();

    // draw Y min/max values
    const QRectF topUnitRect(_area.topLeft(), QSize(leftSpace, fm.height()));
    const QRectF bottomUnitRect(QPointF(_area.left(), _waveformArea.bottom() - fm.height()),
                                QSize(leftSpace, fm.height()));

    painter->drawText(topUnitRect, Qt::AlignCenter, buildUnitText(_minY, _maxY, 1, false, _unitY));
    painter->drawText(bottomUnitRect, Qt::AlignCenter, buildUnitText(_minY, _maxY, 0, false, _unitY));

    const auto drawHorisontalValues = [this, painter](const QString &txt, double horisontalPos) {
        QFontMetrics fm(painter->font());
        const int textWidth = fm.horizontalAdvance(txt) + 2;
        painter->drawText(QRectF(QPointF(_waveformArea.x() - textWidth / 2
                                             + (_waveformArea.width() * horisontalPos),
                                         _waveformArea.y() + _waveformArea.height() + 5),
                                 QSize(textWidth, 10)),
                          Qt::AlignCenter, txt);
    };
    drawHorisontalValues(buildUnitText(_minX, _maxX, 0, true, _unitX), 0);
    drawHorisontalValues(buildUnitText(_minX, _maxX, 0.25, true, _unitX), 0.25);
    drawHorisontalValues(buildUnitText(_minX, _maxX, 0.5, true, _unitX), 0.5);
    drawHorisontalValues(buildUnitText(_minX, _maxX, 0.75, true, _unitX), 0.75);
    drawHorisontalValues(buildUnitText(_minX, _maxX, 1, true, _unitX), 1);

    painter->setPen(QColor(100, 100, 100, 30));

    const auto drawOy = [this, painter](double horisontalPos) {
        painter->drawLine(QPointF(_waveformArea.x() + (_waveformArea.width() * horisontalPos),
                                  _waveformArea.y()),
                          QPointF(_waveformArea.x() + (_waveformArea.width() * horisontalPos),
                                  _waveformArea.y() + _waveformArea.height()));
    };

    drawOy(0.25);
    drawOy(0.5);
    drawOy(0.75);

    painter->drawLine(QPointF(_waveformArea.x(), _waveformArea.y() + (_waveformArea.height() / 2)),
                      QPointF(_waveformArea.x() + _waveformArea.width(),
                              _waveformArea.y() + (_waveformArea.height() / 2)));
}

QRectF Grid::boundingRect() const
{
    return QRectF(_area.x() - 1, _area.y() - 1, _area.width() + 2, _area.height() + 2);
}

void Grid::updateWaveformArea()
{
    _waveformArea.setLeft(_area.left() + leftSpace);
    _waveformArea.setRight(_area.right() - 1);
    _waveformArea.setTop(_area.top() + 1);
    _waveformArea.setBottom(_area.bottom() - bottomSpace - (legendRows() * legentHeight));

    updateLegendsArea();

    emit waveformAreaChanged(_waveformArea);
}

int Grid::legendRows() const
{
    if (_legends.empty())
        return 0;

    if (_legends.size() % maxLegendInRow == 0)
        return _legends.size() / maxLegendInRow;

    return (_legends.size() / maxLegendInRow) + 1;
}

void Grid::updateLegendsArea()
{
    if (_legends.empty())
        return;

    const size_t rowsWithLegend = legendRows();
    const size_t legendInRow = std::min(maxLegendInRow, _legends.size());

    const QSizeF legendSize(_waveformArea.width() / legendInRow, legentHeight);

    for (size_t i = 0; i < _legends.size(); ++i)
    {
        const QRectF legendRect(QPointF(_waveformArea.x()
                                            + (legendSize.width() * (i % legendInRow)),
                                        _waveformArea.bottom() + bottomSpace
                                            + (legentHeight * (i / maxLegendInRow))),
                                legendSize);
        _legends[i]->setArea(legendRect);
    }
}

QString Grid::buildUnitText(double min, double max, double part, bool reverceEnabled,
                            const QString &unitName)
{
    const auto buildTxt = [&unitName](double value, int digitAfterDot) {
        return QString("%0%1").arg(QString::number(value, 'f', digitAfterDot)).arg(unitName);
    };

    const double value = (max - min) * (reverceEnabled ? 1 - part : part);

    for (int i = 0; i < 5; ++i) // check 4 digits after dot, if it == 0 then return value
        if (std::fabs((value - static_cast<int>(value)) * std::pow(10, i)) < 1)
            return buildTxt(value, i);

    return buildTxt(value, 0);
}
