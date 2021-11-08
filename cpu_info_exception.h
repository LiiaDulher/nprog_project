//
// Created by liiadulher on 08.11.21.
//

#ifndef NPROG_PROJECT_CPU_INFO_EXCEPTION_H
#define NPROG_PROJECT_CPU_INFO_EXCEPTION_H

class CpuInfoException : public std::exception {
public:
    [[nodiscard]] const char *what() const noexcept override {
        return "Cpu information file is unreachable!";
    }
};


#endif //NPROG_PROJECT_CPU_INFO_EXCEPTION_H
