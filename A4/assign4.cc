/*********************************************
 * Robert Oury
 * CSCI 480 - Section: 0002
 * Assignment 4
 * 10/11/19
 * Z1841079
 * TA Jingwan Li
 * 
 * Purpose: This assignment receives a list of inputs from a file
 * 		and creates 
 *********************************************/

#include "process.h"

//Queues and priority queues for the processes
queue<Process *> entry;
priority_queue<Process *, vector<Process *>, ComparePriority> ready;
priority_queue<Process *, vector<Process *>, ComparePriority> input;
priority_queue<Process *, vector<Process *>, ComparePriority> output;

//Processes in the current active state for cpu input and output
Process *Active;
Process *IActive;
Process *OActive;

//timer for the cpu clock.
int timer;


/********************************************************
 * Function: MoveToReady
 * 
 * Purpose:	Checks if the ready queue has any entires and
 * 	if the entry queue has any entries. if the ready queue
 * 	can fit more processes then move from ready to entry.
 ********************************************************/
void MoveToReady()
{
	while (ready.size() < IN_PLAY && !entry.empty() && entry.front()->arrivalTime <= timer)
	{
		ready.push(entry.front());	//Copy from entry to ready
		printf("%8s %3d | %12s %3s %21s %3d \n", "Process:", entry.front()->processID, "Entry_Queue", "-->", "Ready_Queue | Time:", timer);
		entry.pop();				//Remove from entry.
	}
}

/********************************************************
 * Function: Terminate
 * 
 * Purpose:	Prints out status information about a process
 * 	that has terminated.
 ********************************************************/
void Terminate(int &completedSucessfully)
{
	///Status informtion for the reminated process
	printf("\n%20s\n", "========================");
	printf("%-20s\n", ">>>Process Terminated<<<");
	printf("%-7s %16s\n", "Name:", Active->processName.c_str());
	printf("%-20s %3d\n", "Process ID:", Active->processID);
	printf("%-20s %3d\n", "Priority:", Active->priority);
	printf("%-20s %3d\n", "CPU Time:", Active->CPUTotal);
	printf("%-20s %3d\n", "CPU Burst:", Active->CPUCount);
	printf("%-20s %3d\n", "Input Time:", Active->ITotal);
	printf("%-20s %3d\n", "Input Burst", Active->ICount);
	printf("%-20s %3d\n", "Output Time:", Active->OTotal);
	printf("%-20s %3d\n", "OutPut Burst:", Active->OCount);
	printf("%-20s %3d\n", "Cycle Time:", timer);
	printf("%20s\n\n", "========================");
	completedSucessfully++;
}

/********************************************************
 * Function: CPUBurst
 * 
 * Purpose:	Simulation of a process running on the CPU
 * 	when a process has cpu time adjust counters like 
 * 	timer and cpu time. ALso moves processes into cpu
 * 	if the active entire is NULL
 ********************************************************/
void CPUBurst(int &completedSucessfully, int &CIdle)
{
	if (Active == NULL)
	{
		//There is an entry in the ready queue with a cpu burst next
		if (ready.top()->history[ready.top()->historySub].first == 'C')
		{	
			//Move from ready into active
			Active = ready.top();
			ready.pop();
			printf("%8s %3d | %12s %3s %21s %3d \n", "Process:", Active->processID, "Ready_Queue", "-->", "Active | Time:", timer);
		}
		else
		{
			//if active is null and readt is empty then the cpu is idle
			CIdle++;
			printf("%-45s %7s %3d \n", "Active is Empty, Ready is Empty, CPU Idle", "| Time:", timer);
		}
		return;
	}

	//if the active process still has more cpu time
	if (Active->CPUTimer != Active->history[Active->historySub].second)
	{
		Active->CPUTimer++;
		return;
	}

	//adjust active process counters and timers
	//add cpu time to cpu total.
	Active->CPUTotal += Active->CPUTimer;
	Active->CPUCount++;

	Active->CPUTimer = 0;
	//move through history
	Active->historySub++;

	//next burst after the CPU burst
	int nextBurst = Active->history[Active->historySub].first;

	if (nextBurst == 'I')
	{
		//Move process into input burst
		input.push(Active);
		printf("%8s %3d | %12s %3s %21s %3d \n", "Process:", Active->processID, "Active", "-->", "Input_Queue | Time:", timer);
	}
	else if (nextBurst == 'O')
	{
		//move process into output burst
		output.push(Active);
		printf("%8s %3d | %12s %3s %21s %3d \n", "Process:", Active->processID, "Active", "-->", "Output_Queue | Time:", timer);
	}
	else
	{
		//if the process next burst is null then terminate the process.
		Terminate(completedSucessfully);
	}
	//after that set the active rocess to null.
	Active = NULL;
	return;
}

