#include "waveformlegend.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QPainter>
#include <algorithm>
#include <cmath>

#include <QDebug>

WaveformLegend::WaveformLegend(const QString &description) : _description(description) {}

void WaveformLegend::setArea(const QRectF &rect)
{
    _area = rect;
    update(_area);
}

void WaveformLegend::setColor(const QColor &color)
{
    _color = color;
    update(_area);
}

void WaveformLegend::setDescription(const QString &description)
{
    _description = description;
    update(_area);
}

void WaveformLegend::setCurrentValueText(const QString &currentValue)
{
    _currentValue = currentValue;
    update(_area);
}

void WaveformLegend::setCurrentValuePersents(size_t percents)
{
    _currentValue = QString("%0 %").arg(QString::number(static_cast<int>(percents)));
    update(_area);
}

void WaveformLegend::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    const QSizeF colorRectSizeP(25, 15);
    const QPointF topLeft(_area.topLeft()
                          + QPointF(0, ((_area.height() - colorRectSizeP.height()) / 2)));

    const QRectF waveformColorRect(topLeft, colorRectSizeP);
    const QRectF textRect(topLeft + QPointF(30, 0), _area.size() - colorRectSizeP);

    painter->drawText(textRect, Qt::AlignLeft, _description + "   " + _currentValue);

    painter->setBackground(QBrush(_color));
    painter->setPen(_color);
    painter->drawRect(waveformColorRect);
    painter->fillRect(waveformColorRect, _color);
}

QRectF WaveformLegend::boundingRect() const { return _area; }
