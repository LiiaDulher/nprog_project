#pragma once

#include <QFont>
#include <QGraphicsItem>
#include <QPen>
#include <QPropertyAnimation>

class Grid : public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Grid(const QString &name = "", const QRectF &rect = QRectF(0, 0, 640, 480));

    void registerWaveform(class Waveform *waveform) const;
    void registerWaveformLegend(class WaveformLegend *legend);

    QRectF waveformArea() const;

    void setWidth(int width);
    void setRangeX(double min, double max);
    void setRangeY(double min, double max);
    void setUnitX(const QString &unit);
    void setUnitY(const QString &unit);

signals:
    void waveformAreaChanged(const QRectF &chartArea) const;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    QRectF boundingRect() const override;
    void updateWaveformArea();

    int legendRows() const;
    int calcLeftSpace() const;
    void updateLegendsArea();

    static QString buildUnitText(double min, double max, double part, bool reverceEnabled,
                                 const QString &unitName);

private:
    QRectF _area;
    QRectF _waveformArea;
    QString _name;

    double _minY{ 0.0 };
    double _maxY{ 100.0 };
    double _minX{ 0.0 };
    double _maxX{ 60.0 };
    QString _unitX{ "S" };
    QString _unitY{ "" };

    std::vector<WaveformLegend *> _legends;
};
