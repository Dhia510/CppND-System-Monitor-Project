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

#define RUN_PROCESS_KEY ("procs_running")
#define UID_KEY  ("Uid:")

/**
 * @brief This function checks if a string is a number
 * This includes also floating point numbers
 * 
 * @param str : String to check
 * @return {true} : String is a number 
 * @return {false} : String is not a number 
 */
static bool isNumber(const std::string& str);

/**
 * @brief Reads the operating system name from the /etc/os-release file
 * The function formats the file replacing spaces with underscores and
 * = and " to spaces. It then extracts the value of the key "PRETTY_NAME
 * and reformat back the string to the original format.
 * 
 * @return {string} : The operating system name as a string. 
 */
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
 * @return {float} : fraction of total used memory
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

/**
 * @brief Extracts the system uptime from the /proc/uptime file
 * This file contains two numbers (values in seconds): the uptime 
 * of the system (including time spent in suspend) and the amount 
 * of time spent in the idle process.
 * 
 * @return {long int} : The system uptime in seconds.  
 */
long int LinuxParser::UpTime() 
{ 
  string line;
  string uptimeValue;
  long int returnValue = 0;
  /* Create input file stream from /proc/uptime*/
  std::ifstream streamUpTime(kProcDirectory + kUptimeFilename);

  /* Check if it is open */
  if (streamUpTime.is_open())
  {
    /* Only one line in this file so we read it and check if it is read correctly */
    if (std::getline(streamUpTime, line))
    {
      /* Create string stream from this line */
      std::stringstream lineStream(line);

      /* Get the Uptime value */
      lineStream >> uptimeValue;

      /* Check if it is a number */
      if (isNumber(uptimeValue))
      {
        /* Convert to long */
        returnValue = stol(uptimeValue);
      }
      else
      {
        std::cout << "LinuxParser::UpTime:: Error value is not a number \n";
      }
    }
    else
    {
      std::cout << "LinuxParser::UpTime:: Error reading line from file\n";
    }
    
  }
  else
  {
    std::cout << "LinuxParser::UpTime:: Error opening input stream from file\n";
  }
  
  return returnValue; 
}

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

/**
 * @brief Reads /proc/stat file and extracts the total number of processes  
 * which is the value to the key "processes".
 * 
 * @return {int} : The total number of processes as an integer. 
 */
int LinuxParser::TotalProcesses() 
{ 
  string key = "";
  string value = "0";
  string line;
  std::ifstream statStream(kProcDirectory + kStatFilename);
  int totProcessNb = 0;

  /* Check if input file stream is open */
  if (statStream.is_open())
  {
    /* As long as the key is different from "processes"*/
    while (key != "processes")
    {
      /* Read line and check if it is read correctly */
      if (std::getline(statStream, line))
      {
        /* Create string stream from line */
        std::istringstream lineStream(line);

        lineStream >> key >> value;
      }
      else
      {
        std::cout << "LinuxParser::TotalProcesses:: Error reading line from file\n";
      }
    }
    
  }
  else
  {
    std::cout << "LinuxParser::TotalProcesses:: Error opening input stream from file\n";
  }
  
  /* If value found is a number */
  if (isNumber(value))
  {
    /* Convert to int and store it in return var */
    totProcessNb = stoi(value);
  }
  
  return totProcessNb; 
}

/**
 * @brief Reads /proc/stat file and extracts the number of running processes 
 *  which is the value to the key "procs_running".
 * 
 * @return {int} : The number of running processes as an integer.
 */
