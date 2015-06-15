#ifndef WRITE_TO_FILE_HPP
#define WRITE_TO_FILE_HPP

#include <fstream>
#include <queue>
#include <iostream>
#include <string>

void writeToFile (const std::string file_path, const std::string file_name, std::queue<int> write_queue);

#endif // WRITE_TO_FILE_HPP
