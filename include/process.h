#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  /**
   * @brief Returns the process's ID
   * 
   * @return {int} : Process Ids as an int 
   */
  int Pid();  
  /**
   * @brief Returns the user associated with this process
   * 
   * @return {string} : User name as a string 
   */                             
  std::string User();                     
  std::string Command();                   // TODO: See src/process.cpp
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
  float CpuUtilization();  
 /**
  * @brief Gets the process memory usage
  * 
  * @return {string} : Memory usage as a string in Mb
  */
  std::string Ram();   
/**
 * @brief Returns the process's uptime
 * Calls LinuxParser::UpTime which returns uptimes in clk
 * ticks then converts it to seconds by dividing by clk frequency
 * 
 * @return {long int} : Process uptime in seconds 
 */                    
  long int UpTime();                     
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp
  /**
   * @brief Construct a new Process object
   * 
   * @param id : process id 
   */
  Process(int id);
  // TODO: Declare any necessary private members
 private:
 std::string pid_;
 static long clkTck_;
};

#endif