/**************************************************************
* Class:  CSC-415-02 Fall 2022
* Name: Mark Kim
* Student ID: 918204214
* GitHub ID: mkim797
* Project: Assignment 4 – Word Blast
*
* File: Kim_Mark_HW4_main.c
*
* Description:
*
**************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE     128

// You may find this Useful
char * delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";

typedef struct 
{
    char *word;
    int freq;
} word_count;

int main (int argc, char *argv[])
    {
    //***TO DO***  Look at arguments, open file, divide by threads
    //             Allocate and Initialize and storage structures
    char *token;

    FILE *in = fopen(argv[1], "r");
    if (in == NULL) 
    {
        perror("Error: file could not be opened.");
    };
    word_count *words_v[] = malloc( sizeof(word_count) * 512 );
    int words_c = 0;
    token = strtok(buf, " \n\t\"");
        sh_argc = 0;
        while (token != NULL) {
            sh_argv[sh_argc++] = token;
            token = strtok(NULL, " \n\t\"");
        }
        sh_argv[sh_argc++] = NULL;

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************
    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish


    // ***TO DO *** Process TOP 10 and display

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Clock output
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;
    if (endTime.tv_nsec < startTime.tv_nsec)
        {
        --sec;
        n_sec = n_sec + 1000000000L;
        }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
    //**************************************************************


    // ***TO DO *** cleanup
    }

int 
