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
    word_freq **arr;
    size_t used;
    size_t size;
} word_arr;

int init_array(word_arr *words, size_t size);

void insert_array(word_arr *words, word_freq *word);

void free_array(word_arr *words);

int main( int argc, char *argv[])
{
    word_arr *words = malloc(sizeof(word_arr));
    init_array(words, 10);
    // word_freq *word = malloc(sizeof(word_freq));
    // word->word = malloc(5);
    // *word->word = "Hello";
    // word->freq = 10;
    // printf("'%s': %d", *word->word, word->freq);
    // insert_array(words, word);
    // printf("'%s': %d", *words->arr[0]->word, words->arr[0]->freq);
    for (int i = 0; i < words->size; i++ ) {
        word_freq *word = malloc(sizeof(word_freq));
        word->word = malloc(5);
        word->word = "Hello";
        word->freq = i;
        insert_array(words, word);
    }
    for (int i = 0; i < words->size; i++) {
        printf("'%s': %d", words->arr[i]->word, words->arr[i]->freq);
    }
    return EXIT_SUCCESS;
}

int init_array(word_arr *words, size_t size)
{
    words->arr = malloc(sizeof(words->arr) * size);
    if((words->arr = malloc(sizeof(word_freq) * size)) == NULL)
    {
        perror("Error: init_array() failed -- malloc failed");
        return EXIT_FAILURE;
    }
    words->used = 0;
    words->size = size;
    return 0;
}

void insert_array(word_arr *words, word_freq *word)
{
    if ( words->used == words->size ) {
        words->size *= 2;
        words->arr = realloc(words->arr, words->size * sizeof(word_freq));
    }
    words->arr[words->used++] = word;
};

void free_array(word_arr *words) 
{
    for (int i = 0; i < words->used; i++ ) {
        free(words->arr[i]->word);
        words->arr[i]->word = NULL;
    }
    free(words->arr);
    words->arr = NULL;
    words->used = words->size = 0;
};