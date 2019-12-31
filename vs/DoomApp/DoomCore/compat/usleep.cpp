#include "usleep.h"

#include <chrono>
#include <thread>

void usleep(unsigned int usec) {
    // https://gist.github.com/ngryman/6482577
    std::this_thread::sleep_for(std::chrono::microseconds(usec));
}