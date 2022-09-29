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
#include <string.h>
#include <strings.h>

#define INITIAL_ARRAY_SIZE  500
#define QTY_TOP_WORDS        10

// You may find this Useful
char * delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";

typedef struct 
{
    char *word;
    int freq;
} word_freq;

typedef struct
{
    word_freq *arr;
    size_t used;
    size_t size;
} word_arr;

typedef struct
{
    char *text;
    volatile word_arr *words;
    pthread_t thread_id;
    pthread_mutex_t mutex;
    int thread_num;
    int chunk_size;
} thread_info;

void init_array(volatile word_arr *words, size_t size);

void insert_array(volatile word_arr *words, word_freq *word);

void free_array(volatile word_arr *words);

void free_tinfo(thread_info *tinfo);

void *test_array( volatile word_arr *words, char *text );

void *process_chunk( void *arg );

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
    void *res;

    // Open file provided in first argument
    FILE *in_file = fopen(argv[1], "r");

    // Initialize the word count array
    volatile word_arr *word_freq_arr;
    init_array(word_freq_arr, INITIAL_ARRAY_SIZE);

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

    // pthread_mutex_t mutex;
    // if (pthread_mutex_init(&mutex, NULL) != 0) {
    //     perror("Error: mutex init failed.");
    //     return EXIT_FAILURE;
    // }

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************
    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish

    // thread_info *tinfo = (thread_info*)calloc(thread_count, sizeof(*tinfo));
    // if (tinfo == NULL)
    // {
    //     perror("Error: calloc failed");
    //     return EXIT_FAILURE;
    // }

    // for (int i = 0; i < thread_count; i++)
    // {
    //     tinfo[i].text = buffer;
    //     tinfo[i].words = word_freq_arr;
    //     tinfo[i].chunk_size = file_length / thread_count;
    //     tinfo[i].thread_num = i;
    //     tinfo[i].mutex = mutex;
    //     if(pthread_create(&tinfo[i].thread_id, NULL, process_chunk, &tinfo[i]))
    //     {
    //         perror("Error: pthread_create failed");
    //     };
    // }

    // for (int i = 0; i < thread_count; i++)
    // {
    //     if(pthread_join(tinfo[i].thread_id, &res))
    //     {
    //         perror("Error: pthread_join failed");
    //         return EXIT_FAILURE;
    //     };
    // }
    test_array(word_freq_arr, buffer);

    printf("First 10 words in array");
    for (int i = 0; i < word_freq_arr->used; i++) {
        if (i < 10) {
            printf("'%s': %d", word_freq_arr->arr[i].word, word_freq_arr->arr[i].freq);
        }
    }

    // ***TO DO *** Process TOP 10 and display
    // word_arr *highest_counts = malloc(QTY_TOP_WORDS * sizeof(word_freq));
    // word_freq_arr->size;

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
    // free_tinfo(tinfo);
}

void init_array(volatile word_arr *words, size_t size)
{
    words->arr = malloc(size, sizeof(char));
    words->used = 0;
    words->size = size;
}

void insert_array(volatile word_arr *words, word_freq *word)
{
    if ( words->used == words->size ) {
        words->size *= 2;
        words->arr = realloc(words->arr, words->size * sizeof(word_freq));
    }
    words->arr[words->used++].word = word;
};

void free_array(volatile word_arr *words) 
{
    for (int i = 0; i < words->used; i++ ) {
        free(words->arr[i].word);
        words->arr[i].word = NULL;
    }
    free(words->arr);
    words->arr = NULL;
    words->used = words->size = 0;
};

void free_tinfo(thread_info *tinfo)
{
    free(tinfo->text);
    tinfo->text = NULL;
    free_array(tinfo->words);
    tinfo->chunk_size = tinfo->thread_num = 0;
}

void *test_array( volatile word_arr *words, char *text )
{
    char *chunk = malloc(words->size/5);
    chunk = strncpy(chunk, text, words->size/5);
    char *lasts;
    // Largest dictionary word is 49, so we initialize a byte size that is slightly greater
    char *token = strtok_r(chunk, delim, &lasts);
    while (token) {
        if (strlen(token) > 5) {
            word_freq *new_word = malloc(sizeof(word_freq));
            new_word->word = malloc(strlen(token));
            new_word->freq = 1;
            if (words->used == words->size) {
                words->size *= 2;
                words->arr = realloc(words->arr, words->size * sizeof(word_freq));
            }
            words->arr[words->used++].word = new_word;
        }
        token = strtok_r(NULL, delim, &lasts);
    }
    free(chunk);
    chunk = NULL;
};

void *process_chunk( void *arg )
{
    thread_info *tinfo = arg;
    char *chunk = malloc(tinfo->chunk_size);
    char *chunk_start = tinfo->text + (tinfo->chunk_size * tinfo->thread_num);
    strncpy(chunk, chunk_start, tinfo->chunk_size);
    char *lasts;
    // Largest dictionary word is 49, so we initialize a byte size that is slightly greater
    char *token = strtok_r(chunk, delim, &lasts);
    while (token) {
        if (strlen(token) > 5) {
            char exists = 0;
            for (int i = 0; i < tinfo->words->used; i++)
            {
                // {
                //     exists = 1;
                //     if(pthread_mutex_lock(&tinfo->mutex))
                //     {
                //         perror("Error: mutex lock failed.");
                //     };
                //     tinfo->words->arr[i].freq++;
                //     if(pthread_mutex_unlock(&tinfo->mutex))
                //     {
                //         perror("Error: mutex failed to unlock");
                //     };
                // }
                // if (!strcasecmp(tinfo->words->arr[i].word, token))
                // {
                //     exists = 1;
                //     if(pthread_mutex_lock(&tinfo->mutex))
                //     {
                //         perror("Error: mutex lock failed.");
                //     };
                //     tinfo->words->arr[i].freq++;
                //     if(pthread_mutex_unlock(&tinfo->mutex))
                //     {
                //         perror("Error: mutex failed to unlock");
                //     };
                // }
            }
            
            // if (!exists) {
            //     if (pthread_mutex_lock(&tinfo->mutex))
            //     {
            //         perror("Error: mutex lock failed.");
            //     };
            //     word_freq *new_word = malloc(sizeof(word_freq));
            //     new_word->word = malloc(strlen(token));
            //     new_word->freq = 1;
            //     insert_array(tinfo->words, new_word);
            //     if(pthread_mutex_unlock(&tinfo->mutex))
            //     {
            //         perror("Error: mutex failed to unlock");
            //     };
            // }
        }
        token = strtok_r(NULL, delim, &lasts);
    }
    free(chunk);
    chunk = NULL;
};