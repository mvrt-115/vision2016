#include "logging/currentDateTime.hpp"

const std::string currentDateTime() 
{
	// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];
	
	time (&rawtime);
  	timeinfo = localtime (&rawtime);

  	strftime (buffer, 80, "%Y-%m-%d.%X", timeinfo);

  	return buffer;
}
