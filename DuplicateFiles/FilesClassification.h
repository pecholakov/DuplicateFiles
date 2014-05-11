// Group duplicate files and provides the ability to manage these files

#ifndef FILECLASSIFICATION_H
#define FILECLASSIFICATION_h

#include <map>
#include <string>
#include <vector>

using namespace std;

class FilesClassification
{
public:
	/** Constructor, initializes base directory given_path with given directory
	*/
	FilesClassification(char* dir);

	/** Empty Destuctor
	*/
	~FilesClassification();

	/** At first classifies given files by their sizes, taken from the file descriptors. 
	*	Uses GroupFilesByContent() to determine which files have same content and 
	*	finnaly stores all duplicate files in duplicate_files.
	*	@sa GroupFilesBySize()
	*	@sa GroupFilesByContent()
	*	@sa GroupDuplicateFiles()
	*	@sa duplicate_files
	*/
	void DetectDuplicateFiles();

	void MoveDuplicateFiles();

	/** Takes all duplicate files from duplicate_files and removes all except
	*	file_to_stay, which is the number of file that should not be removed.
	*	@param number_group the number of gruop with duplicate files that must be deleted
	*	@param file_to_stay the file that should not be deleted
	*/
	void RemoveDuplicateFiles(unsigned number_group, unsigned file_to_stay);

	/** Generates simple HTML output
	*/
	void GenerateHTMLOutput();

	/** Displays the content of duplicate_files, where are stored duplicate files, 
	*	grouped in different groups
	*/
	void Print();


private:
	string given_path; ///< The directory where the program scan for duplicate files
	vector<string> files_and_directories; ///< vector containing all possible files in directories in given_path
	map <long, vector<string> > class_by_sz; ///< map containing classified by size files
											///< Files with same size are put in same vector
	map <string, vector<string> > class_by_content; ///< map containing classified by content files. 
													///< Files with same content are put in same vector.
	map <unsigned, vector<string> > duplicate_files; ///< map containing all duplicate files

	/** Converts given file size in bytes into appopriate type (KB,MB,GB,TB)
	*	@param size_in_bytes the size of given file in bytes
	*/
	template <class T>
	string ConvertFileSize(T size_in_bytes);

	

	/** Recursively traverses the files in a base directory and creates a map
	* from file size to a vector of filenames. The size of a file is taken from
	* file descriptor. Additional information is collected too and saved in 
	* struct stat when using fstat(). Files with the same size are put into same vector. 
	*/
	void GroupFilesBySize();

	/** Hashes files, using md5 algorithm, and stores the keys into map class_by_content to 
	*	files with same hash key value. Files with same hash keys are stored into same vector.
	*/
	void HashFiles();

	/** When there are more than one file with same content, it stores the paths to
	*	these files in duplicate_files
	*/
	void GroupDuplicateFiles();

	
};

#endif