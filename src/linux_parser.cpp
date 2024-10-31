#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string key, value;
  int memTotal, memFree, cached, sReclaimable, shmem;
  int total, green, mCached;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  std::istringstream linestream;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      linestream.str(line);
      linestream >> key >> value;
      if (key == "MemTotal:") memTotal = std::stoi(value);
      else if (key == "MemFree:") memFree = std::stoi(value);
      else if (key == "Cached:") cached = std::stoi(value);
      else if (key == "SReclaimable:") sReclaimable = std::stoi(value);
      else if (key == "Shmem:") shmem = std::stoi(value);
    }
  }
  total = memTotal - memFree;
  mCached = cached + sReclaimable - shmem;
  green = total - mCached;
  return (float)green / (float)total;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string strUpTime,strIdleTime;
  string line;
  int upTime = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> strUpTime >> strIdleTime;
      upTime = std::stoi(strUpTime);
    }
  }
  return upTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string time[22];
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 22; i++) {
        linestream >> time[i];
      }
    }
  }
  if (time[0] == "") return -1.0;
  return stol(time[13]) + stol(time[14]) + stol(time[15]) + stol(time[16]);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  return std::stol(jiffies[1]) + std::stol(jiffies[2]) + std::stol(jiffies[3])
             + std::stol(jiffies[6]) + std::stol(jiffies[7]) + std::stol(jiffies[8]) + std::stol(jiffies[9]) + std::stol(jiffies[10]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  return std::stol(jiffies[4]) + std::stol(jiffies[5]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> jiffies;
  std::string line;

  // First measurement
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      // linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
      string jiffy;
      for (int i = 0; i < 11; i++) {
        linestream >> jiffy;
        jiffies.push_back(jiffy);
      }
    }
  }
  return jiffies;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key, value;
  string line;
  int totalProcesses = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        totalProcesses = std::stoi(value);
        break;
      }
    }
  }
  return totalProcesses;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, value;
  string line;
  int procsRunning = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        procsRunning = std::stoi(value);
        break;
      }
    }
  }
  return procsRunning;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream2(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kCmdlineFilename);
  if (stream2.is_open()) {
    if (std::getline(stream2, line)) {
      return line;
    }
  }
  return "---";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string key, value;
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        value.pop_back();
        value.pop_back();
        value.pop_back();
        return value;
      }
    }
  }
  return "---";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string key, value;
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") return value;
    }
  }
  return "---";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  string username, permission, strUid;
  std::ifstream stream1(LinuxParser::kPasswordPath);
  if (stream1.is_open()) {
    while (std::getline(stream1, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> permission >> strUid;
      if (strUid == LinuxParser::Uid(pid)) {
        return username;
      }
    }
  }
  return "---";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  string time[22];
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 22; i++) {
        linestream >> time[i];
      }
    }
  }
  if (time[0] == "") return -1.0;
  return stol(time[21]);
}
