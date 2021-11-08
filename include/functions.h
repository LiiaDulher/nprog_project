//
// Created by liiadulher on 08.11.21.
//

#ifndef NPROG_PROJECT_FUNCTIONS_H
#define NPROG_PROJECT_FUNCTIONS_H

#include <vector>
#include <string>

std::vector<std::string> line_splitter(std::string line);
std::vector<int64_t> get_idle_total_times(std::string cpu_name);
std::vector<int64_t> get_del_cpu_times(std::string cpu_name);
std::string get_system_uptime();

#endif //NPROG_PROJECT_FUNCTIONS_H