/********************************************************
 * Function: InputBurst
 * 
 * Purpose:	Simulation of a process in an input burst
 * 	either lets a process use an input burst or moves
 * 	it to another queue. 
 ********************************************************/
void InputBurst(int &IIdle)
{
	if (IActive == NULL)
	{
		if (!input.empty())
		{
			//if there is a process in the input queue then move it into the input process
			IActive = input.top();
			input.pop();
			printf("%8s %3d | %12s %3s %21s %3d \n", "Process:", IActive->processID, "Input_Queue", "-->", "Input | Time:", timer);
		}
		else if (input.empty())
		{
			//if the input queue is empty then the input active is idle
			IIdle++;
		}
		return;
	}

	//if the process has more Input time then adjust input counter
	if (IActive->IOTimer != IActive->history[IActive->historySub].second)
	{
		IActive->IOTimer++;
		return;
	}

	//if this part is reaches then the input is done and the process
	//will be moved back to the ready queue.
	//adjust input counters
	//add input time to the input total
	IActive->ITotal += IActive->IOTimer;
	IActive->ICount++;

	IActive->IOTimer = 0;
	//move through history 
	IActive->historySub++;

	printf("%8s %3d | %12s %3s %21s %3d \n", "Process:", IActive->processID, "Input", "-->", "Ready_Queue | Time:", timer);

	ready.push(IActive);	//move to ready
	IActive = NULL;			//set input active to null
	return;
}

/********************************************************
 * Function: OutputBurst
 * 
 * Purpose:	Simulation of a process in an Output burst
 * 	either lets a process use an Output burst or moves
 * 	it to another queue. 
 ********************************************************/
void OutputBurst(int &OIdle)
{
	if (OActive == NULL)
	{
		if (!output.empty())
		{
			//if the output queue has an entry move it into the output actvie.
			OActive = output.top();
			output.pop();
			printf("%8s %3d | %12s %3s %21s %3d \n", "Process:", OActive->processID, "Output_Queue", "-->", "Output | Time:", timer);
			return;
		}
		else if (output.empty())
		{
			//if output is empty and the output active is null then the output burst is idle.
			OIdle++;
			return;
		}
	}

	//if the process has more output time then allow it to run and increment couter.
	if (OActive->IOTimer != OActive->history[OActive->historySub].second)
	{
		OActive->IOTimer++;
		return;
	}

	//if the functions is here:
	//Increment coutners for the end of the output burst.
	//move to the ready queue.
	//set ototal equal to the time running in output
	OActive->OTotal += OActive->IOTimer;
	OActive->OCount++;

	OActive->IOTimer = 0;
	//move foward in history
	OActive->historySub++;

	printf("%8s %3d | %12s %3s %21s %3d \n", "Process:", OActive->processID, "Output", "-->", "Ready_Queue | Time:", timer);

	//move from the active process to the ready queue.
	ready.push(OActive);
	OActive = NULL;
	return;
}

/********************************************************
 * Function: CompletionCheck
 * 
 * Purpose:	checks if all the queues and active process
 *  are empty and sends back true to end the loop and 
 * 	prints out status report.
 ********************************************************/
bool CompletionCheck()
{
	bool emptyQueues = false;
	bool emptyActives = false;
	if (entry.empty() && ready.empty() && input.empty() && output.empty())
		emptyQueues = true;
	if (Active == NULL && IActive == NULL && OActive == NULL)
		emptyActives = true;
	if (emptyQueues && emptyActives)
		return true;
	else
		return false;
}

/********************************************************
 * Function: EndStateReport
 * 
 * Purpose:	A report of the simulation of a process 
 * cylcle.	showing total number of queues idle time
 *  and processes completes successfully.
 ********************************************************/
void EndStateReport(int completed, int CIdle, int IIdle, int OIdle)
{
	printf("\n\n\n%25s\n", "=============================");
	printf("%-25s\n", "--------Data Summary---------");
	printf("%-25s %3d\n", "Cycles:", timer);
	printf("%-25s %3d\n", "Entry_Queue:", (int)entry.size());
	printf("%-25s %3d\n", "Ready_Queue:", (int)ready.size());
	printf("%-25s %3d\n", "Input_Queue:", (int)input.size());
	printf("%-25s %3d\n", "Output_Queue:", (int)output.size());
	printf("%-25s %3d\n", "Terminated Successfully:", completed);
	printf("%-25s %3d\n", "CPU Idle:", CIdle);
	printf("%-25s %3d\n", "Input Idle:", IIdle);
	printf("%-25s %3d\n", "Output Idle:", OIdle);
	printf("%25s\n", "-----------------------------");
	printf("%25s\n", "=============================");
}

