//
// Created by liiadulher on 08.11.21.
//

#include "cpu_info.h"
#include "exceptions/cpu_info_exception.h"
#include "functions.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

CpuInfo::CpuInfo() { cpu_n = 0; }

void CpuInfo::find_cpu_number()
{
    char filename[] = "/proc/cpuinfo";
    std::string line;
    std::ifstream cpu_info_file(filename);
    if (cpu_info_file.is_open())
    {
        while (getline(cpu_info_file, line))
        {
            if (line.rfind("core id", 0) == 0)
            {
                cpu_n += 1;
            }
        }
        cpu_info_file.close();
    }
    else
    {
        throw CpuInfoException();
    }

    _cpuNames.clear();
    _cpuNames.push_back("cpu");
    for (int i = 0; i < cpu_n; ++i)
        _cpuNames.push_back("cpu" + std::to_string(i));
}

void CpuInfo::find_cpu_usage()
{
    auto start_time = std::chrono::system_clock::now();

    std::vector<std::vector<int64_t>> first_times;
    std::vector<std::vector<int64_t>> second_times;

    for (const auto name : _cpuNames)
        first_times.push_back(get_idle_total_times(name));

    std::this_thread::sleep_until(start_time + std::chrono::milliseconds(500));

    for (const auto name : _cpuNames)
        second_times.push_back(get_idle_total_times(name));

    _cores.clear();
    for (int i = 0; i < first_times.size(); ++i)
    {
        int64_t del_total_time = second_times[i][0] - first_times[i][0];
        int64_t del_idle_time = second_times[i][1] - first_times[i][1];
        int64_t del_user_time = second_times[i][3] - first_times[i][3];
        int64_t del_system_time = second_times[i][2] - first_times[i][2];

        CPU coreInfo(i - 1);

        int64_t usage = del_total_time - del_idle_time;
        coreInfo.cpu_usage = float(usage) / float(del_total_time) * 100.0;
        coreInfo.user_usage = float(del_user_time) / float(usage) * 100.0;
        coreInfo.system_usage = float(del_system_time) / float(usage) * 100.0;

        if (i == 0)
            _general = coreInfo;
        else
            _cores.push_back(coreInfo);
    }
}
