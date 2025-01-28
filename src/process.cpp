#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

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

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

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