/********************************************************
 * Function: ReadFile
 * 
 * Purpose:	Reads in data from an input file about
 * 	process information such as the name, priority
 * 	arivaltime	processnumber and history array.
 ********************************************************/
void ReadFile(string filename)
{
	string iname;							//input variable for process name
	int ipriority;							//input for priority
	int iarrivalTime;						//arival time
	int processNumber = 101;				//process number counter
	pair<char, int> history[ARRAY_SIZE];	//history array of process times

	//open file and check for valid input
	ifstream infile;
	infile.open(filename);

	if (!infile.is_open())
		exit(-10);

	//for each entry in file gather all the information and then push onto the entry queue array.
	while (infile >> iname >> ipriority >> iarrivalTime)
	{
		//when to stop reading processes.
		if (iname == "STOPHERE")
			break;
		//adding history to the history array.
		for (uint i = 0; i <= 9; i++)
		{
			infile >> history[i].first >> history[i].second;
		}
		//add to the entry queue
		entry.push(new Process(iname, ipriority, iarrivalTime, processNumber, history));
		//new process number
		processNumber++;
	}
	infile.close();
}


/********************************************************
 * Function: StateCheck
 * 
 * Purpose:	Prints out the Process ID of the 
 * 	process in the queues and in the active entries
 ********************************************************/
void StateCheck()
{
	//temperary variables to view current contents of the queues
	queue<Process *> entry_temp = entry;
	priority_queue<Process *, vector<Process *>, ComparePriority> ready_temp = ready;
	priority_queue<Process *, vector<Process *>, ComparePriority> input_temp = input;
	priority_queue<Process *, vector<Process *>, ComparePriority> output_temp = output;

	cout << "\n******************************";
	cout << "\n******* Status Report ********\n";

	//Print out the ID's of all the Active processes
	if (Active == NULL)
		cout << "\n Active Process ID: NULL\n";
	else
		cout << "\n Active Process ID: " << Active->processID << endl;

	if (IActive == NULL)
		cout << "\n  Input Process ID: NULL\n";
	else
		cout << "\n  Input Process ID: " << IActive->processID << endl;
	if (OActive == NULL)
		cout << "\n Output Process ID: NULL\n";
	else
		cout << "\n Output Process ID: " << OActive->processID << endl;

	//For each queue print out their process ID.
	cout << "******************************";
	cout << "\n  Entry Queue: ";

	while (entry_temp.size() != 0)
	{
		cout << entry_temp.front()->processID << " ";
		entry_temp.pop();
	}

	cout << "\n\n  Ready Queue: ";

	while (ready_temp.size() != 0)
	{
		cout << ready_temp.top()->processID << " ";
		ready_temp.pop();
	}

	cout << "\n\n  Input Queue: ";

	while (input_temp.size() != 0)
	{
		cout << input_temp.top()->processID << " ";
		input_temp.pop();
	}

	cout << "\n\n Output Queue: ";

	while (output_temp.size() != 0)
	{
		cout << output_temp.top()->processID << " ";
		output_temp.pop();
	}
	cout << "\n******************************";
	cout << "\n******************************\n\n";
}

/********************************************************
 * Function: Main
 * 
 * Purpose:	Loops throught the various fucntions for the 
 * 	bursts and passes the variables of idle time and 
 * 	sucessfull completion to them.
 ********************************************************/
int main(int argc, char **argv)
{
	int completedSucessfully = 0;	//how many processes completesd sucessfully
	int CIdle = 0;			//how much time the cpu spent idle
	int IIdle = 0;			//how long the input spent idle
	int OIdle = 0;			//how long the output was idle

	//checks for valid arguments.
	if (argc < 2)
	{
		cerr << "Input A Valid File Name\n";
		return 1;
	}

	//read in file
	ReadFile(argv[1]);

	//loop while timer is less then max time
	//if simulation is complete exit loop
	while (timer <= MAX_TIME && !CompletionCheck())
	{
		//print out status report every few iterations
		if (timer % HOW_OFTEN == 0)
			StateCheck();
		//move any process to ready queue from entry if there is room
		MoveToReady();
		//run a cpu burst
		CPUBurst(completedSucessfully, CIdle);
		//run a input burst
		InputBurst(IIdle);
		//run a output burst
		OutputBurst(OIdle);
		//increment timer of cpu time..
		timer++;
	}
	//print out an end state report of the simulation
	EndStateReport(completedSucessfully, CIdle, IIdle, OIdle);
}