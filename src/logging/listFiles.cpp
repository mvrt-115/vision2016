#include "logs/listFiles.hpp"

int listFiles (std::string dir_name, std::string *dir_array, std::string *file_array, std::string *other_array)
{
	fs::path full_path ( fs::initial_path<fs::path>() );	

	full_path = fs::system_complete ( fs::path(dir_name) );
	
	unsigned long file_count = 0;
	unsigned long dir_count = 0;
	unsigned long other_count = 0;
	unsigned long err_count = 0;

	if ( !fs::exists (full_path))
	{
		std::cout << std::endl << "Not found: " << full_path.string() << std::endl;
		return -1;
	}

	if (fs::is_directory(full_path))
	{
		std::cout << std::endl << "In directory: " << full_path.string() << std::endl << std::endl;
		fs::directory_iterator end_itr;

		for (fs::directory_iterator dir_itr (full_path); dir_itr != end_itr; ++dir_itr)
		{
			try
			{
				if (fs::is_directory( dir_itr->status() )) // List subdirectories
				{
					++dir_count;
					std::cout << "(" << dir_count << "): " << dir_itr->path().string() << " [directory]" << std::endl;
					dir_array[dir_count] = dir_itr->path().string();
				}

				else if (fs::is_regular_file( dir_itr->status() )) // List files
				{
					++file_count;
					std::cout << "(" << file_count << "): " << dir_itr->path().string() << std::endl;
					file_array[file_count] = dir_itr->path().string();
				}

				else // List other
				{
					++other_count;
					std::cout << "(" << other_count << "): " << dir_itr->path().string() << " [other]" << std::endl;
					other_array[other_count] = dir_itr->path().string();
				}
			}

			catch ( const std::exception &ex)
			{
				++err_count;
				std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
			}
		}

		std::cout << "\n" << file_count << " files \n"
				  << dir_count << " directories\n"
				  << other_count << " others\n"
				  << err_count << " errors\n\n";
	}

	else // Must be a file
	{
		std::cout << std::endl << "Found: " << full_path << std::endl;
	}

	return 0;
}
