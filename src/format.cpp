#include <string>
#include <iomanip> // for setw and setfill
#include "format.h"

using std::string;

/**
 * @brief Extracts hours minutes and seconds from the input
 * and returns a formatted string in the format HH:MM:SS
 * 
 * @param seconds (the time in seconds)
 * @return {string} : The formatted time string 
 */
string Format::ElapsedTime(long seconds) 
{ 
    long hours = seconds / 3600;
    long minutes = (seconds % 3600) / 60;
    long secs = seconds % 60;

    std::ostringstream stream;
    stream << std::setw(2) << std::setfill('0') << hours << ":"
           << std::setw(2) << std::setfill('0') << minutes << ":"
           << std::setw(2) << std::setfill('0') << secs;

    return stream.str();
}