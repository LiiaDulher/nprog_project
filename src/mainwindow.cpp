#include "include/mainwindow.h"
#include "charts/grid.h"
#include "charts/waveform.h"
#include "charts/waveformlegend.h"
#include "cpu.h"
#include "cpu_info.h"
#include "functions.h"
#include "memory_info.h"
#include "ui_mainwindow.h"

#include <QFrame>
#include <QLabel>
#include <QThread>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>

#include <QGraphicsLineItem>
#include <QGraphicsScene>

#include <cmath>
#include <vector>

constexpr size_t pointsOnWaveform{ 600 };
constexpr int updateIntervalMs{ 100 };

static QString fromDouble(double val)
{
    if (std::isnan(val))
        val = 0;

    return QString::number(val, 'f', 2);
}

CpuCoreInfoWidget::CpuCoreInfoWidget(QWidget *parent) : QWidget(parent)
{
    auto layout = new QVBoxLayout;

    //    layout->setSpacing(0);
    layout->setMargin(0);

    _usageLabel = new QLabel;
    _userUsageLabel = new QLabel;
    _systemUsageLabel = new QLabel;
    layout->addWidget(_usageLabel);
    layout->addWidget(_userUsageLabel);
    layout->addWidget(_systemUsageLabel);

    setLayout(layout);

    updateView();
}

void CpuCoreInfoWidget::updateCpuCoreInfo(const CPU &cpu)
{
    _cpuCoreInfo = cpu;
    updateView();
}

void CpuCoreInfoWidget::updateView()
{
    // cpu1 usage: 5.10%
    _usageLabel->setText(QString("Core %0 usage: %1%")
                             .arg(_cpuCoreInfo.cpu_id)
                             .arg(fromDouble(_cpuCoreInfo.cpu_usage)));

    _userUsageLabel->setText(QString("Core %0 user usage: %1%")
                                 .arg(_cpuCoreInfo.cpu_id)
                                 .arg(fromDouble(_cpuCoreInfo.user_usage)));

    _systemUsageLabel->setText(QString("Core %0 system usage: %1%")
                                   .arg(_cpuCoreInfo.cpu_id)
                                   .arg(fromDouble(_cpuCoreInfo.system_usage)));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUi();
    ui->tabWidget->setCurrentIndex(1);

    updateView(CpuUsageDetails());

    qRegisterMetaType<CpuUsageDetails>("CpuUsageDetails");

    _cpuSniffer = new CpuSniffer(this);
    _cpuUpdater = new CpuUsageUpdater(this);
    connect(_cpuSniffer, &CpuSniffer::cpuDataChanged, _cpuUpdater,
            &CpuUsageUpdater::addCpuUsageToQueue, Qt::QueuedConnection);
    connect(
        _cpuUpdater, &CpuUsageUpdater::cpuDataByTimeout, this,
        [this](CpuUsageDetails cpuUsage) { updateView(cpuUsage); }, Qt::QueuedConnection);

    QtConcurrent::run(_cpuSniffer, &CpuSniffer::start);
    _cpuUpdater->start(updateIntervalMs);
}

void MainWindow::setupUi()
{
    auto layout = new QVBoxLayout;

    CpuInfo cpu_info;
    cpu_info.find_cpu_number();

    for (int i = 0; i < cpu_info.get_cpu_n(); ++i)
    {
        _coresInfo.push_back(new CpuCoreInfoWidget(this));
        layout->addWidget(_coresInfo.back());

        if (i < cpu_info.get_cpu_n() - 1)
        {
            auto *line = new QFrame(this);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            layout->addWidget(line);
        }
    }

    ui->coreUsage->setLayout(layout);

    setupCharts();
}

