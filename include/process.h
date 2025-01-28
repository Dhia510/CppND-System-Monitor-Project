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
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
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
};

#endif