#include "FilesClassification.h"
#include "FileSystem.h"
#include "md5.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <io.h>
#include <cmath>
#include <Windows.h>

using namespace std;


FilesClassification::FilesClassification(char* dir)
{
	if (!IsValid(dir))
	{
		cerr << "Please enter a valid directory to scan for duplicate files!" << endl;
		return;
	}
	this->given_path = dir;

}

FilesClassification::~FilesClassification()
{
}

void FilesClassification::GroupFilesBySize()
{
	class_by_sz.clear();

	if (!IsDirectory(given_path))
	{
		return;
	}

	GetFilesAndDirectoriesRecursive(given_path, files_and_directories);
	struct _stat fileStat;

	int file = 0;
	for (int i = 0; i < files_and_directories.size(); i++)
	{
		string& name = files_and_directories[i];
		if (IsFile(name))
		{
			file = _open(name.c_str(), _O_RDONLY);
			if(_fstat(file, &fileStat) >= 0)    
				class_by_sz[fileStat.st_size].push_back(name);
			_close(file);
		}
	}
}

void FilesClassification::HashFiles()
{
	crypto::md5_helper_t hhasher;

	for (map<long, vector<string>>::iterator it = class_by_sz.begin();
		it != class_by_sz.end(); ++it)
	{
		if (it->second.size() > 1)
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				std::string digest = hhasher.hexdigestfile(it->second[i]);

				if(digest.empty())
					cout << "error code = " << hhasher.lasterror().errorCode 
					<< ", error message = " << hhasher.lasterror().errorMessage
					<< std::endl;
				else
					class_by_content[digest].push_back(it->second[i]);
			}
		}
	}
}

void FilesClassification::GroupDuplicateFiles()
{
	unsigned number_group = 0;
	for (map<string, vector<string> >::iterator it = class_by_content.begin();
		it != class_by_content.end(); ++it)
	{
		if (it->second.size() > 1)
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				duplicate_files[number_group].push_back(it->second[i]);
			}	
			number_group++;
		}
	}
}

void FilesClassification::DetectDuplicateFiles()
{
	GroupFilesBySize();
	HashFiles();
	GroupDuplicateFiles();
}

void FilesClassification::MoveDuplicateFiles()
{
	/*if (!IsValid(new_directory))
	{
	cerr << "Please enter valid directory to store duplicate files!" << endl;
	return;
	}*/
	if (duplicate_files.empty())
	{
		cerr << "There are no duplicate files to be moved" << endl;
		return;
	}

	//TODO: Add functionality
}

void FilesClassification::RemoveDuplicateFiles(unsigned number_group, unsigned file_to_stay)
{
	--number_group;
	--file_to_stay;
	if(duplicate_files.empty())
	{
		if (class_by_content.empty())
		{
			cerr << "There are no duplicate files or scanning for duplicate files was not performed!" << endl;
		}
		else
		{
			cerr << "There are no duplicate files to be deleted! " << endl;
		}
		return;
	} 

	for(map<unsigned, vector<string>>::iterator it = duplicate_files.begin();
		it != duplicate_files.end(); ++it)
	{
		if (number_group == it->first)
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				if (file_to_stay != i)
				{
					if (!IsValid(it->second[i]))
					{
						cerr << "Could not find the specific file" << endl;
						return;
					}

					if (remove(it->second[i].c_str()) == 0)
					{		
						cout << "File(s) successfully deleted " << endl << endl;
						it->second[i].clear();
					}
					else
					{
						cerr << "File(s) was not deleted " << endl;
					}
				}
			}
		}
	}
}

void FilesClassification::GenerateHTMLOutput()
{
	ofstream stream("htmlOutput.htm", ios::out);
	stream << "<!DOCTYPE html><html><head></head><body><h1><i>Duplicate Files</i></h1>";
	unsigned num_to_print;
	int file = 0;
	struct _stat fileStat;

	if (duplicate_files.empty())
	{
		stream << "<br> Could not find duplicate files! " << endl;
		return;
	}

	for(map <unsigned, vector<string>>::iterator it = duplicate_files.begin();
		it != duplicate_files.end(); it++)
	{
		for (int i = 0; i < it->second.size(); i++)
		{
			if (!it->second[i].empty())
			{
				file = _open(it->second[i].c_str(), _O_RDONLY);
				_fstat(file, &fileStat);
				_close(file);
				break;
			}
		}

		long used_space = it->second.size() * fileStat.st_size;
		num_to_print = it->first;

		stream << "<br><b><h3> Equivalence Group #" << ++num_to_print << ": </h3></b><br> ( " 
			<< ConvertFileSize(used_space) << ", " << ConvertFileSize(fileStat.st_size) << " per file ) <br> ";
		for (int i = 0; i < it->second.size(); i++)
		{
			stream << i+1 << ". " << it->second[i] << "<br>";
		}	
		stream << "<br>";
	}
	stream << "</body></html>";
	stream.close();
}

void FilesClassification::Print()
{
	unsigned num_to_print;
	int file = 0;
	struct _stat fileStat;

	if (duplicate_files.empty())
	{
		cout << "Could not find duplicate files! " << endl;
		return;
	}

	for(map <unsigned, vector<string>>::iterator it = duplicate_files.begin();
		it != duplicate_files.end(); it++)
	{
		for (int i = 0; i < it->second.size(); i++)
		{
			if (!it->second[i].empty())
			{
				file = _open(it->second[i].c_str(), _O_RDONLY);
				_fstat(file, &fileStat);
				_close(file);
				break;
			}
		}

		long used_space = it->second.size() * fileStat.st_size;
		num_to_print = it->first;

		cout << "Equivalence Group #" << ++num_to_print << ":" << endl << "( " 
			<< ConvertFileSize(used_space) << ", " << ConvertFileSize(fileStat.st_size) << " per file ) " << endl;
		for (int i = 0; i < it->second.size(); i++)
		{
			cout << '\t' << i+1 << ". " << it->second[i] << endl;
		}	
		cout << endl;
	}
}

template <class T>
string FilesClassification::ConvertFileSize(T size_in_bytes)
{

	int count = 0;
	double converted_size = 0;
	string result;
	stringstream toString;

	while (size_in_bytes > 1024)
	{
		converted_size = size_in_bytes / 1024.0;
		size_in_bytes /= 1024;
		count++;
	}

	switch (count)
	{
	case 1:
		toString << setprecision(2) << converted_size << "KB";
		result = toString.str();
		break;
	case 2:
		toString << setprecision(2) <<converted_size << "MB";
		result = toString.str();
		break;
	case 3:
		toString << setprecision(2) << converted_size << "GB";
		result = toString.str();
		break;
	case 4:
		toString << setprecision(2) << converted_size << "TB";
		result = toString.str();
		break;
	default:
		toString << size_in_bytes << "B";
		result = toString.str();
		break;
	}

	return result;
}