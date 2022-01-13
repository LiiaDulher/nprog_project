//
// Created by liiadulher on 08.11.21.
//

#ifndef NPROG_PROJECT_CPU_INFO_H
#define NPROG_PROJECT_CPU_INFO_H

#include "cpu.h"

#include <cstdint>
#include <string>
#include <vector>

class CpuInfo
{
public:
    CpuInfo();
    ~CpuInfo() = default;
    [[nodiscard]] int get_cpu_n() const { return cpu_n; };
    [[nodiscard]] CPU generalCpuInfo() const { return _general; };
    [[nodiscard]] std::vector<CPU> coresInfo() const { return _cores; };
    void find_cpu_number();
    void find_cpu_usage(size_t measureTimeMs);

private:
    int cpu_n;
    std::vector<std::string> _cpuNames;

    CPU _general{ -1 };
    std::vector<CPU> _cores;
};

#endif // NPROG_PROJECT_CPU_INFO_H
