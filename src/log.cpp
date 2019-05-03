#include "log.h"
#include <cstdlib>
#include <iostream>

int logError(std::string message) {
    std::cerr << "Error: " << message << std::endl;
    system("Pause");
    exit(-1);
}
