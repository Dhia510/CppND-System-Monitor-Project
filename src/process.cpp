#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h> // for sysconf
#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Define and initialize the static member variable clkTck_
long Process::clkTck_ = sysconf(_SC_CLK_TCK);

/**
 * @brief Construct a new Process object
 * 
 * @param id : process id 
 */
Process::Process(int id) : pid_(to_string(id)) {}

/**
 * @brief Returns the process's ID
 * 
 * @return {int} : Process Ids as an int 
 */
int Process::Pid() 
{ 
    return stoi(pid_); 
}

/**
 * @brief Retireves the CPU utilization data for this process
 * provided by LinuxParser::processUtilData and computes the 
 * CPU utilization in fraction using the following formula
 * ------------------------------------------------------
 * |               CPU Utilization                      |
 * |----------------------------------------------------|
 * | Formula:                                           |
 * |   Total time spent for the process :               |
 * |   total_time = utime + stime + cutime + cstime     |
 * |   Total elapsed time in seconds since the          |
 * |   process started :                                |
 * |   seconds = uptime - (starttime / clk frequency)   |  
 * |   CPU usage :                                      |             
 * |   CPU Utilization = (total_time / clk frequency)   |
 * |                      / seconds                     |
 * ------------------------------------------------------
 * 
 * @return {float} : CPU utilization as a fraction 
 */
float Process::CpuUtilization() 
{   
    /* Get CPU utilization data */
    std::map<string, long> processUtilData = LinuxParser::processUtilData(pid_);
    /* Calculate total time spent by the process */
    long total_time = processUtilData[KEY_UTIME]   
                    + processUtilData[KEY_STIME]   
                    + processUtilData[KEY_CUTIME]  
                    + processUtilData[KEY_CSTIME];

    float cpuUtil = 0;
    float seconds = 0;
    
    if (clkTck_ != 0)
    {
        /* Calculate total elapsed time since the process started */
        seconds = LinuxParser::UpTime() - (processUtilData[KEY_STARTTIME] / clkTck_);

        if (seconds != 0)
        {
            /* Calculate CPU utilization */
            cpuUtil = (total_time / clkTck_) / seconds;
        }       
    }
 
    return cpuUtil; 
}
/**
 * @brief Returns the command that generated this process
 * 
 * @return {std::string} : Command as a string 
 */
string Process::Command() 
{ 
    return LinuxParser::Command(pid_); 
}

/**
 * @brief Gets the process memory usage
 * 
 * @return {string} : Memory usage as a string in Mb
 */
string Process::Ram() 
{ 
    return LinuxParser::Ram(pid_); 
}

/**
 * @brief Returns the user associated with this process
 * 
 * @return {string} : User name as a string 
 */
string Process::User() 
{ 
    /* Find UID associated with this process*/
    string uid = LinuxParser::Uid(pid_);
    /* Return user corresponding to the uid found */
    return LinuxParser::User(uid); 
}

/**
 * @brief Returns the process's uptime
 * Calls LinuxParser::UpTime which returns uptimes in clk
 * ticks then converts it to seconds by dividing by clk frequency
 * 
 * @return {long int} : Process uptime in seconds 
 */
long int Process::UpTime() 
{ 
    return LinuxParser::UpTime(pid_) / clkTck_; 
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }