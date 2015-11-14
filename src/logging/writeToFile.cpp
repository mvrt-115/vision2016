#include "logging/writeToFile.hpp"

void writeToFile ( const std::string &file_path, std::queue<int> write_queue)
{	
	std::ofstream file;
	file.open (file_path, std::ios::out | std::ios::trunc); // Open file for writing, if it exists, delete its original contents
	
	if (file.is_open())
   	{
		while (!write_queue.empty())
		{
			file << write_queue.front() << std::endl; // Write first element to file
			write_queue.pop(); // Release first element
		}
	}

   	else
	{
		throw std::runtime_error("Error - Unable to open file for writing.");
	}

	file.close();	
}
