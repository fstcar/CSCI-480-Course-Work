/*********************************************
 * Robert Oury
 * CSCI 480 - Section: 0002
 * Assignment 6
 * 11/13/19
 * Z1841079
 * TA: Jingwan Li
 * 
 * Purpose: 
 *********************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <string>
using namespace std;

#define HOWOFTEN 5;				//How often to print list contents
const int KB = 1024;			//Kilobyte Constant
const int MB = KB * KB;			//MegaByte Constant
int startAddress = 3 * MB;		//Start Address for Memory taken by OS
bool bestFitAlgorithm;			//Which Algorithm to use for managment.

struct memoryBlock
{
	int startAddress;			//Starting Address of the block
	int size;					//Size of the block
	int blockID;				//ID of the block
	string proccessID;			//Name of the block

	//to String Method for easy testing.
	string toString()
	{
		if (proccessID.empty())
		{
			return "\nAddress: " + to_string(startAddress) + " Size: " + to_string(size);
		}
		else
		{
			return "\nAddress: " + to_string(startAddress) + " Size: " + to_string(size) + " Name: " + proccessID;
		}
	}
};

typedef list<memoryBlock>::iterator list_iter;	//type def for shorter if statments

list<memoryBlock> Avail;		//List of Available memory blocks
list<memoryBlock> InUse;		//list of memory blocks in use

list<string> TransactionList;	//transactions read from file

/*********************************************************
 * Name: ReadFile
 * 
 * Purpose: Read input file and collect the
 * 	lines in the transaction list
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
				TransactionList.push_back(line);
			}
		}
	}
	else
	{
		cerr << "Error opening file\n";
		exit(-1);
	}
	filestr.close();
	return;
}

/*********************************************************
 * Name: LoadorAllocate
 * 
 * Purpose: Loads a program into memory or allocates that
 * 	program more memory via the lists given.
 * 	Uses The algorithm specified on how to search for a new
 * 	block
 *********************************************************/
void LoadorAllocate(int id, int size, string name, bool loadorallocate)
{
	bool blockFound = false;				//Flag for found the block

	list_iter iterAvail;	//iterator for moving through the avail list
	list_iter loadBlock;	//Pointer to which block to Load or Allocate

	if (bestFitAlgorithm)
	{
		int smallestBlock = 15 * MB;		//Smallest block that fits
		for (iterAvail = Avail.begin(); iterAvail != Avail.end(); ++iterAvail)
		{	
			//if the block available is smaller than last and fits the request
			if ((iterAvail->size - size) < smallestBlock && (iterAvail->size - size) > 0)
			{
				smallestBlock = (iterAvail->size - size);	//new smallest block
				loadBlock = iterAvail;	//Points to best block
				blockFound = true;
			}
		}
	}
	else
	{
		for (iterAvail = Avail.begin(); iterAvail != Avail.end(); ++iterAvail)
		{
			//finds first block that fits
			if ((iterAvail->size - size) >= 0)
			{
				loadBlock = iterAvail;	//Points to first block that fits
				blockFound = true;
				break;
			}
		}
	}

	if (blockFound)
	{
		loadBlock->size = loadBlock->size - size;					//reduce block by requested amount
		startAddress = loadBlock->startAddress;						//start address for new block
		loadBlock->startAddress = loadBlock->startAddress + size;	//new start address for available block

		//initalize new block inuse
		memoryBlock block;
		block.size = size;
		block.startAddress = startAddress;
		block.blockID = id;
		block.proccessID = name;
		InUse.push_front(block);
		//done initalize

		//Print Out Status Info
		if (loadorallocate)
			cout << "      LOAD: ";
		else
			cout << "  ALLOCATE: ";
		cout << "ID: " << id << ", Name " << name << ", Size " << size << endl;
		return;
	}
	else
	{
		//If Block not found
		if (loadorallocate)
			cout << "      LOAD: ";
		else
			cout << "  ALLOCATE: ";
		cout << "ID: " << id << " --ERROR--" << endl;
		return;
	}
}

