#include "processor.h"
#include "linux_parser.h"

/**
 * @brief Returns the CPU utilization
 * 
 * @return {float} : CPU utilization in fraction 
 */
float Processor::Utilization() 
{ 
    long total = LinuxParser::Jiffies();
    long active = LinuxParser::ActiveJiffies();
    
    float ret = (float)active / (float)total;
    return ret;
}