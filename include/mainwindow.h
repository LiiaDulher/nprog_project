#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cpu.h"
#include "cpu_info.h"

#include <QMainWindow>
#include <QMetaType>
#include <QTimer>

#include <atomic>

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

struct CpuUsageDetails
{
    size_t coresCount{ 0 };
    CPU generalUsage{ -1 };
    std::vector<CPU> coresUsage;
};

Q_DECLARE_METATYPE(CpuUsageDetails);

class CpuSniffer : public QObject
{
    Q_OBJECT
public:
    CpuSniffer(QObject *parent = nullptr);
    void start();
    void stop();

signals:
    void cpuDataChanged(CpuUsageDetails cpuDetails);

private:
    std::atomic<bool> _isStarted{ false };
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

private:
    Ui::MainWindow *ui;
    std::vector<CpuCoreInfoWidget *> _coresInfo;
    //    QTimer _updateTimer;
    CpuSniffer *_cpuSniffer;
};
#endif // MAINWINDOW_H