/*********************************************************
 * Name: MergeBlock
 * 
 * Purpose: Combines any Blocks in avail that could be 
 * 	merged together
 *********************************************************/
void MergeBlocks()
{
	//merge blocks together in available that can be
	for (list_iter iterAvail = Avail.begin(); iterAvail != --Avail.end(); iterAvail++)
	{
		list_iter nextBlock = iterAvail; //next block in list
		nextBlock++;
		//if the blocks are companions and their sum is < 4MB then merge					
		if ((iterAvail->size + iterAvail->startAddress) == (nextBlock->startAddress))
		{
			if ((iterAvail->size + nextBlock->size) <= 4 * MB)
			{
				iterAvail->size = iterAvail->size + nextBlock->size;
				Avail.erase(nextBlock);
				iterAvail--;
			}
		}
	}
}

/*********************************************************
 * Name: AddBlockToAvail
 * 
 * Purpose: Adds a block into the Correct Location in
 * 	the available list
 *********************************************************/
void AddBlockToAvail(list_iter InUse)
{
	//New available block allocation
	memoryBlock block;
	block.size = InUse->size;
	block.startAddress = InUse->startAddress;

	//find location in memory to place block
	for (list_iter iterAvail = Avail.begin(); iterAvail != Avail.end(); iterAvail++)
	{
		if (iterAvail->startAddress > InUse->startAddress)
		{
			//if memory location is found place block there
			Avail.insert(iterAvail, block);
			break;
		}
		else if (next(iterAvail) == Avail.end())
		{
			//place block at end if start address is after all others
			Avail.push_back(block);
		}
	}
}

/*********************************************************
 * Name: Deallocate
 * 
 * Purpose: Removes a specified memory block name and id
 * 	from the inuse list and returns the block to the avail
 * 	memory list
 *********************************************************/
void Deallocate(int id, string name)
{
	bool blockFound = false;					//Flag for found block
	for (list_iter iterInUse = InUse.begin(); iterInUse != InUse.end(); ++iterInUse)
	{
		//If block is found with correct name and id
		if (iterInUse->blockID == id && iterInUse->proccessID == name)
		{
			blockFound = true;				//flag block found
			AddBlockToAvail(iterInUse);		//Add Block to proper location in list
			MergeBlocks();					//Merge Blocks in Available
			InUse.erase(iterInUse); 		//remove block from InUse
			break;							//if found then break from search
		}
	}
	//print status info
	if (blockFound)
		cout << "DEALLOCATE: ID: " + to_string(id) + " Name: " + name << endl;
	else
		cout << "DEALLOCATE: ID: " + to_string(id) + " Name: " + name << " --ERROR--" << endl;
	return;
}

/*********************************************************
 * Name: Terminate
 * 
 * Purpose: Removes a specified memory block id
 * 	from the inuse list and returns the block to the avail
 * 	memory list
 *********************************************************/
void Terminate(int id)
{
	bool blockFound = false;		//Flag for if block found
	for (list_iter iterInUse = InUse.begin(); iterInUse != InUse.end(); ++iterInUse)
	{
		//if block found
		if (iterInUse->blockID == id)
		{
			blockFound = true;				//flag block found
			AddBlockToAvail(iterInUse);		//Add Block to proper location in list
			MergeBlocks();					//Merge Blocks in Available
			InUse.erase(iterInUse); 		//remove block from InUse
			iterInUse--;
		}
	}
	//Print Status info
	if (blockFound)
		cout << " TERMINATE: ID: " + to_string(id) << endl;
	else
		cout << " TERMINATE: ID: " + to_string(id) << " --ERROR--" << endl;

	return;
}

/*********************************************************
 * Name: PrintMemory
 * 
 * Purpose: Prints out the lists of memory blocks and their
 * 	total allocation
 *********************************************************/
