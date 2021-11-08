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
    std::string str = "Memory information(in Kb):\n";
    str += "\tmemory:\n";
    str += "\t\ttotal memory: " + std::to_string(memory_total);
    str += "\n\t\tused memory:  " + std::to_string(memory_used);
    str += "\n\t\tfree memory:  " + std::to_string(memory_free);
    str += "\n\tavailable memory: " + std::to_string(available);
    str += "\n\tshared memory:    " + std::to_string(shared);
    str += "\n\tbuff&cache:       " + std::to_string(buff_cache);
    str += "\n\tswap space:\n";
    str += "\t\ttotal swap space: " + std::to_string(swap_total);
    str += "\n\t\tused swap space:  " + std::to_string(swap_used);
    str += "\n\t\tfree swap space:  " + std::to_string(swap_free);
    str += "\n";
    return str;
}
