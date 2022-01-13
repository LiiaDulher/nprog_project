#ifndef CPUSNIFFER_H
#define CPUSNIFFER_H

#include "cpu.h"
#include "cpu_info.h"

#include <QMetaType>
#include <QObject>
#include <QTimer>

#include <atomic>
#include <list>

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
    void cpuDataChanged(CpuUsageDetails cpuDetails) const;

private:
    CpuUsageDetails measureCpuUsage(size_t measuteTimeMs) const;

private:
    std::atomic<bool> _isStarted{ false };
};

class CpuUsageUpdater : public QObject
{
    Q_OBJECT
public:
    CpuUsageUpdater(QObject *parent = nullptr);
    void start(int updateInterval);

    void addCpuUsageToQueue(const CpuUsageDetails &cpuUsage);

signals:
    void cpuDataByTimeout(CpuUsageDetails cpuDetails) const;

private:
    void prepareAndSendData() const;

private:
    std::list<CpuUsageDetails> _resultQueue;
    QTimer *_updateDataTimer{ nullptr };
};

#endif // CPUSNIFFER_H
