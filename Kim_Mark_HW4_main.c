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

#define INITIAL_ARRAY_SIZE  500

// You may find this Useful
char * delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";

typedef struct 
{
    char *word;
    int freq;
} Word_Freq;

typedef struct
{
    Word_Freq *arr;
    size_t used;
    size_t size;
} Word_Arr;

void init_array(Word_Arr *words, size_t size);

void insert_array(Word_Arr *words, Word_Freq word);

void free_array(Word_Arr *words);

int main (int argc, char *argv[])
{
    //***TO DO***  Look at arguments, open file, divide by threads
    //             Allocate and Initialize and storage structures

    // Two arguments are required; check to see if two arguments are supplied.
    if (argc < 3) 
    {
        perror("Error: less than the two arguments required were found.");
        return EXIT_FAILURE;
    };

    // Pull the number of threads to use from second argument
    const int thread_count = strtol(argv[2], NULL, 10);

    // Open file provided in first argument
    FILE *in_file = fopen(argv[1], "r");

    // Initialize the word count array
    volatile Word_Arr *word_freq;
    init_array(word_freq, INITIAL_ARRAY_SIZE);

    // Check to see if fopen succeeds
    if (!in_file) 
    {
        perror("Error: file could not be opened.");
        return EXIT_FAILURE;
    };

    // Move to end of file to get file length and move pointer back to the beginning
    fseek(in_file, 0, SEEK_END);
    const int file_length = ftell(in_file);
    fseek(in_file, 0, SEEK_SET);

    // Reserve sufficient memory to store the entire text file
    char *buffer = malloc(file_length);
    if (!buffer)
    {
        perror("Error: malloc failed.");
        return EXIT_FAILURE;
    };

    fread(buffer, sizeof(char), file_length, in_file);
    fclose(in_file);

    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Error: mutex init failed.");
        return EXIT_FAILURE;
    }

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************
    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish

    pthread_t *threads = (pthread_t*)malloc(thread_count * sizeof(pthread_t));



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
    free(buffer);
    buffer = NULL;
}

void init_array(Word_Arr *words, size_t size)
{
    words->arr = malloc(sizeof(Word_Freq) * size);
    words->used = 0;
    words->size = size;
}

void insert_array(Word_Arr *words, Word_Freq word)
{
    if ( words->used == words->size ) {
        words->size *= 2;
        words->arr = realloc(words->arr, words->size * sizeof(Word_Freq));
    }
};

void free_array(Word_Arr *words) 
{
    free(words->arr);
    words->arr = NULL;
    words->used = words->size = 0;
};



// void *process_chunk(int thread_num, int chunk_size, void *text, Word_Arr *words)
// {
//     char *chunk = malloc(chunk_size);
//     char *chunk_start = text + (chunk_size * thread_num);
//     strncpy(chunk, chunk_start, chunk_size);
//     char *lasts;
//     // Largest dictionary word is 49, so we initialize a byte size that is slightly greater
//     char *token = strtok_r(chunk, delim, &lasts);
//     while (token != NULL) {
//         if (strlen(token) > 5) {
//             for (int i = 0; i < words.used; i++) {
//                 if (stcasecmp(words.))
//             }
//         }
//     }
// };