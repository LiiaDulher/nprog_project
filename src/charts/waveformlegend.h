#pragma once

#include <QFont>
#include <QGraphicsItem>
#include <QPen>
#include <QPropertyAnimation>
#include <list>

class WaveformLegend : public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    WaveformLegend(const QString &description);

    void setArea(const QRectF &rect);
    void setColor(const QColor &color);
    void setDescription(const QString &description);
    void setCurrentValueText(const QString &currentValue);
    void setCurrentValuePersents(size_t percents);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    QRectF boundingRect() const override;

private:
    QRectF _area;
    QColor _color;
    QString _description;
    QString _currentValue;
};
