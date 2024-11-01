#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    int sec = seconds % 60;
    long rest = seconds - sec;
    int min = rest % 3600;
    rest = rest - min;
    min = min / 60;
    int h = rest;
    h = h / 3600;
    string output = std::to_string(h) + ":" + std::to_string(min) + ":" + std::to_string(sec) + "    ";
    return output;
}