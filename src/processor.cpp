#include <string>
#include <thread>
#include <chrono>

#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  // First measurement
  long active_time0 = LinuxParser::ActiveJiffies();
  long total_time0 = LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
  
  // wait for 1 second
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Second measurement
  long active_time1 = LinuxParser::ActiveJiffies();
  long total_time1 = LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();

  float activeness = float(active_time1 - active_time0) / float(total_time1 - total_time0);
  return activeness;
}