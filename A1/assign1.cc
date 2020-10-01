/*********************************************
 * Robert Oury
 * CSCI 480 - Section: 0002
 * Assignment 1
 * 09/06/19
 * Z1841079
 * TA: Jingwan Li
 * 
 * Purpose: This assignment involves using LINUX system functions
 * 	such as fork(), getpid(), getppid(), wait() and system().
 * 	This program Forks twice to create 3 diffent proccesses 
 *********************************************/
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;
//Main Process
int main()
{
	//Base proccess PID
	cerr << "\nTest I am the original process.  My PID is  " + to_string(getpid());
	cerr << "   and my parent's PID is  " + to_string(getppid());
	cerr << "\nNow we have the first fork.";

	int child = fork();		//fork into child

	//If fork fails then exit
	if (child < 0)
	{
		cerr << "The first fork failed.";
		return -1;
	}
	//If fork succeeds
	if (child == 0)
	{
		//Print Child and parent's PIDs
		cerr << "\nI am the child. My PID is " + to_string(getpid());
		cerr << "  and my parents's PID is  " + to_string(getppid());
		cerr << "\nNow we have the second fork.";

		int grandchild = fork(); //fork into grandchild

		//if second fork fails then exit
		if (grandchild < 0)
		{
			cerr << "The second fork failed.";
			return -1;
		}
		//If fork succeeds
		if (grandchild == 0)
		{
			//Print out grandchild and child PIDs
			cerr << "\nI am the grandchild. My PID is " + to_string(getpid());
			cerr << "  and my parents's PID is  " + to_string(getppid());
			cerr << "\nI am the grandchild, about to exit.";
			return 0;
		}
		else
		{
			//Print out child and parent PIDs
			cerr << "\nI am the child. My PID is " + to_string(getpid());
			cerr << "  and my parents's PID is  " + to_string(getppid());
			//wait for grandchild to finish
			wait(0);
			cerr << "\nI am the child, about to exit.";
			return 0;
		}
	}
	else
	{
		//If in parent proccess
		cerr << "\nI am the parent. My PID is " + to_string(getpid());
		cerr << "  and my parents's PID is  " + to_string(getppid());
		//pauses proccess for 2 seconds
		sleep(2);
		cerr << "\nI am the parent, about to call ps.\n";
		//call ps from system process
		system("ps");
		//wait untill child proccess completes
		wait(0);
		cerr << "\nI am the parent, having waited on the child, about to call ps again.\n";
		//call ps once finished
		system("ps");
		cerr << "\nI am the parent, about to exit.\n";
		//return to caller
		return 0;
	}
	cerr << '\n';
	return 0;
}
