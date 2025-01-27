#include <cstddef>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor &System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process> &System::Processes() { return processes_; }

/**
 * @brief Return kernel version provided by the 
 * LinuxParser API 
 * calls LinuxParser::Kernel()
 * @return {string}  : The kernel version as a string.
 */
std::string System::Kernel() 
{ 
    return LinuxParser::Kernel(); 
}

/**
 * @brief Returns the system's memory utilization.
 * Calls LinuxParser::MemoryUtilization()
 * @return {float}  : memory utilization as a float
 */
float System::MemoryUtilization() 
{ 
    return LinuxParser::MemoryUtilization( System::memoryUtilData_); 
}

/**
 * @brief Return the operating system name provided by the 
 * LinuxParser API
 * 
 * @return {string} : The operating system name as a string.
 */
std::string System::OperatingSystem() 
{ 
    return LinuxParser::OperatingSystem(); 
}

/**
 * @brief This function returns the number of running processes
 * by calling LinuxParser::RunningProcesses()
 * 
 * @return {int} number of running processes 
 */
int System::RunningProcesses() 
{ 
    return LinuxParser::RunningProcesses(); 
}

/**
 * @brief Return the total number of processes by calling
 * LinuxParser::TotalProcesses()
 * 
 * @return {int} : The total number of processes as an integer. 
 */
int System::TotalProcesses() 
{ 
    return LinuxParser::TotalProcesses(); 
}

/**
 * @brief Return the system uptime by calling LinuxParser::UpTime()
 * 
 * @return {long int} : The system uptime in seconds. 
 */
long int System::UpTime() 
{ 
    return LinuxParser::UpTime(); 
}
