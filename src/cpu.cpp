//
// Created by liiadulher on 08.11.21.
//

#include <string>
#include "cpu.h"
#include "functions.h"

CPU::CPU(int id) {
    cpu_id = id;
}

void CPU::find_cpu_usage() {
    std::string cpu_name = "cpu" + std::to_string(cpu_id);
    std::vector<int64_t> del_times = get_del_cpu_times(cpu_name);
    int64_t usage = del_times[0] - del_times[1];
    cpu_usage = float(usage) / float(del_times[0]) * 100.0;
    user_usage = float(del_times[3]) / float(usage) * 100.0;
    system_usage = float(del_times[2])/float(usage) * 100.0;
}
