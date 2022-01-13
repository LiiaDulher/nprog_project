#include "waveform.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>
#include <algorithm>
#include <cmath>

#include <QDebug>

Waveform::Waveform(size_t pointsCount, const QRectF &rect) : _pointsCount(pointsCount), _area(rect)
{
}

void Waveform::addPoint(double yValue, bool updateLegend)
{
    _points.push_back(yValue);

    if (_points.size() > _pointsCount)
        _points.pop_front();

    if (updateLegend)
        if (_legend)
            _legend->setCurrentValuePersents(static_cast<size_t>(std::round(yValue)));

    update(_area);
}

void Waveform::setArea(const QRectF &rect)
{
    _area = rect;
    update(_area);
}

void Waveform::setColor(const QColor &color)
{
    _color = color;
    if (_legend)
        _legend->setColor(color);
}

void Waveform::setMinMaxY(double minY, double maxY)
{
    _minY = minY;
    _maxY = maxY;
}

void Waveform::setDrawMode(WaveformDrawMode mode) { _drawMode = mode; }

void Waveform::setLegend(WaveformLegend *legend)
{
    _legend = legend;
    if (_legend)
        _legend->setColor(_color);
}

WaveformLegend *Waveform::legend() const { return _legend; }

void Waveform::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (_drawMode == WaveformDrawMode::Waveform)
        drawAsWaveform(painter);
    else if (_drawMode == WaveformDrawMode::Histogram)
        drawAsHistogram(painter);
}

QRectF Waveform::boundingRect() const
{
    return QRectF(_area.x() - 1, _area.y() - 1, _area.width() + 2, _area.height() + 2);
}

void Waveform::drawAsWaveform(QPainter *painter)
{
    painter->setBrush(QBrush(_color));
    painter->setPen(_color);

    if (_points.size() < 2)
        return;

    const double pointWidth = _area.width() / static_cast<double>(_pointsCount - 1);
    double pointX = 0;

    auto pointI = std::prev(_points.cend());
    double prevPointY = 0;

    do
    {
        prevPointY = *pointI;
        pointI = std::prev(pointI);

        const QPointF rightSide(_area.right() - pointX,
                                _area.y() + calcYCoordinate(prevPointY) + 1);
        pointX += pointWidth;
        const QPointF leftSide(_area.right() - pointX, _area.y() + calcYCoordinate(*pointI) + 1);
        painter->drawLine(rightSide, leftSide);

    } while (pointI != _points.cbegin());
}
void Waveform::drawAsHistogram(QPainter *painter)
{
    painter->setBrush(QBrush(_color));
    painter->setPen(_color);

    if (_points.empty())
        return;

    const double pointWidth = _area.width() / static_cast<double>(_pointsCount);
    double pointX = 0;

    auto pointI = _points.cend();
    do
    {
        pointI = std::prev(pointI);
        pointX += pointWidth;
        const QPointF leftTop(_area.right() - pointX, _area.y() + calcYCoordinate(*pointI) + 1);
        QRectF r(leftTop, QPointF(leftTop.x() + pointWidth, _area.bottom()));
        painter->drawRect(QRectF(leftTop, QPointF(leftTop.x() + pointWidth, _area.bottom())));

    } while (pointI != _points.cbegin());
}

double Waveform::calcYCoordinate(double pointY) const
{
    if (pointY < _minY)
        pointY = _minY;
    if (pointY > _maxY)
        pointY = _maxY;

    const double valuePart = pointY / std::abs(_maxY - _minY);
    return (_area.height() - (_area.height() * valuePart)) - 1;
}
