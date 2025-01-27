#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
/**
 * @brief Extracts hours minutes and seconds from the input
 * and returns a formatted string in the format HH:MM:SS
 * 
 * @param seconds (the time in seconds)
 * @return {string} : The formatted time string 
 */
std::string ElapsedTime(long times); 
};                                    // namespace Format

#endif