void MainWindow::setupCharts()
{
    const auto getTextItem = [](const QString text, const QPointF &pos) {
        auto *txt = new QGraphicsTextItem(text);
        txt->setPos(pos);
        QFont font;
        font.setBold(true);
        font.setPointSize(18);
        txt->setFont(font);
        return txt;
    };

    _chartScene = new QGraphicsScene(this);

    _chartView = new ChartsGraphicsView(this);
    _chartView->setRenderHint(QPainter::Antialiasing);
    _chartView->setScene(_chartScene);
    _chartView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    connect(_chartView, &ChartsGraphicsView::sizeChanged, _chartView, [this](QSize size) {
        _chartScene->setSceneRect(_chartScene->itemsBoundingRect() /*QRectF({ 0, 0 }, size)*/);
    });

    ui->chartsWidget->setLayout(new QVBoxLayout);
    ui->chartsWidget->layout()->addWidget(_chartView);

    _chartScene->addItem(getTextItem("CPU History", QPointF(0, 0)));
    auto *grid = new Grid("CPU %", QRectF(0, 40, 800, 150));
    connect(_chartView, &ChartsGraphicsView::sizeChanged, grid,
            [grid](QSize size) { grid->setWidth(size.width() - 10); });
    _chartScene->addItem(grid);

    const double minPointValue{ 0 };
    const double maxPointValue{ 100 };
    const std::vector<QColor> colorList{
        Qt::gray,       Qt::red,     Qt::green,     Qt::blue,     Qt::cyan,     Qt::magenta,
        Qt::yellow,     Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta,
        Qt::darkYellow, Qt::black,   Qt::darkGray,  Qt::color1
    };

    WaveformLegend *cpuWaveformLegend = new WaveformLegend("CPU");
    _chartScene->addItem(cpuWaveformLegend);

    _cpuWaveform = new Waveform(pointsOnWaveform, grid->waveformArea());
    _cpuWaveform->setMinMaxY(minPointValue, maxPointValue);
    _cpuWaveform->setColor(QColor(Qt::red));
    _cpuWaveform->setDrawMode(WaveformDrawMode::Waveform);
    _cpuWaveform->setLegend(cpuWaveformLegend);
    _chartScene->addItem(_cpuWaveform);
    grid->registerWaveform(_cpuWaveform);
    grid->registerWaveformLegend(cpuWaveformLegend);

    _chartScene->addItem(getTextItem("Cores History", QPointF(0, 200)));
    auto *gridCores = new Grid("Cores %", QRectF(0, 240, 800, 180));
    connect(_chartView, &ChartsGraphicsView::sizeChanged, grid,
            [gridCores](QSize size) { gridCores->setWidth(size.width() - 10); });
    _chartScene->addItem(gridCores);

    CpuInfo cpu_info;
    cpu_info.find_cpu_number();
    for (int i = 0; i < cpu_info.get_cpu_n(); ++i)
    {
        WaveformLegend *waveformLegend = new WaveformLegend(QString("CPU %0").arg(i));
        _chartScene->addItem(waveformLegend);

        auto waveform = new Waveform(pointsOnWaveform, gridCores->waveformArea());
        gridCores->registerWaveform(waveform);
        waveform->setMinMaxY(minPointValue, maxPointValue);
        waveform->setColor(colorList[i % colorList.size()]);
        waveform->setDrawMode(WaveformDrawMode::Waveform);
        waveform->setLegend(waveformLegend);
        _chartScene->addItem(waveform);
        gridCores->registerWaveformLegend(waveformLegend);

        _coreWaveforms.push_back(waveform);
    }

    _chartScene->addItem(getTextItem("Memory History", QPointF(0, 430)));
    auto *gridMem = new Grid("Memory %", QRectF(0, 470, 800, 150));
    connect(_chartView, &ChartsGraphicsView::sizeChanged, gridMem,
            [gridMem](QSize size) { gridMem->setWidth(size.width() - 10); });
    _chartScene->addItem(gridMem);

    WaveformLegend *memWaveformLegend = new WaveformLegend(QString("Used Memory"));
    _chartScene->addItem(memWaveformLegend);

    _memWaveform = new Waveform(pointsOnWaveform, grid->waveformArea());
    _memWaveform->setMinMaxY(minPointValue, maxPointValue);
    _memWaveform->setColor(QColor(0, 255, 0, 30));
    _memWaveform->setDrawMode(WaveformDrawMode::Histogram);
    _memWaveform->setLegend(memWaveformLegend);
    _chartScene->addItem(_memWaveform);
    gridMem->registerWaveform(_memWaveform);
    gridMem->registerWaveformLegend(memWaveformLegend);

    WaveformLegend *swapWaveformLegend = new WaveformLegend(QString("Used Swap"));
    _chartScene->addItem(swapWaveformLegend);

    _swapWaveform = new Waveform(pointsOnWaveform, grid->waveformArea());
    _swapWaveform->setMinMaxY(minPointValue, maxPointValue);
    _swapWaveform->setColor(QColor(255, 0, 0, 30));
    _swapWaveform->setDrawMode(WaveformDrawMode::Histogram);
    _swapWaveform->setLegend(swapWaveformLegend);
    _chartScene->addItem(_swapWaveform);
    gridMem->registerWaveform(_swapWaveform);
    gridMem->registerWaveformLegend(swapWaveformLegend);

    //    _coreWaveforms[0]->addPoint(0);
    //    _coreWaveforms[0]->addPoint(0);
    //    _coreWaveforms[0]->addPoint(100);
    //    _coreWaveforms[0]->addPoint(100);

    //    _cpuWaveform->addPoint(0);
    //    _cpuWaveform->addPoint(0);
    //    _cpuWaveform->addPoint(100);
    //    _cpuWaveform->addPoint(100);
}

