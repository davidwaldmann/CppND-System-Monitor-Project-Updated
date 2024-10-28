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
  // Get UId
  string key, value;
  int uid;
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") uid = std::stoi(value);
    }
  }
  // Get Username
  string username, permission, strUid;
  std::ifstream stream1(LinuxParser::kPasswordPath);
  if (stream1.is_open()) {
    while (std::getline(stream1, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> permission >> strUid;
      if (strUid == std::to_string(uid)) {
        user_ = username;
      }
    }
  }
  // get command
  std::ifstream stream2(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kCmdlineFilename);
  if (stream2.is_open()) {
    if (std::getline(stream2, line)) {
      command_ = line;
    }
  }
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return 0; }

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

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
  return Process::CpuUtilization() < a.CpuUtilization();
}