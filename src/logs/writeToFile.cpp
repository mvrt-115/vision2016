#include "logs/writeToFile.hpp"

void writeToFile (const std::string file_path, const std::string file_name, std::queue<int> write_queue)
{	
	std::ofstream file;
	file.open (file_path + file_name, std::ios::out | std::ios::app); // Open file for writing
	
	if (file.is_open())
   	{
		while (!write_queue.empty())
		{
			file << write_queue.front() << std::endl; // Write first element to file
			write_queue.pop(); // Release first element
		}
	}
   	else std::cout << "Error - unable to open file for writing";

	file.close();	
}
