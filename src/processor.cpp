#include <string>
#include <thread>
#include <chrono>

#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::string key, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::string line;

  // First measurement
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    }
  }
  // active time = sum of all times - idle time
  int active_time0 = std::stoi(user) + std::stoi(nice) + std::stoi(system) + std::stoi(iowait) + std::stoi(irq) + std::stoi(softirq) + std::stoi(steal) + std::stoi(guest) + std::stoi(guest_nice);
  int total_time0 = active_time0 + std::stoi(idle);
  
  // wait for 1 second
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Second measurement
  std::ifstream stream1(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (stream1.is_open()) {
    if (std::getline(stream1, line)) {
      std::istringstream linestream(line);
      linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    }
  }

  // active time = sum of all times - idle time
  int active_time1 = std::stoi(user) + std::stoi(nice) + std::stoi(system) + std::stoi(iowait) + std::stoi(irq) + std::stoi(softirq) + std::stoi(steal) + std::stoi(guest) + std::stoi(guest_nice);
  int total_time1 = active_time1 + std::stoi(idle);

  float activeness = float(active_time1 - active_time0) / float(total_time1 - total_time0);

  return activeness;
}