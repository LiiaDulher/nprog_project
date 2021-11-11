#include <iostream>
#include <vector>
#include "cpu_info.h"
#include "cpu.h"
#include "memory_info.h"
#include "functions.h"

int main() {
    CpuInfo cpu_info;
    cpu_info.find_cpu_number();
    printf("Number of cores: %d\n", cpu_info.get_cpu_n());
    std::cout << std::endl;
    cpu_info.find_cpu_usage();
    printf("Average CPU usage: %.2f%c\n", cpu_info.get_cpu_usage(), '%');
    printf("Average CPU user usage: %.2f%c\n", cpu_info.get_cpu_user_usage(), '%');
    printf("Average CPU system usage: %.2f%c\n", cpu_info.get_cpu_system_usage(), '%');
    std::cout << std::endl;
              std::vector<CPU> cpus;
    for (int i=0; i < cpu_info.get_cpu_n(); ++i){
        cpus.push_back(CPU(i));
    }
    for (int i=0; i < cpu_info.get_cpu_n(); ++i){
        CPU cur_cpu = cpus[i];
        cur_cpu.find_cpu_usage();
        printf("cpu%d usage: %.2f%c\n", cur_cpu.get_cpu_id()+1, cur_cpu.get_cpu_usage(), '%');
        printf("cpu%d user usage: %.2f%c\n", cur_cpu.get_cpu_id()+1, cpu_info.get_cpu_user_usage(), '%');
        printf("cpu%d system usage: %.2f%c\n", cur_cpu.get_cpu_id()+1, cpu_info.get_cpu_system_usage(), '%');
        std::cout << std::endl;
    }
    MemoryInfo memory;
    memory.get_memory_info();
    std::cout << memory.memory_info_string();
    std::cout << std::endl;
    std::cout << "System uptime: " << get_system_uptime() << std::endl;
    return 0;
}
