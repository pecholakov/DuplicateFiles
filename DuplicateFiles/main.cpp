#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "FileSystem.h"
#include "FilesClassification.h"

using namespace std;

int main(int argc, char* argv[])
{
	// Verification for correct given absolute or relative directory to the program
	string err_message = "Please enter a valid directory to scan for duplicate files!";

	if (argc != 2)
	{
		cerr << err_message << endl;
		return 1;
	}


	if (!IsValid(argv[1]))
	{
		cerr << err_message << endl;
		return 1;
	}
	
	
	FilesClassification manip(argv[1]);
	manip.DetectDuplicateFiles();
	manip.Print();
	//manip.HashFiles();

	int m, group, file_to_stay;
	cout << "Enter m: ";
	cin >> m;

	if (m == 0)
	{
		cout << "Set working group: ";
		cin >> group;
		cout << "Set file that should not be removed: ";
		cin >> file_to_stay;
		manip.RemoveDuplicateFiles(group, file_to_stay);
	}

	manip.GenerateHTMLOutput();


	return 0;
}
