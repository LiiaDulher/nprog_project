#include "include/mainwindow.h"
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

#include <cmath>
#include <vector>

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

CpuSniffer::CpuSniffer(QObject *parent) : QObject(parent) {}

void CpuSniffer::start()
{
    _isStarted = true;
    while (_isStarted)
    {
        CpuUsageDetails usageDetails;

        CpuInfo cpu_info;
        cpu_info.find_cpu_number();
        cpu_info.find_cpu_usage();

        usageDetails.coresCount = cpu_info.get_cpu_n();
        usageDetails.generalUsage = cpu_info.generalCpuInfo();
        usageDetails.coresUsage = cpu_info.coresInfo();

        emit cpuDataChanged(usageDetails);
    }
}

void CpuSniffer::stop() { _isStarted = false; }

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUi();

    updateView(CpuUsageDetails());

    qRegisterMetaType<CpuUsageDetails>("CpuUsageDetails");
    _cpuSniffer = new CpuSniffer();
    QtConcurrent::run(_cpuSniffer, &CpuSniffer::start);
    connect(_cpuSniffer, &CpuSniffer::cpuDataChanged, this,
            [this](CpuUsageDetails cpuUsage) { updateView(cpuUsage); });
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
}

MainWindow::~MainWindow()
{
    _cpuSniffer->stop();
    delete ui;
}

void MainWindow::updateView(const CpuUsageDetails &cpuUsage)
{
    ui->labelCoreCount->setText(QString("Number of cores: %1").arg(cpuUsage.coresCount));

    ui->labelAvrCpu->setText(
        QString("Average CPU usage: %1%").arg(fromDouble(cpuUsage.generalUsage.cpu_usage)));
    ui->labelAvrCpuUser->setText(
        QString("Average CPU user usage: %1%").arg(fromDouble(cpuUsage.generalUsage.user_usage)));
    ui->labelAvrsystem->setText(QString("Average CPU system usage: %1%")
                                    .arg(fromDouble(cpuUsage.generalUsage.system_usage)));

    for (int i = 0; i < cpuUsage.coresUsage.size(); ++i)
        if (i < _coresInfo.size())
            _coresInfo[i]->updateCpuCoreInfo(cpuUsage.coresUsage[i]);

    MemoryInfo memory;
    memory.get_memory_info();

    ui->labelMemTotal->setText((QString("Total memory: %1").arg(memory.get_memory_total())));
    ui->labelMemToUsed->setText((QString("Used memory: %1").arg(memory.get_memory_used())));
    ui->labelMemFree->setText((QString("Free memory: %1").arg(memory.get_memory_free())));

    ui->labelSwapTotal->setText((QString("Total swap space: %1").arg(memory.get_swap_total())));
    ui->labelSwapUsed->setText((QString("Used swap space: %1").arg(memory.get_swap_used())));
    ui->labelSwapTFree->setText((QString("Free swap space: %1").arg(memory.get_swap_free())));
}
