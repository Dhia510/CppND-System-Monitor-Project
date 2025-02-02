#include <cstddef>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"
#include <iostream>

using std::set;
using std::size_t;
using std::string;
using std::vector;

/**
 * @brief returns the system's CPU
 * 
 * @return Processor& 
 */
Processor &System::Cpu() { return cpu_; }

/**
 * @brief reutrns the system's processes ordered by CPU utilization
 * 
 * @return vector<Process>& 
 */
vector<Process> &System::Processes() {
  // Clear and rebuild process list
  processes_.clear();

  // Get current PIDs
  vector<int> pids = LinuxParser::Pids();

  // Create processes
  for (int pid : pids) {
    try {
      processes_.emplace_back(pid);
    } catch (...) {
      continue; // Skip failed process creation
    }
  }

  // Single sort operation
  std::sort(processes_.begin(), processes_.end());

  return processes_;
}

/**
 * @brief Construct a new System:: System object
 * The constructor retrieves the list of process ids
 * and fills the processes_ attributes
 */
System::System() {
  for (int id : LinuxParser::Pids()) {
    processes_.push_back(Process(id));
  }
}
/**
 * @brief Return kernel version provided by the
 * LinuxParser API
 * calls LinuxParser::Kernel()
 * @return {string}  : The kernel version as a string.
 */
std::string System::Kernel() { return LinuxParser::Kernel(); }

/**
 * @brief Returns the system's memory utilization.
 * Calls LinuxParser::MemoryUtilization()
 * @return {float}  : memory utilization as a float
 */
float System::MemoryUtilization() {
  return LinuxParser::MemoryUtilization(System::memoryUtilData_);
}

/**
 * @brief Return the operating system name provided by the
 * LinuxParser API
 *
 * @return {string} : The operating system name as a string.
 */
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

/**
 * @brief This function returns the number of running processes
 * by calling LinuxParser::RunningProcesses()
 *
 * @return {int} number of running processes
 */
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

/**
 * @brief Return the total number of processes by calling
 * LinuxParser::TotalProcesses()
 *
 * @return {int} : The total number of processes as an integer.
 */
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

/**
 * @brief Return the system uptime by calling LinuxParser::UpTime()
 *
 * @return {long int} : The system uptime in seconds.
 */
long int System::UpTime() { return LinuxParser::UpTime(); }
