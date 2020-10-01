/*********************************************
 * Robert Oury
 * CSCI 480 - Section: 0002
 * Assignment 7
 * 12/02/19
 * Z1841079
 * TA: Jingwan Li
 * 
 * Purpose: This program is designed to run a simulation
 * 	of a FAT file system that allows different operations
 * 	to be applied to the files within or trasfering
 * 	to the fat
 *********************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <stddef.h>
#include <algorithm>
#include <iostream>
#include <list>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;
//Howoften to print out the FAT
#define HOWOFTEN 5
//Size of each block
const int blockSize = 512;

//Struct for each file in the fat with size name and blocks used
struct Entry
{
public:
	int size;
	string name;
	vector<short> blocks;
};

//type def for iteration
typedef vector<Entry>::iterator dir_iter;
typedef list<string>::iterator command_iter;
typedef vector<short>::iterator vec_iter;

//Command List from the Data file
list<string> CommandList;
//FAT Containing info for each block being used
vector<short> FAT(4096, 0);
//directory of every entry in the fat
vector<Entry> directory;

//Function Declerations
stringstream Copy(string name, string newname);
stringstream Delete(string name);
stringstream NewFile(string name, int size);
stringstream Modify(string name, int newSize);
stringstream Rename(string name, string newName);

/*********************************************************
 * Name: SEARCH
 * 
 * Purpose: Searches the Directory for a file with a given
 * 	name.
 *********************************************************/
dir_iter Search(string name)
{
	for (dir_iter search = directory.begin(); search != directory.end(); ++search)
	{
		//If the name is found then return the iterator
		if (search->name == name)
		{
			return (search);
		}
	}
	return (directory.end());
}

/*********************************************************
 * Name: BlocksNeeded
 * 
 * Purpose: Determines how many blocks are required for
 * 	a file allocation
 *********************************************************/
int BlocksNeeded(int size)
{
	if (size % blockSize != 0)
		return ((size / blockSize) + 1);
	else
		return (size / blockSize);
}

/*********************************************************
 * Name: PrintTable
 * 
 * Purpose: Prints out the content of the Fat and the 
 * 	Directory of all files
 *********************************************************/
void PrintTable()
{
	int totalSize = 0;
	cout << "\nDirectory Listing\n";

	//For each File in the directort print its name size and the blocks currently being used.
	for (dir_iter search = directory.begin(); search != directory.end(); ++search)
	{
		cout << "File Name: " << setw(10) << left << search->name;
		cout << "    File Size: " << search->size << "\n";
		cout << "Cluster(s) in use:";
		for (uint i = 0; i < search->blocks.size(); i++)
		{
			if (i % 10 == 0 && i != 0)
			{
				cout << "\n\t\t\t\t";
			}
			cout << setw(6) << right << search->blocks.at(i);
		}
		if (search->blocks.size() == 0)
		{
			cout << " (none)";
		}
		cout << "\n";
		totalSize += search->size;
	}
	//Print total size of directory
	cout << "Files: " << directory.size() << "\t"
		 << "Total Size: " << totalSize << " Bytes\n\n";

	int COLUMNS = 12;
	//Print out 252 sized chunk of fat table since thats all we used
	//for the simulation
	for(int i = 0; i < 252;)
	{
		cout << i << " - " << i+COLUMNS-1 << ": \t";

		for(int j = 0; j < COLUMNS; j++)
		{
			cout << FAT[j+i] << "\t";
		}
		cout << "\n";
		i += COLUMNS;
	}
	cout << "\n";
}

/*********************************************************
 * Name: Copy
 * 
 * Purpose: Copies a file and finds space for the new allocated
 * 	file on the FAT
 *********************************************************/
stringstream Copy(string name, string newname)
{
	stringstream output;
	int newSize = 0;
	dir_iter found = Search(name);
	//if the file is found then make the new file the same size
	if (found != directory.end())
	{
		newSize = found->size;
	}
	else
	{
		output << "Cannot find file to copy: " << name << "\n";
		return (output);
	}
	//if the file doesnt exist already then create the new file.
	if (Search(newname) == directory.end())
	{
		output << "Copy already made of file: " << newname << "\n";
		return (output);
	}
	NewFile(newname, newSize);
	output << "Successfully " << setw(10) << "Copied: " << setw(15) << name << " to " + newname << '\n';
	return (output);
}

/*********************************************************
 * Name: Delete
 * 
 * Purpose: Removes a file from the diretory and reallocates
 * 	space back to the FAT
 *********************************************************/
stringstream Delete(string name)
{
	stringstream output;
	dir_iter found = Search(name);
	//if the file is found then remove its blocks from the fat.
	if (found != directory.end())
	{
		for (uint i = 0; i < found->blocks.size(); i++)
		{
			FAT[found->blocks.at(i)] = 0;
		}
		directory.erase(found);	
	}
	output << "Successfully " << setw(10) << "Deleted: " << setw(15) << name << '\n';
	return (output);
}

