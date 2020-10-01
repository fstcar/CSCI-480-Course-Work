#ifndef PROCESS_H
#define PROCESS_H

#include<iostream>
#include<string>
#include<queue>
#include<fstream>
#include<vector>
#include<istream>
using namespace std;

const int MAX_TIME = 500;   //max time to run the simulation for
const int IN_PLAY = 6;      //amount of process in the ready queu at once.
const int QUEUE_SIZE = 20;  //largest queue size
const int ARRAY_SIZE = 10;  //size of the history array
const int HOW_OFTEN = 25;   //how many cpu cycles to print out status report.

struct Process
{
    string processName; //name of the process
    uint priority;      //priority for moving through the queue
    int processID;      //id for process
    int arrivalTime;    //time the process entered the ready queue.

    pair<char, int> history[ARRAY_SIZE];    //array of process bursts
    int historySub; //counter for moving throught the history array

    int CPUTimer;   //time spend on cpu
	int IOTimer;    //time spend in input and output
	int CPUTotal;   //total time on cpu
	int ITotal;     //total time on input
	int OTotal;     //total time on output
	int CPUCount;   //bursts on cpu
	int ICount;     //number of input bursts
	int OCount;     //number of output bursts

    
    Process(string processName, uint priority, int arrivalTime, 
                        int processID, pair<char, int> history[]);

};

struct ComparePriority
{   
     bool operator()(const Process* lhs, const Process* rhs);
};

//function declerations for main function.
void ReadFile(string filename);
void StateCheck();
void MoveToReady();
void CPUBurst(int &completedSucessfully, int &CIdle);
void InputBurst(int &IIdle);
void OutputBurst(int &OIdle);
void EndStateReport(int completed, int CIdle, int IIdle, int OIdle);
bool CompletionCheck();
void Terminate(int &completedSucessfully);

#endif