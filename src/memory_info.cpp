//
// Created by liiadulher on 08.11.21.
//

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "memory_info.h"
#include "exceptions/cpu_info_exception.h"
#include "functions.h"


MemoryInfo::MemoryInfo() {

}

void MemoryInfo::get_memory_info(){
    char filename[] = "/proc/meminfo";
    std::map<std::string, int64_t> mem_info;
    std::string line;
    std::ifstream memory_info_file (filename);
    if (memory_info_file.is_open()) {
        while (getline(memory_info_file, line)) {
            std::vector<std::string> line_info = line_splitter(line);
            std::string field_name = line_info[0].substr(0, line_info[0].size()-1);
            char* pEnd;
            int64_t number = std::strtoll(line_info[1].c_str(), &pEnd, 10);
            mem_info[field_name] = number;
        }
        memory_info_file.close();
    } else {
        throw CpuInfoException();
    }
    memory_total = mem_info["MemTotal"];
    memory_free = mem_info["MemFree"];
    shared = mem_info["Shmem"];
    available = mem_info["MemAvailable"];
    swap_total = mem_info["SwapTotal"];
    swap_free = mem_info["SwapFree"];
    swap_used = swap_total - swap_free;
    buff_cache = mem_info["Buffers"] + mem_info["Cached"] + mem_info["Slab"];
    memory_used = memory_total - buff_cache -memory_free;
}

std::string MemoryInfo::memory_info_string() {
    std::string used_mem_per = std::to_string(float(memory_used)/memory_total*100);
    auto pos = used_mem_per.find('.');
    if (pos != std::string::npos){
        used_mem_per = used_mem_per.substr(0, used_mem_per.size()-pos-2);
    }
    std::string free_mem_per = std::to_string(float(memory_free)/memory_total*100);
    pos = free_mem_per.find('.');
    if (pos != std::string::npos){
        free_mem_per = free_mem_per.substr(0, free_mem_per.size()-pos-2);
    }
    std::string avil_mem_per = std::to_string(float(available)/memory_total*100);
    pos = avil_mem_per.find('.');
    if (pos != std::string::npos){
        avil_mem_per = avil_mem_per.substr(0, avil_mem_per.size()-pos-2);
    }
    std::string str = "Memory information(in Mb):\n";
    str += "\tmemory:\n";
    str += "\t\ttotal memory: " + std::to_string(float(memory_total)/1024);
    str += "\n\t\tused memory:  " + std::to_string(float(memory_used)/1024) + " (" + used_mem_per + "%)";
    str += "\n\t\tfree memory:  " + std::to_string(float(memory_free)/1024) + " (" + free_mem_per + "%)";
    str += "\n\tavailable memory: " + std::to_string(float(available)/1024) + " (" + avil_mem_per + "%)";
    str += "\n\tshared memory:    " + std::to_string(float(shared)/1024);
    str += "\n\tbuff&cache:       " + std::to_string(float(buff_cache)/1024);
    str += "\n\tswap space:\n";
    str += "\t\ttotal swap space: " + std::to_string(float(swap_total)/1024);
    str += "\n\t\tused swap space:  " + std::to_string(float(swap_used)/1024);
    str += "\n\t\tfree swap space:  " + std::to_string(float(swap_free)/1024);
    str += "\n";
    return str;
}
