#include "mainwindow.h"

#include <QApplication>

#include <iostream>
#include <vector>
#include "cpu_info.h"
#include "cpu.h"
#include "memory_info.h"
#include "functions.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
