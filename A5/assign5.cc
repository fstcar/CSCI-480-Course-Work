/*********************************************
 * Robert Oury
 * CSCI 480 - Section: 0002
 * Assignment 5
 * 10/25/19
 * Z1841079
 * TA Jingwan Li
 * 
 * Purpose: This program is a simulation of
 *  the producer and consumer problem with a 
 *  buffer shared by both.
 *********************************************/

#include<iostream>
#include<semaphore.h>
#include<unistd.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<iomanip>
#include<mutex>
#include<vector>
#include<queue>

using namespace std;


struct Widget
{
    int producer;       //Thread that created widget
    int widgetnumber;   //Number of created widget

    //constuctor
    Widget(int producer, int widgetnumber)
    {
     this->producer = producer;
     this->widgetnumber = widgetnumber;
    };

    //toString for printing widget details
    string toString()
    {
    return ("  P" + to_string(producer) + "W" + to_string(widgetnumber));
    };

};

#define BUFFER_SIZE 35  //Maximum Buffer contents
int P_NUMBER = 7;       //Number of producer threads
int C_NUMBER = 5;       //number of consumer threads
int P_STEPS = 5;        //number of producer steps
int C_STEPS = 7;        //number of consumer steps

sem_t notFull;          //semephore to signify buffer isnt full
sem_t notEmpty;         //signify buffer isnt empty

int widgetcount;        //number of widgets in buffer

pthread_mutex_t mutex1; //mutex to monitor thread completion

queue<Widget *> buffer; //queue of widgets

//function declerations
void Insert(int ID, int widgetnumber);    
void Remove(int ID);
void * Produce(void * ID);
void * Consume(void * ID);

/********************************************************
 * Function: PrintBuffer
 * 
 * Purpose:	Copies buffer into temp queue and prints out
 *  total contents calling tostring.
 ********************************************************/
void PrintBuffer()
{
    queue<Widget *> tempBuffer = buffer;    //temp queue
    cout << "Buffer: ";

    if(tempBuffer.size() == 0)
    {
        cerr << "  Empty";
    }
    //print out every widget
    while (tempBuffer.size() != 0)
	{
		cerr << tempBuffer.front()->toString() << " ";
		tempBuffer.pop();
	}
    cout << endl<<endl;
}

/********************************************************
 * Function: Produce
 * 
 * Purpose:	Creates widgets for consumer threads by calling
 *  inset function.
 ********************************************************/
void * Produce(void * ID)
{
    int widgetnumber = 1; 
    for (uint i = 0; i < P_STEPS; i++)
    {
        sleep(1);   //time to see widget creation
        sem_wait(&notFull); //wait untill buffer isnt full
        Insert((long) ID, widgetnumber);  //insert a widget
        widgetnumber++;
        sem_post(&notEmpty);//signal buffer isnt empty
    }
    pthread_exit(NULL);
}

/********************************************************
 * Function: Consume
 * 
 * Purpose:	Removes widgets from queue when buffer isnt
 *  empty
 ********************************************************/
void * Consume(void * ID)
{
    for (uint i = 0; i < C_STEPS; i++)
    {
        sleep(1);      //time to see widget removal
        sem_wait(&notEmpty);    //wait untill buffer isnt empty
        Remove((long) ID);      //remove item from buffer
        sem_post(&notFull);     //signal buffer isnt full
    }
    pthread_exit(NULL);
}

/********************************************************
 * Function: Insert
 * 
 * Purpose:	If no other threads are accessing the buffer then
 *  add a widget to it
 ********************************************************/
void Insert(int ID, int widgetnumber)
{
    pthread_mutex_lock(&mutex1);    //lock threads
    buffer.push(new Widget(ID+1, widgetnumber));    //add widget to buffer
    widgetcount++;  //increment widget counter
    cerr << "Producer " << ID+1 << " added one item. Count: " << widgetcount << endl;
    PrintBuffer();  //print contents of buffer
    pthread_mutex_unlock(&mutex1);  //unlock threads
}

/********************************************************
 * Function: Remove
 * 
 * Purpose:	If no other threads are accessing the buffer 
 *  then remove a widget from the buffer
 ********************************************************/
void Remove(int ID)
{
    pthread_mutex_lock(&mutex1);    //lock other threads
    /*
    *   This if statement is nessesary since the program
    *   seems to not lock and unlock the semephore as
    *   it should and this function will attempt to 
    *   access an empty buffer ==> segmentation fault.
    */
    if(buffer.empty()) 
    {
        cerr << "\nError accessing buffer while empty\n";
        exit(-5);
    }

    widgetcount--;  //decrement widget counter
    buffer.pop();   //remove widget from buffer

    cerr << "Consumer " << ID+1 << " removed one item. Count: " << widgetcount << endl;
    PrintBuffer();  //print buffer contents

    pthread_mutex_unlock(&mutex1);  //unlock threads
}


int main(int argc, char *argv[])
{
    cout << "Producer and Consumer Simulation" << endl;

    cout << "\nHow many producers: ";
    cin >> P_NUMBER;

    cout << "\nHow many steps in the producer threads:";
    cin >> P_STEPS;
    
    cout << "\nHow many consumers: ";
    cin >> C_NUMBER;

    cout << "\nHow many steps in the consumer threads:";
    cin >> C_STEPS;

    //checking if the thread iterations are equivalent
    if(P_NUMBER * P_STEPS != C_NUMBER * C_STEPS)
    {
        cout << "Error, Not Equal Iterations of threads" << endl;
        exit(-1);
    }

    //instantiate the mutex and semaphores
    pthread_mutex_init(&mutex1, NULL);

    sem_init(&notFull, 0, BUFFER_SIZE);

	sem_init(&notEmpty, 0, 0);

    //create array of producer and consumer threads
    pthread_t C_Threads[C_NUMBER];
    pthread_t P_Threads[P_NUMBER];


    //add new producer threads based on P_NUMBER
    for (int i = 0; i < P_NUMBER; i++)
    {
        if(pthread_create(&P_Threads[i], NULL, Produce, (void *) i))
        {
            cout << "Error in creating thread";
            exit(-1);
        }
    }
    //Allow producer threads to complete one iteration before 
    //creating consumer threads
    sleep(1);

    //add new consumer threads based on C_NUMBER
    for (int i = 0; i < C_NUMBER; i++)
    {
        if(pthread_create(&C_Threads[i], NULL, Consume, (void *) i))
        {
            cout << "Error in creating thread";
            exit(-1);
        }
    }

    //Main threads joins and waits for children to complete
    for (uint i = 0; i < P_NUMBER; i++)
    {
        pthread_join(P_Threads[i], NULL);
    }

    for (uint i = 0; i < C_NUMBER; i++)
    {
        pthread_join(C_Threads[i], NULL);
    }


    //complete simulation and remove mutex and semephores.
    cout << "\nSimulation Completed\n";
    pthread_mutex_destroy(&mutex1);
	pthread_exit(NULL);

	sem_destroy(&notFull);
	sem_destroy(&notEmpty);

    return(0);
}
