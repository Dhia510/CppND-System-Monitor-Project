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
 * @brief Construct a new System:: System object
 * The constructor retrieves the list of process ids
 * and fills the processes_ attributes
 */
  System();
  
/**
 * @brief Returns the system's memory utilization.
 * Calls LinuxParser::MemoryUtilization()
 * @return {float}  : memory utilization as a float
 */
  float MemoryUtilization();       
/**
 * @brief Return the system uptime by calling LinuxParser::UpTime()
 * 
 * @return {long int} : The system uptime in seconds. 
 */
  long int UpTime();                      

/**
 * @brief Return the total number of processes by calling
 * LinuxParser::TotalProcesses()
 * 
 * @return {int} : The total number of processes as an integer. 
 */
  int TotalProcesses();               
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
  /**
   * @brief Return the operating system name provided by the 
   * LinuxParser API
   * 
   * @return {string} : The operating system name as a string.
   */
  std::string OperatingSystem();      

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