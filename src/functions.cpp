//
// Created by liiadulher on 08.11.21.
//

#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <iostream>
#include "functions.h"
#include "exceptions/cpu_info_exception.h"

std::vector<std::string> line_splitter(std::string line){
    std::istringstream splited_line(line);
    std::vector<std::string> command_args{};
    do{
        std::string subs;
        splited_line >> subs;
        command_args.push_back(subs);
    } while(splited_line);
    command_args.pop_back();
    return command_args;
}

std::vector<int64_t> get_idle_total_times(std::string cpu_name) {
    char filename[] = "/proc/stat";
    std::string line;
    int64_t total_time = 0;
    int64_t idle_time;
    int64_t user_time;
    int64_t system_time;
    std::ifstream cpu_stat_file (filename);
    if (cpu_stat_file.is_open()) {
        while(getline(cpu_stat_file, line)){
            if (line.rfind(cpu_name, 0) == 0) break;
        }
        std::vector<std::string> times_line = line_splitter(line);
        idle_time = std::stoi(times_line[4]);
        user_time = std::stoi(times_line[1]);
        system_time = std::stoi(times_line[3]);
        for (int i=1; i<times_line.size(); ++i){
            total_time += std::stoi(times_line[i]);
        }
        cpu_stat_file.close();
    } else {
        throw CpuInfoException();
    }
    std::vector<int64_t> times = {total_time, idle_time, system_time, user_time};
    return times;
}

std::vector<int64_t> get_del_cpu_times(std::string cpu_name) {
    auto start_time = std::chrono::system_clock::now();
    std::vector<int64_t> first_times = get_idle_total_times(cpu_name);
    std::this_thread::sleep_until(start_time + std::chrono::milliseconds(1000));
    std::vector<int64_t> second_times = get_idle_total_times(cpu_name);
    int64_t del_total_time = second_times[0] - first_times[0];
    int64_t del_idle_time = second_times[1] - first_times[1];
    int64_t del_user_time = second_times[3] - first_times[3];
    int64_t del_system_time = second_times[2] - first_times[2];
    std::vector<int64_t> del_times = {del_total_time, del_idle_time, del_system_time, del_user_time};
    return del_times;
}

std::string time_repr(int64_t time){
    if (time < 10){
        return "0" + std::to_string(time);
    }
    return std::to_string(time);
}

std::string get_system_uptime(){
    char filename[] = "/proc/uptime";
    std::string line;
    float time;
    std::ifstream system_time_file (filename);
    if (system_time_file.is_open()) {
        getline(system_time_file, line);
        std::string uptime = line_splitter(line)[0];
        time = std::stof(uptime);
        system_time_file.close();
    } else {
        throw CpuInfoException();
    }
    int64_t seconds = static_cast<int64_t>(time);
    int64_t minutes = seconds / 60;
    seconds = seconds % 60;
    int64_t hours = minutes/60;
    minutes = minutes % 60;
    std::string str = std::to_string(hours) + ":" + time_repr(minutes) + ":" + time_repr(seconds);
    return str;
}
