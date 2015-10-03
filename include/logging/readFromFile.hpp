#ifndef READ_FROM_FILE_HPP
#define READ_FROM_FILE_HPP

#include <fstream>
#include <string>
#include <queue>
#include <stdexcept>

void readFromFile (const std::string &file_path, std::queue<int> &read_queue);

#endif // READ_FROM_FILE_HPP
	
