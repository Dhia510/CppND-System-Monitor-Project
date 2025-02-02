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
  /**
   * @brief Returns the command that generated this process
   *
   * @return {std::string} : Command as a string
   */
  std::string Command();
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
  float CpuUtilization() const;
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
  /**
   * @brief Overload the less than operator to compare two processes
   * based on their CPU utilization
   *
   * @param a : Process to compare with
   * @return {true} : If this process has a lower CPU utilization
   * @return false  : Otherwise
   */
  bool operator<(Process const &a) const; // TODO: See src/process.cpp
  /**
   * @brief Construct a new Process object
   *
   * @param id : process id
   */
  Process(int id);
  /**
   * @brief Updates cached CPU utilization
   *
   */
  void UpdateCpuUtilization();

private:
  std::string pid_;
  static long clkTck_;
  float cached_cpu_{0.0};
};

#endif