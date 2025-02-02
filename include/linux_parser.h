#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {

// Defines
#define KEY_UTIME ("utime")
#define KEY_STIME ("stime")
#define KEY_CUTIME ("cutime")
#define KEY_CSTIME ("cstime")
#define KEY_STARTTIME ("starttime")
#define UTIME_IDX (13)
#define STIME_IDX (14)
#define CUTIME_IDX (15)
#define CSTIME_IDX (16)
#define STARTTIME_IDX (21)

struct MemoryUtilData_t {
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
/**
 * @brief Extracts the system uptime from the /proc/uptime file
 * This file contains two numbers (values in seconds): the uptime
 * of the system (including time spent in suspend) and the amount
 * of time spent in the idle process.
 *
 * @return {long int} : The system uptime in seconds.
 */
long int UpTime();
std::vector<int> Pids();
/**
 * @brief Reads /proc/stat file and extracts the total number of processes
 * which is the value to the key "processes".
 *
 * @return {int} : The total number of processes as an integer.
 */
int TotalProcesses();
/**
 * @brief Reads /proc/stat file and extracts the number of running processes
 *  which is the value to the key "procs_running".
 *
 * @return {int} : The number of running processes as an integer.
 */
int RunningProcesses();
/**
 * @brief Reads the operating system name from the /etc/os-release file
 * The function formats the file replacing spaces with underscores and
 * = and " to spaces. It then extracts the value of the key "PRETTY_NAME
 * and reformat back the string to the original format.
 *
 * @return {string} : The operating system name as a string.
 */
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

/**
 * @brief Reads /proc/stat file and extracts the CPU utilization
 *
 * @return {std::vector<std::string>} : Vector containing the CPU utilization
 * data
 */
std::vector<std::string> CpuUtilization();
/**
 * @brief Returns the CPU total utilization
 *
 * @return {long} : The total CPU utilization
 */
long Jiffies();
/**
 * @brief Return the number of active jiffies for the system
 *
 * @return {long} : The number of active jiffies for the system
 */
long ActiveJiffies();
/**
 * @brief Return the number of idle jiffies for the system
 *
 * @return {long} : The number of idle jiffies for the system
 */
long IdleJiffies();

// Processes
std::string Command(std::string pid);
/**
 * @brief Retrieves memory size of a process from /proc/pid/status file
 * We take the size indicated by the key "VmRSS:" and convert it to Mb
 * instead of VmSize which is the total virtual memory size of the process
 * because it includes shared memory and memory that is swapped out
 *
 * @param pid : Process ID
 * @return {string} : Memory size of the process in Mb
 */
std::string Ram(std::string pid);
/**
 * @brief Reads /proc/pid/status file and extracts the UID associated with the
 * process the uid is the values associated with the key "Uid:"
 * @param pid : Process ID
 * @return {string} : UID associated with the process
 */
std::string Uid(std::string pid);
/**
 * @brief Reads /etc/passwd file and extracts the user name associated with the
 * UID the function loops through the file line by line formatting the line by
 * replacing
 * ':' with ' ' and then extracts the user name associated with the UID
 * if the UID is not found the function returns "UNKNOWN"
 * @param uid : User ID
 * @return {string} : User name associated with the UID
 */
std::string User(std::string uid);
/**
 * @brief Reads /proc/pid/stat file and extract necesary data
 * for process cpu utilization in a map :
 * idx 14 utime - CPU time spent in user code, measured in clock ticks
 * idx 15 stime - CPU time spent in kernel code, measured in clock ticks
 * idx 16 cutime - Waited-for children's CPU time spent in user code (in clock
 * ticks) idx 17 cstime - Waited-for children's CPU time spent in kernel code
 * (in clock ticks) idx 22 starttime - Time when the process started, measured
 * in clock ticks
 *
 * @param pid : Process ID
 * @return {std::map<std::string, long>} : Map containing the data
 */
std::map<std::string, long> processUtilData(std::string pid);
/**
 * @brief Reads the /proc/pid/stat file and extracts all the data
 * then returns the starttime at index 22 which is the uptime in seconds
 * of this process.
 *
 * @param pid : Process ID.
 * @return {long} : The uptime time of the process in seconds.
 */
long int UpTime(std::string pid);

}; // namespace LinuxParser

#endif