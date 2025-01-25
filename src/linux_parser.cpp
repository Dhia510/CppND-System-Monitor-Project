#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

static bool LinuxParser::isNumber(const std::string& str) {
    for (char c : str) {
        if (!std::isdigit(c)) return false;
    }
    return !str.empty();
}

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

/**
 * @brief Reads the version file in proc directory and 
 * retrives the kernel version of the operating system.
 * The version file contains only one line and the 
 * kernel version is the third string in this line.
 * @return {string}  : The kernel version as a string.
 */
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

/**
 * @brief Computes memory utilization based on the data available in
 * the /proc/meminfo file
 * This function reads the file and extracts useful data and updates 
 * memoryUtilData members, then returns the memory utilization in 
 * fraction using the following formula
 * --------------------------------------------------
 * |               Memory Utilization               |
 * |------------------------------------------------|
 * | Formula:                                       |
 * |   Used Memory = MemTotal - MemFree             |
 * |   Memory Utilization = (Used Memory / MemTotal)|
 * --------------------------------------------------
 * @note the return value is converted to percent before display in 
 * NCursesDisplay::ProgressBar
 * @param memoryUtilData 
 * @return float (fraction of total used memory)
 */
float LinuxParser::MemoryUtilization(MemoryUtilData_t &memoryUtilData) 
{ 
  string key;
  string value;
  string line;
  vector<int> memValues;
  /* Create an input file stream from file meminfo containing memory util data*/
  std::ifstream memInfoStream(kProcDirectory + kMeminfoFilename);

  /* Check if the stream is opened successfully */
  if (memInfoStream.is_open())
  {
    /* iterate threw the 4 first lines for MemTotal, MemFree, MemAvailable and Buffers*/
    for (int l_idx = 0; l_idx < 4; l_idx++)
    {
      /* Read line and store it in line variable */
      if (std::getline(memInfoStream, line))
      {
        /* Create input string stream from the line we just read */
        std::istringstream linestream(line);
        /* Extract key and value */
        linestream >> key >> value;
        /* Check if the value is a number */
        if (isNumber(value))
        {
          /* Push it in the memValues vector */
          memValues.push_back(stoi(value));
        }
        else
        {
          std::cout << "LinuxParser::MemoryUtilization:: Error reading value in line " << l_idx << std::endl;
        }
      }
      else
      {
        std::cout << "LinuxParser::MemoryUtilization:: Error reading line " << l_idx << " from file\n";
      }
    }
  }
  else
  {
    std::cout << "LinuxParser::MemoryUtilization:: Error opening input stream from file\n";
  }

  /* Parse extracted values in a structure */
  memoryUtilData.MEM_TOTAL = memValues[0];
  memoryUtilData.MEM_FREE = memValues[1];
  memoryUtilData.MEM_AVAILABLE = memValues[2];
  memoryUtilData.MEM_BUFFERS = memValues[3];

  /* Return memory ulitzation in percent */
  return float((memoryUtilData.MEM_TOTAL - memoryUtilData.MEM_FREE) / memoryUtilData.MEM_TOTAL); 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { return 0; }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return 0; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
