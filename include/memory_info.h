//
// Created by liiadulher on 08.11.21.
//

#ifndef NPROG_PROJECT_MEMORY_INFO_H
#define NPROG_PROJECT_MEMORY_INFO_H

#include <string>

class MemoryInfo
{
public:
    MemoryInfo();
    ~MemoryInfo() = default;
    void get_memory_info();
    std::string memory_info_string();

    int64_t get_memory_total() const { return memory_total / 1024; }
    int64_t get_memory_used() const { return memory_used / 1024; }
    int64_t get_memory_free() const { return memory_free / 1024; }
    int64_t get_available() const { return available / 1024; }
    int64_t get_shared() const { return shared / 1024; }
    int64_t get_buff_cache() const { return buff_cache / 1024; }
    int64_t get_swap_total() const { return swap_total / 1024; }
    int64_t get_swap_used() const { return swap_used / 1024; }
    int64_t get_swap_free() const { return swap_free / 1024; }

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