void PrintMemory()
{
	cout << "\n*********************************\n";
	list_iter iter;
	int availTotal = 0;
	int inUseTotal = 0;
	cout << "-------Available Blocks-------";
	//print out all available blocks and total size
	for (iter = Avail.begin(); iter != Avail.end(); ++iter)
	{
		cout << (iter)->toString();
		availTotal += iter->size;
	}

	if (Avail.empty())
		cout << "\n-None";

	cout << "\nTotal Size: " << availTotal;
	cout << "\n-------Blocks in Use-------";
	//print out all InUse blocks and total size
	for (iter = InUse.begin(); iter != InUse.end(); ++iter)
	{
		cout << (iter)->toString();
		inUseTotal += iter->size;
	}

	if (InUse.empty())
		cout << "\n-None";

	cout << "\nTotal Size: " << inUseTotal;
	cout << "\n*********************************\n";
	cout << endl;
}

/*********************************************************
 * Name: MMU (Memory Managment Unit)
 * 
 * Purpose: Reads from the transaction list and calls each
 * 	function based on the operaation specified
 *********************************************************/
void MMU()
{
	int printCount = 0;	//Times Through loop
	PrintMemory();		//Inital Print
	for (list<string>::iterator line = TransactionList.begin(); line != TransactionList.end(); ++line)
	{
		istringstream currentTransaction(*line);	//Retrieve string for parse
		char operation = (*line).at(0); //Operation to Execute

		//variables to read in data about process
		char typeIn;
		int idIn;
		int sizeIn;
		string nameIn;

		//Switch case to control Operation of Memory based on operatin code
		switch (operation)
		{
		case 'L':
			currentTransaction >> typeIn >> idIn >> sizeIn >> nameIn;
			LoadorAllocate(idIn, sizeIn, nameIn, 0);
			break;
		case 'A':
			currentTransaction >> typeIn >> idIn >> sizeIn >> nameIn;
			LoadorAllocate(idIn, sizeIn, nameIn, 1);
			break;
		case 'D':
			currentTransaction >> typeIn >> idIn >> nameIn;
			Deallocate(idIn, nameIn);
			break;
		case 'T':
			currentTransaction >> typeIn >> idIn;
			Terminate(idIn);
			break;
		default:
			cout << "Invalid Read";
		}
		//Print out Lists every 5 Rounds
		printCount++;
		if (printCount % 5 == 0)
		{
			PrintMemory();
		}
	}
	//End Print
	PrintMemory();
}

/*********************************************************
 * Name: Initalize
 * 
 * Purpose: initalizes the availble memory blocks
 *********************************************************/
void Initalize()
{
	memoryBlock initalizer;
	for (int i = 0; i < 5; i++)
	{
		//Set Avail Blocks to 1 MB 2MB 2MB 4MB 4MB
		int initalsize = 0;
		if (i == 0)
			initalsize = MB;
		else if (1 <= i && i < 3)
			initalsize = MB * 2;
		else if (3 <= i && i < 5)
			initalsize = MB * 4;

		initalizer.size = initalsize;
		initalizer.startAddress = startAddress;
		startAddress += initalsize;
		Avail.push_back(initalizer);
	}
}

/*********************************************************
 * Name: BestOrFirst
 * 
 * Purpose: Determains which algorithm to use based
 * 	on the arguments given
 *********************************************************/
void BestOrFirst(int argc, char arg)
{
	//Determine Which Algorithm to use based on argument
	if (argc > 1)
	{
		if (arg == 'b' || arg == 'B')
		{
			cout << "--Best-Fit method--\n";
			bestFitAlgorithm = 1;
		}
		else if (arg == 'f' || arg == 'F')
		{
			cout << "--First-Fit method--\n";
			bestFitAlgorithm = 0;
		}
		else
		{
			cout << "Argument Specification Not Valid" << endl;
			exit(-1);
		}
	}
	else
	{
		cout << "\'f\' = First-Fit, \'b\' = Best-Fit" << endl;
		exit(-1);
	}
}

//Main Process
int main(int argc, char *argv[])
{
	cout << "--Memory Management Simulation--\n";
	BestOrFirst(argc, argv[1][0]);
	ReadFile("datafile.txt");
	Initalize();
	MMU();
	cout << "--Simulation Completed--\n";
	exit(1);
}
