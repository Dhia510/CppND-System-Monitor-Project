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

// TODO: Return the command that generated this process
string Process::Command() { return string(); }

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

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

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }