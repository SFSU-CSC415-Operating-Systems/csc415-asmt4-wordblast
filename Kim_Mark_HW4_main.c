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
    volatile size_t used;
    size_t size;
} array;

typedef struct
{
    char *text;
    array *words;
    pthread_t thread_id;
    pthread_mutex_t mutex;
    int thread_num;
    int chunk_size;
} thread_info;

void init_array(array *words, size_t size);

void insert_array(array *words, char *token);

void free_array(array *words);

void free_tinfo(thread_info *tinfo);

void *test_array( array *words, char *text );

void *process_chunk( void *arg );

int max( int a, int b);

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
    array *words = malloc(sizeof(array));
    init_array(words, INITIAL_ARRAY_SIZE);

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

    size_t num_elements = fread(buffer, sizeof(char), file_length, in_file);
    printf("Number of Elements: %lu\n", num_elements);
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

    thread_info *tinfo = (thread_info*)calloc(thread_count, sizeof(*tinfo));
    if (tinfo == NULL)
    {
        perror("Error: calloc failed");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < thread_count; i++)
    {
        tinfo[i].text = buffer;
        tinfo[i].words = words;
        tinfo[i].chunk_size = file_length / thread_count;
        tinfo[i].thread_num = i;
        tinfo[i].mutex = mutex;
        if(pthread_create(&tinfo[i].thread_id, NULL, process_chunk, &tinfo[i]))
        {
            perror("Error: pthread_create failed");
        };
    }

    for (int i = 0; i < thread_count; i++)
    {
        if(pthread_join(tinfo[i].thread_id, &res))
        {
            perror("Error: pthread_join failed");
            return EXIT_FAILURE;
        };
    }

    // char *lasts;
    // char *token = strtok_r(buffer, delim, &lasts);
    // for (int i = 0; i < 5; i++) {
    //     insert_array(words, token);
    //     printf("Token: '%s'\n  Array element: %d\n  word: '%s'\n  freq: %d\n\n", 
    //         token, i, words->arr[i].word, words->arr[i].freq);
    //     token = strtok_r(NULL, delim, &lasts);
    // }

    // printf("RESULTS TEST:\nUSED: %lu\nSIZE: %lu\n", words->used, words->size);
    // // printf("'%s': %d", words->arr[0]->word, words->arr[0]->freq);
    // for (int i = 0; i < 5; i++) {
    //     printf("%d   '%s': %d\n", i, words->arr[i].word, words->arr[i].freq);
    // }

    // ***TO DO *** Process TOP 10 and display
    // array *highest_counts = malloc(QTY_TOP_WORDS * sizeof(word_freq));
    // words->size;

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
    // free_tinfo(tinfo);
    free(buffer);
    buffer = NULL;
    free_tinfo(tinfo);
    pthread_mutex_destroy(&mutex);
}

void init_array(array *words, size_t size)
{
    words->arr = malloc(size * sizeof(word_freq));
    words->used = 0;
    words->size = size;
}

void insert_array(array *words, char *token)
{
    if ( words->used == words->size ) {
        words->size *= 2;
        words->arr = realloc(words->arr, words->size * sizeof(word_freq));
    }
    word_freq new_word;
    new_word.word = token;
    new_word.freq = 1;
    words->arr[words->used++] = new_word;
};

void free_array(array *words) 
{
    free(words->arr);
    words->arr = NULL;
    words->used = words->size = 0;
    free(words);
    words = NULL;
};

void free_tinfo(thread_info *tinfo)
{
    tinfo->text = NULL;
    free_array(tinfo->words);
    tinfo->chunk_size = tinfo->thread_num = 0;
    free(tinfo);
    tinfo = NULL;
}

word_freq* merge( array *words, int l, int m, int r ) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    word_freq left[n1], right[n2], ret_array[n1 + n2];

    for (i = 0; i < n1; i++)
        left[i] = words->arr[i];
    for (j = 0; j < n2; j++)
        right[j] = words->arr[m + 1 + j];

    i = j = 0;
    k = l;

    while( i < n1 && j < n2)
    {
        if (left[i].freq <= right[j].freq) {
            ret_array[k] = left[i];
            i++;
        } else {
            ret_array[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        ret_array[k] = left[i];
        i++;
        k++;
    }

    while (j < n2) {
        ret_array[k] = right[j];
        j++;
        k++;
    }
    
    return ret_array;
}

word_freq* merge_sort( array *words, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        
    }
}

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
                if (strcasecmp(tinfo->words->arr[i].word, token) == 0)
                {
                    exists = 1;
                    if(pthread_mutex_lock(&tinfo->mutex))
                    {
                        perror("Error: mutex lock failed.");
                    };
                    tinfo->words->arr[i].freq++;
                    if(pthread_mutex_unlock(&tinfo->mutex))
                    {
                        perror("Error: mutex failed to unlock");
                    };
                }
            }
            
            if (!exists) {
                if (pthread_mutex_lock(&tinfo->mutex))
                {
                    perror("Error: mutex lock failed.");
                };
                insert_array(tinfo->words, token);
                if(pthread_mutex_unlock(&tinfo->mutex))
                {
                    perror("Error: mutex failed to unlock");
                };
            }
        }
        token = strtok_r(NULL, delim, &lasts);
    }
    free(chunk);
    chunk = NULL;
};

int max( int a, int b) {
    return (a > b) ? a : b;
}