#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "linux_parser.h"
class System {
 public:
  Processor& Cpu();                   // TODO: See src/system.cpp
  std::vector<Process>& Processes();  // TODO: See src/system.cpp
  /**
 * @brief Returns the system's memory utilization.
 * Calls LinuxParser::MemoryUtilization()
 * @return {float}  : memory utilization as a float
 */
  float MemoryUtilization();          
  long UpTime();                      // TODO: See src/system.cpp
  int TotalProcesses();               // TODO: See src/system.cpp
  /**
 * @brief This function returns the number of running processes
 * by calling LinuxParser::RunningProcesses()
 * 
 * @return {int} number of running processes 
 */
  int RunningProcesses();           
  /**
 * @brief Return kernel version provided by the 
 * LinuxParser API 
 * calls LinuxParser::Kernel()
 * @return {string}  : The kernel version as a string.
 */
  std::string Kernel();
  std::string OperatingSystem();      // TODO: See src/system.cpp

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  /**
   * @brief structure for holding memory usage
   * data
   */
  LinuxParser::MemoryUtilData_t memoryUtilData_;
};

#endif