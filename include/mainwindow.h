#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cpu.h"
#include "cpu_info.h"
#include "cpusniffer.h"

#include <QGraphicsView>
#include <QMainWindow>
#include <QMetaType>
#include <QResizeEvent>

namespace Ui
{
class MainWindow;
}

class QLabel;

class CpuCoreInfoWidget : public QWidget
{
public:
    CpuCoreInfoWidget(QWidget *parent = nullptr);

    void updateCpuCoreInfo(const CPU &cpu);

private:
    void updateView();

private:
    CPU _cpuCoreInfo{ -1 };
    QLabel *_usageLabel{ nullptr };
    QLabel *_userUsageLabel{ nullptr };
    QLabel *_systemUsageLabel{ nullptr };
};

class ChartsGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    using QGraphicsView::QGraphicsView;

signals:
    void sizeChanged(QSize size) const;

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QGraphicsView::resizeEvent(event);
        emit sizeChanged(event->size());
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateView(const CpuUsageDetails &cpuUsage);

private:
    void setupUi();
    void setupCharts();

private:
    Ui::MainWindow *ui;
    class ChartsGraphicsView *_chartView{ nullptr };
    class QGraphicsScene *_chartScene{ nullptr };
    class Waveform *_cpuWaveform{ nullptr };
    std::vector<class Waveform *> _coreWaveforms;
    class Waveform *_memWaveform{ nullptr };
    class Waveform *_swapWaveform{ nullptr };

    std::vector<CpuCoreInfoWidget *> _coresInfo;
    CpuSniffer *_cpuSniffer{ nullptr };
    CpuUsageUpdater *_cpuUpdater{ nullptr };
};
#endif // MAINWINDOW_H
