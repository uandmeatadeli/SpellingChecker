// dictionary.c
#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Assuming a fixed size for simplicity; consider dynamic allocation for scalability
#define MAX_WORDS 100000
char* dictionary[MAX_WORDS];
int dict_size = 0;

void load_dictionary(const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening dictionary file");
        exit(EXIT_FAILURE);
    }
    char word[256]; // Assuming words won't exceed 255 characters
    while (fgets(word, sizeof(word), file) && dict_size < MAX_WORDS) {
        word[strcspn(word, "\n")] = 0; // Remove newline character
        dictionary[dict_size++] = strdup(word); // Store a copy of the word
    }
    fclose(file);
    // You may want to sort the array if it's not already sorted
}

int check_word(const char* word) {
    // Implement binary search or another efficient search method here
    return 0; // Placeholder: return 1 if found, 0 otherwise
}