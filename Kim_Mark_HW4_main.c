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
* This program will read a text file and form a vocabulary with a count of 
* each word found within the text.  This process can occur with multi-threading
* with the number of threads determined by the second argument given.  Once
* complete, the program will print out the top 10 words with a length of 6 or
* greater.  Each run of the program will give a total run time which can be used 
* to evaluate the speed of each run with respect to the number of threads being
* used.
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

#define INITIAL_ARRAY_SIZE  50000   // array size to work with
#define QTY_TOP_WORDS       10      // number of words to list
#define STRING_LENGTH       6       // inclusive string length

// You may find this Useful
char * delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";

// Data structure for words with frequency count
typedef struct 
{
    char *word;
    int freq;
} word_freq;

// Data structure for dynamic array
typedef struct
{
    word_freq *arr;
    size_t used;
    size_t size;
} array;

// Data structure of thread info to be passed into pthread_create
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
void *process_chunk( void *arg );
void print_results(array *words, char *filename, 
    int num_threads, int num_words, int num_chars);
void selection_sort(array *ptr);
int partition(array *ptr, int low, int high);
void quick_sort(array *ptr, int low, int high);
void swap(word_freq *a, word_freq *b);

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
    int file = open(argv[1], O_RDONLY);
    if (file == -1)
    {
        perror("ERROR: file could not be opened.");
        return EXIT_FAILURE;
    };

    // Seek to end of file to find file length
    int file_length = lseek(file, 0, SEEK_END);
    if (file_length == -1)
    {
        perror("ERROR: lseek to end failed.");
        return EXIT_FAILURE;
    }

    // Reset pointer to beginning of file
    if (lseek(file, 0, SEEK_SET) == -1)
    {
        perror("ERROR: lseek to beginning failed.");
        return EXIT_FAILURE;
    }

    // Initialize the word count array
    array *words = malloc(sizeof(array));
    init_array(words, INITIAL_ARRAY_SIZE);

    // Reserve sufficient memory to store the entire text file
    char *buffer = malloc(file_length);
    if (!buffer)
    {
        perror("Error: malloc failed.");
        return EXIT_FAILURE;
    };

    // Read entire file into buffer
    int read_result = read(file, buffer, file_length);
    if (read_result == -1)
    {
        perror("Error: read file into buffer failed.");
        return EXIT_FAILURE;
    }

    // Close file
    if (close(file) == -1)
    {
        perror("Error: file close failed.");
        return EXIT_FAILURE;
    }

    // Initialize mutex
    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, NULL) != 0) 
    {
        perror("Error: mutex init failed.");
        return EXIT_FAILURE;
    }

    // Declare and allocate memory for tinfo array to pass into each thread
    thread_info *tinfo = (thread_info*)calloc(thread_count, sizeof(*tinfo));
    if (tinfo == NULL)
    {
        perror("Error: calloc failed");
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

    // Loop to initialize thread info for each thread and create threads to
    // process chunks of data
    for (int i = 0; i < thread_count; i++)
    {
        tinfo[i].text = buffer;     // pointer to full text
        tinfo[i].words = words;     // pointer to array of word counts
        // Split file into parts equalling thread count
        tinfo[i].chunk_size = i == thread_count - 1
            ? file_length - ((thread_count - 1) * (int)(file_length / thread_count)) 
            : file_length / thread_count;
        tinfo[i].thread_num = i;    // number of thread
        tinfo[i].mutex = mutex;     // pass in mutex for thread locks
        // Create threads
        if(pthread_create(&tinfo[i].thread_id, NULL, process_chunk, &tinfo[i]))
        {
            perror("Error: pthread_create failed");
        };
    }

    // Loop to join threads
    for (int i = 0; i < thread_count; i++)
    {
        if(pthread_join(tinfo[i].thread_id, &res))
        {
            perror("Error: pthread_join failed");
            return EXIT_FAILURE;
        };
    }

    // Initialize the top words count array
    // array *top_words = malloc(sizeof(array));
    // init_array(top_words, QTY_TOP_WORDS);
    // selection_sort(words);
    quick_sort(words, 0, words->used);

    // print_array(words);

    // for (int i = 0; i < QTY_TOP_WORDS; i++)
    // {
    //     top_words->arr[top_words->used++] = words->arr[i];
    // }
    print_results(words, argv[1], thread_count, QTY_TOP_WORDS, STRING_LENGTH);

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
    free_array(words);
    free(tinfo);
    // free(top_words->arr);
    // free(top_words);
    pthread_mutex_destroy(&mutex);
}

