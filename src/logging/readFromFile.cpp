#include "logging/readFromFile.hpp"

void readFromFile (const std::string &file_path, std::queue<int> &read_queue)
{
	std::string line;
	int line_to_int;
	std::fstream file (file_path);

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			line_to_int = std::stoi(line);
			read_queue.push(line_to_int);
		}
	}	

	else
	{
		throw std::runtime_error("Error - Unable to open file for reading.");
	}
}
