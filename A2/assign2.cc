/********************************************************
 * Robert Oury
 * CSCI 480 - Section: 0002
 * Assignment 2
 * 09/17/19
 * Z1841079
 * TA: Jingwan Li
 * 
 * Purpose: This program is designed to use pipe function 
 *              of the linux os to send a string in a 
 *              circle to do basic arethmetic
 ********************************************************/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
using namespace std;

/********************************************************
 * Function: Calculate
 * 
 * Purpose:	Preforms the basic arethmetic based on which
 * 				fork called the method.
 ********************************************************/
void Calculate(long &m, uint fork)
{
	if(fork == 0)	   m = (3 * m) + 7;
	else if(fork == 1) m = (2 * m) + 5;
	else if(fork == 2) m = (5 * m) + 1;
	else cerr << "Invalid fork #";
}
/********************************************************
 * Function: PipeWork
 * 
 * Purpose:	Combines the Piping and conversion components
 * 			of the multiple pipes taking place and calls
 * 			the Calculate function to preform the
 * 			arethmetic on the value passed though the
 * 			pipes.
 ********************************************************/
bool PipeWork(int writeStream, int readStream, uint fork)
{
	string buffer;	//String to send value through pipe
	string value;	//String to store full piped number
	char tempChar;	//Temperary Char to read bytewise from pipe
	long m;			//Long int for calculations
	const long maxValue = 99999999999; //max value of m variable

	//Read Individual Char from pipe into tempChar
	while(read(readStream, &tempChar, 1) > 0){
			if(tempChar == '@')	//If delimeter stop reading
				break;
			value.push_back(tempChar); //Add Char read to string
		}

		try{
			m = stol(value);	//Convert string to int
		}
		catch(const invalid_argument){
			//if conversion fails exit loop
			return(false);
		}

		Calculate(m, fork);		//calculate new number

		if (m >= maxValue)
		{	//If number is larger than max
			write(writeStream, "*@", 2);	//Send empty String to pipe to break loop
			return(false);	//Exit Current Loop
		}

		buffer = to_string(m);	//convert number to string
		//Determaines which fork is this.
		switch(fork)
		{
		case 0: cerr << "Parent        "; break;
		case 1: cerr << "Child         "; break;
		case 2: cerr << "GrandChild    "; break;
		}
		cerr << "Value: " << m <<endl;	//Print out current number
		write(writeStream, buffer.c_str(), buffer.length());	//send buffer through pipe
		write(writeStream, "@", 1);	//send delimeter though pipe
		value.clear();	//clear value and buffer variables
		buffer.clear();	
		return(true);	//return true to continue while loop
}

/********************************************************
 * Function: GWork
 * 
 * Purpose:	Calls the PipeWork Function as long as the
 * 			function doesnt return false.
 * 			Returns if: Value is too Large
 * 						Exit if passed through pipe
 ********************************************************/
void GWork(int writeStream, int readStream)
{
	cerr << "Grandchild process ready to proceed.\n";	
	while(PipeWork(writeStream, readStream, 2)); //Contiously look for more input
}

/********************************************************
 * Function: CWork
 * 
 * Purpose:	Calls the PipeWork Function as long as the
 * 			function doesnt return false.
 * 			Returns if: Value is too Large
 * 						Exit if passed through pipe
 ********************************************************/
void CWork(int writeStream, int readStream)
{
	cerr << "Child process ready to proceed.\n";	
	while(PipeWork(writeStream, readStream, 1)); //Contiously look for more input
}

/********************************************************
 * Function: PWork
 * 
 * Purpose:	Starts the Pipe Circle by sending an inital
 * 			value of 1 through the pipe.
 * 
 * 			Calls the PipeWork Function as long as the
 * 			function doesnt return false.
 * 			Returns if: Value is too Large
 * 						Exit if passed through pipe
 ********************************************************/
void PWork(int writeStream, int readStream)
{
	cerr << "Parent process ready to proceed.\n";
	string firstbuffer = "1@";	//String for Starting pipe Loop
	cerr << "Parent        Value: " << 1 <<endl;	//Display first value
	write(writeStream, firstbuffer.c_str(), firstbuffer.length());	//send buffer to child	
	while(PipeWork(writeStream, readStream, 0));	//Contiously look for more input
}

/********************************************************
 * Function: Main
 * 
 * Purpose:	Creates the Forks and calls the subsequent
 * 			functions related the forks created.
 ********************************************************/
int main()
{
    //declare pipes
    int parentpipe[2],
        childpipe[2],
        grandchildpipe[2];

    pid_t pid; //store pid of forks

    //Create Pipes
    if(pipe(parentpipe))
    {
		cerr << "Parent Pipe Unable to be Created" <<endl;
		exit(-5);
	}
     if(pipe(childpipe))
    {
		cerr << "Child Pipe Unable to be Created" <<endl;
		exit(-5);
	}
     if(pipe(grandchildpipe))
    {
		cerr << "Grandchild Pipe Unable to be Created" <<endl;
		exit(-5);
	}
    //End creating Pipes

    pid = fork(); //Create child proccess
    if(pid < 0) //fork failed
    {
        cerr << "Child Fork Failed, Exiting Now.\n";
        exit(-1);
    }
    else if(pid == 0) //Child begin "GrandChild" generating
    {
        pid_t gpid = fork(); //Create grandchild proccess
        if(pid < 0) //fork failed
        {
            cerr << "GrandChild Fork Failed, Exiting Now.\n";
            exit(-1);
        }
        else if(gpid == 0) //Grandchild Working proccess
        {
            close(grandchildpipe[0]);       		//Close Read End of Pipe
			close(childpipe[1]);            		//Close Write end of Pipe
			GWork(grandchildpipe[1], childpipe[0]);	//Grandchild Work Function
			close(grandchildpipe[1]);	    		//Close Pipe after Use
			close(childpipe[0]);            		//Continue closing
			exit(0);								//End Grandchild
        }
        else //Child  Working proccess
        {
            close(childpipe[0]);   				    //Close Read End of Pipe
			close(parentpipe[1]);           	 	//Close Write end of Pipe
			CWork(childpipe[1], parentpipe[0]);		//Child Work Function
			close(childpipe[1]);	    			//Close Pipe after Use
			close(parentpipe[0]);            		//Continue closing
			exit(0);								//End Child
        }
    }
    else //Parent Working proccess
    {
        close(parentpipe[0]);      					//Close Read End of Pipe
        close(grandchildpipe[1]);           		//Close Write end of Pipe
        PWork(parentpipe[1], grandchildpipe[0]);	//Parent Work Function
        close(parentpipe[1]);	    				//Close Pipe after Use
        close(grandchildpipe[0]);            		//Continue closing
        exit(0);									//End Parent
    }



    return(0);
}