// Initialize dynamic array
void init_array(array *words, size_t size)
{
    words->arr = calloc(size, sizeof(word_freq));
    words->used = 0;
    words->size = size;
}

// Insert new word into dynamic array
// NOT thread safe: requires mutex lock.  Race conditions exist.
void insert_array(array *words, char *token)
{
    // Check if current array is full and reallocate memory if necessary
    if ( words->used == words->size ) 
    {
        words->size += words->size;
        words->arr = realloc(words->arr, words->size * sizeof(word_freq));
    }
    // Initialize new word_freq count struct to be inserted into array
    // Mutex lock not necessary here
    word_freq new_word;
    new_word.freq = 1;
    // Must copy token into word, otherwise the pointer will point to whatever
    // token is pointing at
    new_word.word = malloc(strlen(token)+1);
    strcpy(new_word.word, token);
    // Mutex lock necessary here
    words->arr[words->used++] = new_word;
}

// Selection Sort for array
void selection_sort(array *ptr)
{
    for (int i = 0; i < ptr->used; i++)
    {
        for (int j = i + 1; j < ptr->used; j++)
        {
            if (ptr->arr[i].freq < ptr->arr[j].freq)
            {
                swap(&ptr->arr[i], &ptr->arr[j]);
            }
        }
    }
}

// Partition array for quick_sort
int partition(array *ptr, int low, int high)
{
    word_freq pivot = ptr->arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (ptr->arr[j].freq > pivot.freq)
        {
            i++;
            swap(&ptr->arr[i], &ptr->arr[j]);
        }
    }
    swap(&ptr->arr[i+1], &ptr->arr[high]);
    return (i + 1);
}

// Quick Sort for array (recursive)
void quick_sort(array *ptr, int low, int high)
{
    if (low < high)
    {
        int pi = partition(ptr, low, high);
        quick_sort(ptr, low, pi - 1);
        quick_sort(ptr, pi + 1, high);
    }
}

// Swap for both quick sort and selection sort
void swap(word_freq *a, word_freq *b)
{
    word_freq temp = *a;
    *a = *b;
    *b = temp;
}

// Free entire array
void free_array(array *words) 
{
    for (int i = 0; i < words->used; i++) 
    {
        free(words->arr[i].word);
        words->arr[i].word = NULL;
    }
    free(words->arr);
    words->arr = NULL;
    words->used = words->size = 0;
    free(words);
    words = NULL;
}

// Print results
void print_results(array *words, char *filename, 
    int num_threads, int num_words, int num_chars)
{
    printf("Word Frequency Count on %s with %d threads.\n", filename, num_threads);
    printf("Printing top %d words %d characters or more.\n", num_words, num_chars);
    for (int i = 0; i < num_words; i++)
    {
        printf("Number %d is '%s' with a count of %d\n", 
            i + 1, words->arr[i].word, words->arr[i].freq);
    }
}

// Process chunks function to pass into pthread_create
void *process_chunk( void *arg )
{
    // Thread info arguments passed in for use by each thread
    thread_info *tinfo = arg;

    // Chunk data into pieces for each thread
    char *chunk = calloc(tinfo->chunk_size + 1, sizeof(char));
    char *chunk_start = tinfo->text + (tinfo->chunk_size * tinfo->thread_num);
    strncpy(chunk, chunk_start, tinfo->chunk_size);

    // Tokenization and processing of chunk
    char *lasts = chunk;
    char *token = strtok_r(chunk, delim, &lasts);
    while (token != NULL) 
    {
        // Check if token length satisfied string length requirement
        if (strlen(token) >= STRING_LENGTH) 
        {
            // Check if token exists in word array; initialize to false
            char exists = 0;
            for (int i = 0; i < tinfo->words->used; i++)
            {
                // Increment word count if token matches a word in the array
                if (tinfo->words->arr[i].word != NULL && 
                    strcasecmp(tinfo->words->arr[i].word, token) == 0)
                {
                    exists = 1;

                    // Mutex lock necessary for incrementing word count
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
            
            // Insert new word into array if it does not already exist
            if (!exists) 
            {
                // Mutex necessary for inserting new word into array
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

    // Cleanup
    free(chunk);
    chunk = NULL;
}