#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include "linux_parser.h"


using std::stof;
using std::string;
using std::to_string;
using std::vector;

#define RUN_PROCESS_KEY ("procs_running")
#define UID_KEY  ("Uid:")
#define KEY_VMRSS ("VmRSS:")

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
           //error
        }
      }
      else
      {
        //error
      }
    }
  }
  else
  {
    //error
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
        //error
      }
    }
    else
    {
      //error
    }
    
  }
  else
  {
    //error
  }
  
  return returnValue; 
}

/**
 * @brief Returns the CPU total utilization 
 * 
 * @return {long} : The total CPU utilization 
 */
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

/**
 * @brief Return the number of active jiffies for the system
 * 
 * @return {long} : The number of active jiffies for the system 
 */
long LinuxParser::ActiveJiffies() {
  std::vector<std::string> cpuUtilization = LinuxParser::CpuUtilization();
  long int activeJiffies = 0;
  
  if (cpuUtilization.size() > kGuestNice_)
  {
    activeJiffies = std::stol(cpuUtilization[kUser_]) 
                    + std::stol(cpuUtilization[kNice_]) 
                    + std::stol(cpuUtilization[kSystem_]) 
                    + std::stol(cpuUtilization[kSoftIRQ_]) 
                    + std::stol(cpuUtilization[kSteal_]) 
                    + std::stol(cpuUtilization[kGuest_]) 
                    + std::stol(cpuUtilization[kGuestNice_]);
  }
  

  return activeJiffies;
}

/**
 * @brief Return the number of idle jiffies for the system
 * 
 * @return {long} : The number of idle jiffies for the system 
 */
long LinuxParser::IdleJiffies() {
  std::vector<std::string> cpuUtilization = LinuxParser::CpuUtilization();
  long int idleJiffies = 0;

  if (cpuUtilization.size() > kIOwait_)
  {
    idleJiffies = std::stol(cpuUtilization[kIdle_]) + std::stol(cpuUtilization[kIOwait_]);
  }
  {
    /* code */
  }

  return idleJiffies;
}

/**
 * @brief Reads /proc/stat file and extracts the CPU utilization
 * 
 * @return {std::vector<std::string>} : Vector containing the CPU utilization data
 */
vector<string> LinuxParser::CpuUtilization() 
{
  string line;
  vector<string> values;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open()) 
  {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      std::istream_iterator<string> begin(linestream), end;
      // Skip "cpu" by advancing iterator
      begin = std::next(begin);
      values = vector<string>(begin, end);
    } 
    else 
    {
      //error
    }
  } 
  else 
  {
    //error
  }

  return values;
}

  

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
        //error
      }
    }
    
  }
  else
  {
    //error
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
        //error
      }
    }
    
    /* Check if the value found is actually a number */
    if (isNumber(value))
    {
      nbOfRunningProcesses = stoi(value);
    }
    else
    {
      //error
    }
  }
  else
  {
    //error
  }
  
  /* Convert and return the value */
  return nbOfRunningProcesses; 
}

/**
 * @brief Reads /proc/pid/cmdline file and extracts the command that generated the
 * process with the given pid.
 * the file contains only one line which is the command that generated the process.
 * 
 * @param pid : Process ID
 * @return {string} : Command that generated the process 
 */
string LinuxParser::Command(string pid) 
{ 
  string line;
  string command = "No command found";
  std::ifstream cmdlineStream(kProcDirectory + pid + kCmdlineFilename);
  
  if (cmdlineStream.is_open())
  {
    if (std::getline(cmdlineStream, line))
    {
      command = line;
    }
  }
  

  return command; 
}

/**
 * @brief Retrieves memory size of a process from /proc/pid/status file
 * We take the size indicated by the key "VmRSS:" and convert it to Mb
 * instead of VmSize which is the total virtual memory size of the process
 * because it includes shared memory and memory that is swapped out
 * 
 * @param pid : Process ID
 * @return {string} : Memory size of the process in Mb 
 */
string LinuxParser::Ram(string pid) {

  string line;
  string key = " ";
  string value = "0";
  bool vmSizeFound = false;
  string retVal = "N/A";
  /* Create input file stream from status file */
  std::ifstream streamStatus(kProcDirectory + pid + kStatusFilename);

  /* check if it is open */
  if (streamStatus.is_open())
  {
    /* Start reading file line by line */
    while (std::getline(streamStatus, line))
    {
      /* Create line stream */
      std::istringstream lineStream(line);
      /* Read key and value in that line */
      lineStream >> key >> value;

      /* If the key is Vmsize:*/
      if (key == KEY_VMRSS)
      {
        /* Set found flag to true */
        vmSizeFound = true;
        /* break while loop */
        break;
      }
    }
    
  }
  else
  {
    /*error opening file*/
  }

  /* Check if vmsize is found */
  if (vmSizeFound)
  {
    /* Convert value to Mb */
    retVal = to_string(stol(value) / 1024);
  }
  else /* Zombie process*/
  {
    retVal = "N/A";
  }
  
  return retVal;
}

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
            //error
          }
        }
    }
    else
    {
        //error
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
        //error
      }
    }

    returnValue = userId;
  }
  else
  {
    //error
  }

  return returnValue; 
}

/**
 * @brief Reads the /proc/pid/stat file and extracts all the data
 * then returns the starttime at index 22 which is the uptime in seconds
 * of this process.
 * 
 * @param pid : Process ID.
 * @return {long} : The uptime time of the process in seconds.
 */
long LinuxParser::UpTime(string pid) 
{ 
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
        //error
      }
    }
    else
    {
      //error
    }

  return valuesFromFile[21] != "" ? std::stol(valuesFromFile[21]) : 0; 
}

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
        //error
      }
    }
    else
    {
      //error
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