MainWindow::~MainWindow()
{
    _cpuSniffer->stop();
    delete ui;
}

void MainWindow::updateView(const CpuUsageDetails &cpuUsage)
{
    _cpuWaveform->addPoint(cpuUsage.generalUsage.cpu_usage);

    ui->labelCoreCount->setText(QString("Number of cores: %1").arg(cpuUsage.coresCount));

    ui->labelAvrCpu->setText(
        QString("Average CPU usage: %1%").arg(fromDouble(cpuUsage.generalUsage.cpu_usage)));
    ui->labelAvrCpuUser->setText(
        QString("Average CPU user usage: %1%").arg(fromDouble(cpuUsage.generalUsage.user_usage)));
    ui->labelAvrsystem->setText(QString("Average CPU system usage: %1%")
                                    .arg(fromDouble(cpuUsage.generalUsage.system_usage)));

    for (int i = 0; i < cpuUsage.coresUsage.size(); ++i)
    {
        if (i < _coresInfo.size())
            _coresInfo[i]->updateCpuCoreInfo(cpuUsage.coresUsage[i]);

        if (i < _coreWaveforms.size())
            _coreWaveforms[i]->addPoint(cpuUsage.coresUsage[i].cpu_usage);
    }

    MemoryInfo memory;
    memory.get_memory_info();

    _memWaveform->addPoint(std::round((memory.get_memory_used()
                                       / static_cast<double>(memory.get_memory_total()))
                                      * 100),
                           false);
    _swapWaveform->addPoint(std::round((memory.get_swap_used()
                                        / static_cast<double>(memory.get_swap_total()))
                                       * 100),
                            false);

    _memWaveform->legend()->setCurrentValueText(
        QString("Used %1 GB").arg(fromDouble(memory.get_memory_used() / 1000.0)));
    _swapWaveform->legend()->setCurrentValueText(
        QString("Used %1 GB").arg(fromDouble(memory.get_swap_used() / 1000.0)));

    ui->labelMemTotal->setText((QString("Total memory: %1").arg(memory.get_memory_total())));
    ui->labelMemToUsed->setText((QString("Used memory: %1").arg(memory.get_memory_used())));
    ui->labelMemFree->setText((QString("Free memory: %1").arg(memory.get_memory_free())));

    ui->labelSwapTotal->setText((QString("Total swap space: %1").arg(memory.get_swap_total())));
    ui->labelSwapUsed->setText((QString("Used swap space: %1").arg(memory.get_swap_used())));
    ui->labelSwapTFree->setText((QString("Free swap space: %1").arg(memory.get_swap_free())));
}