/*********************************************************
 * Name: NewFile
 * 
 * Purpose: Creates a new file by finding a suitable block
 * 	to contain the file. also adds the file into the Directory
 *********************************************************/
stringstream NewFile(string name, int size)
{
	stringstream output;
	Entry file;
	if (Search(name) != directory.end())
	{
		output << "File is already created cannot duplicate: " << name << "\n";
		return (output);
	}

	file.name = name;
	file.size = size;
	int amountOfBlocks = BlocksNeeded(size);
	//Search for blocks to be able to allocate for the new file
	for (int i = 1; i <= amountOfBlocks; i++)
	{
		int blockNum = 0;
		for (vec_iter search = FAT.begin(); search != FAT.end(); search++)
		{
			if (*search == 0 && i == amountOfBlocks)
			{
				*search = -1;
				file.blocks.push_back(blockNum);
				break;
			}
			else if (*search == 0)
			{
				*search = blockNum+1;
				file.blocks.push_back(blockNum);
				break;
			}
			blockNum++;
		}
	}
	output << "Successfully " << setw(10) << "Created: " << setw(15) << name << " Size: " << size << '\n';
	directory.push_back(file);
	return (output);
}

/*********************************************************
 * Name: Modify
 * 
 * Purpose: File size is altered so this function creates
 * 	new file as a copy reallocating enough space to house
 * 	the file and then deletes the old version and renames
 * 	the new one
 *********************************************************/
stringstream Modify(string name, int newSize)
{
	stringstream output;
	string tempname = name + "(1)";
	NewFile(tempname, newSize);
	Delete(name);
	Rename(tempname, name);
	output << "Successfully " << setw(10) << "Modified: " << setw(15) << name << "\n";
	return (output);
}

/*********************************************************
 * Name: Rename
 * 
 * Purpose: Changes name of file in directory
 *********************************************************/
stringstream Rename(string name, string newName)
{
	stringstream output;
	dir_iter found = Search(name);
	//if file exists the rename it.
	if (Search(newName) != directory.end())
	{
		output << "File Already Exists " << newName << "\n";
		return (output);
	}
	if (found == directory.end())
		output << "No File found, File: " << name << "\n";
	else
		found->name = newName;

	output << "Successfully " << setw(10) << "Renamed: " << setw(15) << name << " to " + newName << '\n';
	return (output);
}

/*********************************************************
 * Name: ReadFile
 * 
 * Purpose: Read input file and collect the
 * 	lines in the Command list
 *********************************************************/
void ReadFile(string fileName)
{
	ifstream filestr;
	filestr.open(fileName);
	//If file exists
	if (filestr.is_open())
	{
		string line;
		while (getline(filestr, line))
		{
			if (line.at(0) != '?')
			{
				//Read each line into list if not delimeter
				CommandList.push_back(line);
			}
		}
	}
	else
	{
		cout << "Error opening file\n";
		exit(-1);
	}
	filestr.close();
	return;
}

/*********************************************************
 * Name: FileManager
 * 
 * Purpose: Reads each file operation and executes the
 * 	function to complete the operation.
 *********************************************************/
void FileManager()
{
	int printCount = 0; //Times Through loop
	NewFile(".", 512);
	NewFile("..", 0);
	PrintTable(); //Inital Print
	for (command_iter line = CommandList.begin(); line != CommandList.end(); ++line)
	{
		istringstream currentCommand(*line); //Retrieve string for parse
		char operation = (*line).at(0);		 //Operation to Execute

		//variables to read in data about process
		int size;
		int newSize;
		string name;
		string newName;
		//Switch case to control operation of storage based on operation code
		switch (operation)
		{
		case 'C':
			currentCommand >> operation >> name >> newName;
			cout << Copy(name, newName).str();
			break;
		case 'D':
			currentCommand >> operation >> name;
			cout << Delete(name).str();
			break;
		case 'N':
			currentCommand >> operation >> name >> size;
			cout << NewFile(name, size).str();
			break;
		case 'M':
			currentCommand >> operation >> name >> newSize;
			cout << Modify(name, newSize).str();
			break;
		case 'R':
			currentCommand >> operation >> name >> newName;
			cout << Rename(name, newName).str();
			break;
		default:
			cout << "Invalid Read";
		}
		//Print out Lists every 5 Rounds
		printCount++;
		if (printCount % HOWOFTEN == 0)
		{
			PrintTable();
		}
	}
	//End Print
	PrintTable();
}
//Reads in the file and calls the simulation function.
int main(int argc, char *argv[])
{
	ReadFile("data7.txt");
	FileManager();
}
