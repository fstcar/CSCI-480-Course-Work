#include "process.h"

	//Constructor
    Process::Process(string processName, uint priority, int arrivalTime, 
                        int processID, pair<char, int> history[])
     {
		this->processName = processName;
		this->priority = priority;
		this->processID = processID;
		this->arrivalTime = arrivalTime;
		
		for(int i = 0; i < ARRAY_SIZE; i++)
        {
		  this->history[i] = history[i];
		}

        historySub = 0;

		CPUTimer = 0;
		IOTimer = 0;
		
		CPUTotal = 0;
		ITotal = 0;
		OTotal = 0;
		
		CPUCount = 0;
		ICount = 0;
		OCount = 0;
	};

	//function to compare the priority of the processes for the prioity queue.
    bool ComparePriority::operator()(const Process* lhs, const Process* rhs)
    {
       if(lhs->priority < rhs->priority) 
        return lhs->priority < rhs->priority;

        return false;
    }

