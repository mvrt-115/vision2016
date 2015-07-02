#ifndef WRITE_TO_FILE_HPP
#define WRITE_TO_FILE_HPP

#include <fstream>
#include <queue>
#include <string>
#include <stdexcept>

void writeToFile ( const std::string &file_path, std::queue<int> write_queue);

#endif // WRITE_TO_FILE_HPP
