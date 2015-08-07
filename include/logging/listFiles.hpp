#ifndef LIST_FILES_HPP
#define LIST_FILES_HPP

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/progress.hpp>
#include <iostream>

namespace fs = boost::filesystem;

int listFiles (std::string dir_name, std::string *dir_array, std::string *file_array, std::string *other_array);

#endif // LIST_FILES_HPP
