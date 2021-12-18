//
// Created by liiadulher on 08.11.21.
//

#ifndef NPROG_PROJECT_CPU_H
#define NPROG_PROJECT_CPU_H

struct CPU
{
public:
    CPU(int id);
    CPU(const CPU &) = default;
    CPU(CPU &&) = default;
    CPU &operator=(const CPU &) = default;
    CPU &operator=(CPU &&) = default;

    int cpu_id;
    float cpu_usage;
    float user_usage;
    float system_usage;
};

#endif // NPROG_PROJECT_CPU_H
