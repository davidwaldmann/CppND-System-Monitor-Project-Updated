#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  user_ = LinuxParser::User(pid);
  command_ = LinuxParser::Command(pid);
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
  long seconds = LinuxParser::UpTime(pid_);
  long totaltime = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  return float(totaltime) / float(seconds);
}

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() {
  string ram = LinuxParser::Ram(pid_);
  ram_ = stoi(ram);
  return ram;
}

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
  string line, tmp, up_time;

  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid_) + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 21; i++)
      {
        linestream >> tmp;
      }
      linestream >> up_time;
    }
  }
  try { stol(up_time); }
  catch(const std::exception& e) {
    // std::cerr << e.what() << '\n';
    return -1;
  }

  return stol(up_time) / sysconf(_SC_CLK_TCK);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}