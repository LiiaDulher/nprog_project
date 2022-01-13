#include "include/cpusniffer.h"
#include "cpu.h"
#include "cpu_info.h"

#include <QElapsedTimer>

#include <cmath>

static constexpr int ResultsHistoryCount{ 15 };
static constexpr int SingleMeasureTimeMs{ 100 };

CpuSniffer::CpuSniffer(QObject *parent) : QObject(parent) {}

void CpuSniffer::start()
{
    _isStarted = true;

    QElapsedTimer lastUpdateDataTime;
    lastUpdateDataTime.start();

    while (_isStarted)
    {
        emit cpuDataChanged(measureCpuUsage(SingleMeasureTimeMs));
    };
}

void CpuSniffer::stop() { _isStarted = false; }

CpuUsageDetails CpuSniffer::measureCpuUsage(size_t measuteTimeMs) const
{
    CpuInfo cpu_info;
    cpu_info.find_cpu_number();
    cpu_info.find_cpu_usage(measuteTimeMs);

    CpuUsageDetails usageDetails;
    usageDetails.coresCount = cpu_info.get_cpu_n();
    usageDetails.generalUsage = cpu_info.generalCpuInfo();
    usageDetails.coresUsage = cpu_info.coresInfo();
    return usageDetails;
}

/**************************************************************************************************/

CpuUsageUpdater::CpuUsageUpdater(QObject *parent)
{
    _updateDataTimer = new QTimer(this);
    connect(_updateDataTimer, &QTimer::timeout, this, &CpuUsageUpdater::prepareAndSendData);
}

void CpuUsageUpdater::start(int updateInterval) { _updateDataTimer->start(updateInterval); }

void CpuUsageUpdater::addCpuUsageToQueue(const CpuUsageDetails &cpuUsage)
{
    _resultQueue.push_back(cpuUsage);
    if (_resultQueue.size() > ResultsHistoryCount)
        _resultQueue.pop_front();
}

void CpuUsageUpdater::prepareAndSendData() const
{
    if (_resultQueue.empty())
        return;

    auto cpuInfoI = _resultQueue.cend();
    cpuInfoI = std::prev(cpuInfoI);
    CpuUsageDetails cpuUsage = *cpuInfoI;

    if (_resultQueue.size() >= 2)
    {
        size_t measuremantCount{ 1 };
        do
        {
            cpuInfoI = std::prev(cpuInfoI);
            ++measuremantCount;

            cpuUsage.generalUsage.cpu_usage += cpuInfoI->generalUsage.cpu_usage;
            for (size_t coreIndex = 0; coreIndex < cpuUsage.coresCount; ++coreIndex)
                cpuUsage.coresUsage[coreIndex].cpu_usage += cpuInfoI->coresUsage[coreIndex]
                                                                .cpu_usage;

        } while (cpuInfoI != _resultQueue.cbegin());

        const auto recalcUsagePersents = [measuremantCount](int summaryData) {
            return static_cast<int>(
                std::round(summaryData / static_cast<double>(measuremantCount)));
        };

        cpuUsage.generalUsage.cpu_usage = recalcUsagePersents(cpuUsage.generalUsage.cpu_usage);
        for (size_t coreIndex = 0; coreIndex < cpuUsage.coresCount; ++coreIndex)
            cpuUsage.coresUsage[coreIndex].cpu_usage = recalcUsagePersents(
                cpuUsage.coresUsage[coreIndex].cpu_usage);
    }

    emit cpuDataByTimeout(cpuUsage);
}
