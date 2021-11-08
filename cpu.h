//
// Created by liiadulher on 08.11.21.
//

#ifndef NPROG_PROJECT_CPU_H
#define NPROG_PROJECT_CPU_H


class CPU {
public:
    CPU(int id);
    ~CPU() = default;
    [[nodiscard]] int get_cpu_id() const {return cpu_id;};
    [[nodiscard]] float get_cpu_usage() const {return cpu_usage;};
    [[nodiscard]] float get_cpu_user_usage() const {return user_usage;};
    [[nodiscard]] float get_cpu_system_usage() const {return system_usage;};
    void find_cpu_usage();
private:
    int cpu_id;
    float cpu_usage;
    float user_usage;
    float system_usage;
};


#endif //NPROG_PROJECT_CPU_H
