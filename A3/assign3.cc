/********************************************************
 * Robert Oury
 * CSCI 480 - Section: 0002
 * Assignment 3
 * 09/26/19
 * Z1841079
 * TA: Jingwan Li
 * 
 * Purpose: This program is a basic microshell which 
 *              can accept linux commands and create a 
 *              special pipe between them with the ||   
 *              designation.
 ********************************************************/

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>

using namespace std;

/********************************************************
 * Function: Main
 * 
 * Purpose:	Runs entirety of microshell within the main
 *          function. Basic structure is a while loop
 *          receiving continous input from the user
 *          and proccessing commands given.
 ********************************************************/
int main(void)
{
    char buf[1024];                             //buffer to read in from cin
    int pipeA[2];                               //pipe between both processes
    int statusA;                                //status of first child proccess
    int statusB;                                //status of the second child proccess
    pid_t pidA;                                 //Pid of child proccess 1
    pid_t pidB;                                 //Pid of child proccess 2
    printf("480shell> ");                       //Print out shell prompt

    //Contiously receive input from User for new commands until entered
    while (fgets(buf, 1024, stdin) != NULL)
    {
        char * bufTokens;                       //Splits user input into tokens
        char * command1[] = {NULL,NULL,NULL};   //first command array
        char * command2[] = {NULL,NULL,NULL};   //second command array

        bool pipeRequired = false;              //flag for required pipe
        int arguments = 0;                      //Tracking number of arguments
        pipe(pipeA);                            //Create pipeA

        buf[strlen(buf) - 1] = 0;               //Remove last character of input
        bufTokens = strtok(buf, " ");           //Tokenize buffer

        if (buf[0] == 'q' || strcmp(buf, "quit") == 0)
        {
            //if the exit command is issued close program
            exit(0);
        }

        //Until all tokens have been parsed through
        while (bufTokens != NULL) 
        { 
            if(strcmp(bufTokens, "||") == 0)    //if Pipe exists
            {
                pipeRequired = true;            //flag for pipe
                arguments = -1;                 //reset argument count
            }
            else
            {
            if(pipeRequired)
            {
                command2[arguments] = bufTokens;//Pipe found add to second array
            }
            else
            {
                command1[arguments] = bufTokens;//Pipe !found add to first array
            }
            }
            arguments++;                        //increment argument count
            bufTokens = strtok(NULL, " ");      //Parse tokens
        } 

        //create fork for first child proccess
        if ((pidA = fork()) < 0)
        {
            //if failed then print error and exit
            printf("Fork 1 Failed, Exiting Now!");
            exit(-5);
        }
        else if (pidA == 0)
        { 
            /* Child 1 */
            if(pipeRequired)
            {
                //redirect output to pipe and close read end of pipe
                dup2(pipeA[1], STDOUT_FILENO);
                close(pipeA[0]);
            }
            execvp(command1[0], command1);  //execute command 1
            //Bellow should be unreachable unless command failed 
            printf("Couldn't execute: %s\n", command1[0]);
            exit(127);
        }
        /* Parent */
        if(pipeRequired)
         {
             //create second proccess to handle second command
            if((pidB = fork()) < 0)
            {
                //if failed then print error and exit
                printf("Fork 2 Failed, Exiting Now!");
                exit(-5);
            }
            else if(pidB == 0)
            {
            /* Child 2 */
            //redirect input receiving from pipe and close write end
            dup2(pipeA[0], STDIN_FILENO);
            close(pipeA[1]);

            execvp(command2[0], command2);  //execute command 2
            //Bellow should be unreachable unless command failed 
            printf("Couldn't execute: %s\n", command2[0]);
            exit(127);
            }
        }


        close(pipeA[0]);                    //close remaining ends of pipeA
        close(pipeA[1]);

        waitpid(pidA, &statusA, 0);         //Wait for proccess A and B to terminate
		waitpid(pidB, &statusB, 0);

        printf("480shell> ");               //print out new prompt and restart loop
    }
    exit(0);                                //exit upon succesful completion
}