int LinuxParser::RunningProcesses() 
{ 
  string key = "";
  string value = "0";
  string line;
  int nbOfRunningProcesses = 0;
  /* Open a file input stream from file /proc/stat */
  std::ifstream statStream(kProcDirectory + kStatFilename);

  /* Check if the input stream is opened correctly */
  if (statStream.is_open())
  {
    /* loop through the file until the key procs_running is found */
    while (key != RUN_PROCESS_KEY)
    {
      /* Read the line */
      if (std::getline(statStream, line))
      {
        /* Create a string stream from this line */
        std::istringstream lineStream(line);
        /* Get key and Value */
        lineStream >> key >> value;

      }
      else
      {
        std::cout << "LinuxParser::RunningProcesses:: error reading line from file\n";
      }
    }
    
    /* Check if the value found is actually a number */
    if (isNumber(value))
    {
      nbOfRunningProcesses = stoi(value);
    }
    else
    {
      std::cout << "LinuxParser::RunningProcesses:: Error extracting the value for nb of processes\n";
    }
  }
  else
  {
    std::cout << "LinuxParser::RunningProcesses:: Error opening input stream from file\n";
  }
  
  /* Convert and return the value */
  return nbOfRunningProcesses; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

/**
 * @brief Reads /proc/pid/status file and extracts the UID associated with the process
 *  the uid is the values associated with the key "Uid:"
 * @param pid : Process ID
 * @return {string} : UID associated with the process 
 */
string LinuxParser::Uid(string pid) 
{ 
    string key = " ";
    string uid = " ";
    string line;
    /* Find the UID associated with the process */
    /* Create input file stream from /proc/pid/status */
    std::ifstream statuStream(LinuxParser::kProcDirectory + pid + LinuxParser::kStatusFilename);

    /* Check if the file stream is open */
    if (statuStream.is_open())
    {
        /* Read lines until key Uid is found */
        while (key != UID_KEY)
        {
          /* Check if line is read correctly*/
          if(std::getline(statuStream, line))
          {
              statuStream >> key >> uid;
          }
          else
          {
            std::cout << "LinuxParser::Uid:: Error reading line from file\n";
          }
        }
    }
    else
    {
        std::cout << "LinuxParser::Uid:: Error opening input stream from file\n";
    }
    
  return uid; 
}

/**
 * @brief Reads /etc/passwd file and extracts the user name associated with the UID
 *  the function loops through the file line by line formatting the line by replacing
 * ':' with ' ' and then extracts the user name associated with the UID
 * if the UID is not found the function returns "UNKNOWN"
 * @param uid 
 * @return string 
 */
string LinuxParser::User(string uid) 
{ 
  string userId = "";
  string returnValue = "UNKNOWN";
  std::ifstream passwdStream(kPasswordPath);
  string value = " ";
  string x;
  string line;

  if (passwdStream.is_open())
  {
    while (value != uid)
    {
      if (std::getline(passwdStream, line))
      {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream lineStream(line);
        lineStream >> userId >> x >> value;
      }
      else
      {
        std::cout << "LinuxParser::User:: Error reading line from file\n";
      }
    }

    returnValue = userId;
  }
  else
  {
    std::cout << "LinuxParser::User:: Error opening input stream from file\n";
  }

  return returnValue; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }

/**
 * @brief Reads /proc/pid/stat file and extract necesary data
 * for process cpu utilization in a map :
 * idx 14 utime - CPU time spent in user code, measured in clock ticks
 * idx 15 stime - CPU time spent in kernel code, measured in clock ticks
 * idx 16 cutime - Waited-for children's CPU time spent in user code (in clock ticks)
 * idx 17 cstime - Waited-for children's CPU time spent in kernel code (in clock ticks)
 * idx 22 starttime - Time when the process started, measured in clock ticks
 * 
 * @param pid : Process ID
 * @return {std::map<std::string, long>} : Map containing the data
 */
std::map<std::string, long> LinuxParser::processUtilData(string pid)
{
    std::map<std::string, long> processUtilData;
    string line;
    string value;
    vector<string> valuesFromFile;
    /* Create file iput stream from file /proc/pid/stat */
    std::ifstream statStream(kProcDirectory + pid + kStatFilename);

    /* Check file stream is open */
    if (statStream.is_open())
    {
      /* Read line and check if it is read successfully*/
      if (std::getline(statStream, line))
      {
        /* Create string stram from line */
        std::istringstream lineStream(line);
        
        /* Read values in the line */
        while (lineStream >> value)
        {
          /* Store each value in a vector */
          valuesFromFile.push_back(value);
        }
        
      }
      else
      {
        std::cout << "LinuxParser::processUtilData:: Error reading line from file\n";
      }
    }
    else
    {
      std::cout << "LinuxParser::processUtilData:: Error opening input stream from file\n";
    }

    /* Check if the vector size is coherent */
    if (valuesFromFile.size() > 22) {
        processUtilData[KEY_UTIME] = std::stol(valuesFromFile[UTIME_IDX]);
        processUtilData[KEY_STIME] = std::stol(valuesFromFile[STIME_IDX]);
        processUtilData[KEY_CUTIME] = std::stol(valuesFromFile[CUTIME_IDX]);
        processUtilData[KEY_CSTIME] = std::stol(valuesFromFile[CSTIME_IDX]);
        processUtilData[KEY_STARTTIME] = std::stol(valuesFromFile[STARTTIME_IDX]);
    }

    return processUtilData;
}

/**
 * @brief This function checks if a string is a number
 * This includes also floating point numbers
 * 
 * @param str : String to check
 * @return true 
 * @return false 
 */
static bool isNumber(const std::string& str) {
    bool decimalPointSeen = false;

    for (char c : str) {
        if (c == '.') {
            if (decimalPointSeen) return false; // More than one decimal point
            decimalPointSeen = true;
        } else if (!std::isdigit(c)) {
            return false;
        }
    }

    return !str.empty();
}
