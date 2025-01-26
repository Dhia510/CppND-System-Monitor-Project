#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {

struct MemoryUtilData_t
{
  float MEM_TOTAL = 0;
  float MEM_FREE = 0;
  float MEM_AVAILABLE = 0;
  float MEM_BUFFERS = 0; 
};

// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

static bool isNumber(const std::string& str);

// System
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
 * @return {float} : fraction of total used memory
 */
float MemoryUtilization(MemoryUtilData_t &memoryUtilData);
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
/**
 * @brief Reads /proc/stat file and extracts the number of running processes 
 *  which is the value to the key "procs_running".
 * 
 * @return {int} : The number of running processes as an integer.
 */
int RunningProcesses();

std::string OperatingSystem();
/**
 * @brief Reads the version file in proc directory and 
 * retrives the kernel version of the operating system.
 * The version file contains only one line and the 
 * kernel version is the third string in this line.
 * @return {string}  : The kernel version as a string.
 */
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
};  // namespace LinuxParser

#endif