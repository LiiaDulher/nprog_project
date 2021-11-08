//
// Created by liiadulher on 08.11.21.
//

#ifndef NPROG_PROJECT_MEMORY_INFO_H
#define NPROG_PROJECT_MEMORY_INFO_H


#include <string>

class MemoryInfo {
public:
    MemoryInfo();
    ~MemoryInfo() = default;
    void get_memory_info();
    std::string memory_info_string();
private:
    int64_t memory_total;
    int64_t memory_used;
    int64_t memory_free;
    int64_t available;
    int64_t shared;
    int64_t buff_cache;
    int64_t swap_total;
    int64_t swap_used;
    int64_t swap_free;
};


#endif //NPROG_PROJECT_MEMORY_INFO_H
