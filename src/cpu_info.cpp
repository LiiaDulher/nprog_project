//
// Created by liiadulher on 08.11.21.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "functions.h"
#include "cpu_info.h"
#include "exceptions/cpu_info_exception.h"

CpuInfo::CpuInfo() {
    cpu_n = 0;
}

void CpuInfo::find_cpu_number() {
    char filename[] = "/proc/cpuinfo";
    std::string line;
    std::ifstream cpu_info_file (filename);
    if (cpu_info_file.is_open()) {
        while (getline(cpu_info_file, line)) {
            if (line.rfind("core id", 0) == 0){
                cpu_n += 1;
            }
        }
        cpu_info_file.close();
    } else {
        throw CpuInfoException();
    }
}

void CpuInfo::find_cpu_usage() {
    std::string cpu_name = "cpu";
    std::vector<int64_t> del_times = get_del_cpu_times(cpu_name);
    int64_t usage = del_times[0] - del_times[1];
    cpu_usage = float(usage) / float(del_times[0]) * 100.0;
    user_usage = float(del_times[3]) / float(usage) * 100.0;
    system_usage = float(del_times[2])/float(usage) * 100.0;
}
