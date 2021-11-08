//
// Created by liiadulher on 08.11.21.
//

#ifndef NPROG_PROJECT_CPU_INFO_H
#define NPROG_PROJECT_CPU_INFO_H

#include <cstdint>
#include <vector>

class CpuInfo {
public:
    CpuInfo();
    ~CpuInfo() = default;
    [[nodiscard]] int get_cpu_n() const {return cpu_n;};
    [[nodiscard]] float get_cpu_usage() const {return cpu_usage;};
    [[nodiscard]] float get_cpu_user_usage() const {return user_usage;};
    [[nodiscard]] float get_cpu_system_usage() const {return system_usage;};
    void find_cpu_number();
    void find_cpu_usage();
private:
    int cpu_n;
    float cpu_usage;
    float user_usage;
    float system_usage;
};

#endif //NPROG_PROJECT_CPU_INFO_H
