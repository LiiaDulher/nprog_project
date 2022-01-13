#pragma once

#include "waveformlegend.h"

#include <QFont>
#include <QGraphicsItem>
#include <QPen>
#include <QPropertyAnimation>
#include <list>

enum class WaveformDrawMode
{
    Waveform,
    Histogram
};

class Waveform : public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Waveform(size_t pointsCount, const QRectF &rect);
    void addPoint(double yValue, bool updateLegend = true);
    void setArea(const QRectF &rect);

    void setColor(const QColor &color);
    void setMinMaxY(double minY, double maxY);

    void setDrawMode(WaveformDrawMode mode);

    void setLegend(WaveformLegend *legend);
    WaveformLegend *legend() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    QRectF boundingRect() const override;

private:
    void drawAsWaveform(QPainter *painter);
    void drawAsHistogram(QPainter *painter);
    double calcYCoordinate(double pointY) const;

private:
    QRectF _area;
    size_t _pointsCount{ 0 };
    std::list<double> _points;
    double _minY{ 0 };
    double _maxY{ 0 };

    QColor _color;

    WaveformDrawMode _drawMode{ WaveformDrawMode::Waveform };
    WaveformLegend *_legend{ nullptr };
};
