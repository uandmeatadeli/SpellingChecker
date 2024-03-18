#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h> 

#define MAX_WORDS_IN_DICT 100000
#define MAX_WORD_LENGTH 100
#define BUFFER_SIZE 4096

char dictionary[MAX_WORDS_IN_DICT][MAX_WORD_LENGTH];
int dictionarySize = 0;

// Function Prototypes
void loadDictionary(const char* dictionaryPath);
bool findWordInDictionary(const char* word);
void processFile(const char* filePath);
void traverseDir(const char* dirPath);
void toLowerCase(char* str);
void toUpperCase(char* str);
bool isAllUpperCase(const char* str);
bool checkWord(const char* word);
bool isPunctuation(char c);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <dictionary_path> <file_or_directory_path>...\n", argv[0]);
        return EXIT_FAILURE;
    }

    loadDictionary(argv[1]);

    for (int i = 2; i < argc; i++) {
        struct stat pathStat;
        stat(argv[i], &pathStat);
        if (S_ISDIR(pathStat.st_mode)) {
            traverseDir(argv[i]);
        } else {
            processFile(argv[i]);
        }
    }

    return EXIT_SUCCESS;
}

void loadDictionary(const char* dictionaryPath) {
    FILE* file = fopen(dictionaryPath, "r");
    if (!file) {
        perror("Failed to open dictionary file");
        exit(EXIT_FAILURE);
    }

    while (fgets(dictionary[dictionarySize], MAX_WORD_LENGTH, file)) {
        dictionary[dictionarySize][strcspn(dictionary[dictionarySize], "\n")] = '\0';
        dictionarySize++;
        if (dictionarySize >= MAX_WORDS_IN_DICT) break;
    }

    fclose(file);
}

bool findWordInDictionary(const char* word) {
    for (int i = 0; i < dictionarySize; i++) {
        if (strcmp(dictionary[i], word) == 0) {
            return true;
        }
    }
    return false;
}

void processFile(const char* filePath) {
    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    int line = 1, column = 1;
    bool isWord = false;
    char word[MAX_WORD_LENGTH];
    int wordIndex = 0;
    int wordStartColumn = 1;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; ++i) {
            char c = buffer[i];
            if (isalnum(c) || (isWord && c == '-' && i + 1 < bytesRead && isalnum(buffer[i + 1]))) {
                if (!isWord) wordStartColumn = column;
                word[wordIndex++] = c;
                isWord = true;
            } else {
                if (isWord) {
                    word[wordIndex] = '\0';
                    while (wordIndex > 0 && isPunctuation(word[wordIndex - 1])) {
                        word[--wordIndex] = '\0';
                    }
                    if (!checkWord(word)) {
                        printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
                    }
                    wordIndex = 0;
                    isWord = false;
                }
                if (c == '\n') {
                    line++;
                    column = 0;
                }
            }
            column++;
        }
    }

    if (isWord) {
        word[wordIndex] = '\0';
        if (!checkWord(word)) {
            printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
        }
    }

    close(fd);
}

void traverseDir(const char* dirPath) {
    DIR* dir = opendir(dirPath);
    if (dir == NULL) {
        perror("Failed to open directory");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        char pathBuffer[1024];
        snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", dirPath, entry->d_name);

        struct stat pathStat;
        if (stat(pathBuffer, &pathStat) != 0) continue;

        if (S_ISDIR(pathStat.st_mode)) {
            traverseDir(pathBuffer);
        } else if (S_ISREG(pathStat.st_mode) && strstr(entry->d_name, ".txt")) {
            processFile(pathBuffer);
        }
    }

    closedir(dir);
}

void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

void toUpperCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

bool isAllUpperCase(const char* str) {
    for (int i = 0; str[i]; i++) {
        if (islower((unsigned char)str[i])) {
            return false;
        }
    }
    return true;
}

/*
bool checkWord(const char* word) {
   char lowerCase[MAX_WORD_LENGTH];
   char upperCase[MAX_WORD_LENGTH];
   char firstLetterCap[MAX_WORD_LENGTH];


   // Convert to lowercase for comparison
   strcpy(lowerCase, word);
   toLowerCase(lowerCase);


   // Convert to uppercase for comparison
   strcpy(upperCase, word);
   toUpperCase(upperCase);


   // First letter capitalized version
   strcpy(firstLetterCap, word);
   firstLetterCap[0] = toupper(firstLetterCap[0]);
   for (int i = 1; firstLetterCap[i]; i++) {
       firstLetterCap[i] = tolower(firstLetterCap[i]);
   }


   // Direct match (exact as in dictionary, includes proper nouns or acronyms)
   if (findWordInDictionary(word)) {
       return true;
   }


   // If lowercase version exists in dictionary, accept "Hello" and "HELLO" forms but not mixed case like "HeLlO"
   if (findWordInDictionary(lowerCase)) {
       // If original word is exactly lowercase, it's incorrect since dictionary has lowercase version
       if (strcmp(word, lowerCase) == 0) {
           return false;
       }


       // Accept if word is in uppercase or has the first letter capitalized (proper noun or start of sentence)
       if (strcmp(word, upperCase) == 0 || strcmp(word, firstLetterCap) == 0) {
           return true;
       }
   }

   


   // The word did not match any allowed variations
   return false;
}


bool checkWord(const char* word) {
    char lowerCase[MAX_WORD_LENGTH];
    char upperCase[MAX_WORD_LENGTH];
    char firstLetterCap[MAX_WORD_LENGTH];

    // Convert to lowercase for comparison
    strcpy(lowerCase, word);
    toLowerCase(lowerCase);

    // Convert to uppercase for comparison
    strcpy(upperCase, word);
    toUpperCase(upperCase);

    // First letter capitalized version
    strcpy(firstLetterCap, word);
    firstLetterCap[0] = toupper(firstLetterCap[0]);
    for (int i = 1; firstLetterCap[i]; i++) {
        firstLetterCap[i] = tolower(firstLetterCap[i]);
    }

    // Check for direct match (exact as in dictionary, includes proper nouns or acronyms)
    if (findWordInDictionary(word)) {
        return true;
    }

    // If lowercase version exists in dictionary, it's a match for the assignment rules
    if (findWordInDictionary(lowerCase)) {
        // Accept if word is in uppercase or has the first letter capitalized (proper noun or start of sentence)
        if (strcmp(word, upperCase) == 0 || strcmp(word, firstLetterCap) == 0) {
            return true;
        }
        // Reject if it's exactly lowercase or doesn't match the dictionary case (like "HeLlO")
        return false;
    }

    // Since none of the conditions matched, the word is not in the dictionary
    return false;
}
*/

bool checkWord(const char* word) {
    char temp[MAX_WORD_LENGTH] = {0};
    char firstCap[MAX_WORD_LENGTH] = {0};

    // Direct dictionary match (case-sensitive).
    if (findWordInDictionary(word)) {
        return true;
    }

    // Create lowercase and first-letter capitalized versions of the word.
    strcpy(temp, word);
    toLowerCase(temp); // Convert word to lowercase.

    // Check if lowercase version exists in the dictionary.
    bool lowercaseFound = findWordInDictionary(temp);

    // If lowercase version found, validate against allowed capitalization forms.
    if (lowercaseFound) {
        // Check if word is all uppercase and matches the lowercase dictionary entry.
        strcpy(firstCap, temp);
        firstCap[0] = toupper(firstCap[0]); // First-letter capitalized version.
        toUpperCase(temp); // Convert to uppercase for comparison.
        if (strcmp(word, temp) == 0 || strcmp(word, firstCap) == 0) {
            return true; // Accepts "HELLO" and "Hello" for "hello".
        }
    } else {
        // Prepare for special capitalization case handling.
        strcpy(temp, word);
        toUpperCase(temp); // Convert the input word to uppercase for comparison.

        // Handle case where dictionary might contain special capitalization, e.g., "MacDonald".
        strcpy(firstCap, word);
        firstCap[0] = toupper(firstCap[0]);
        for (int i = 1; word[i] != '\0'; i++) {
            firstCap[i] = tolower(firstCap[i]);
        }

        // Check if "MACDONALD" for "MacDonald"
        if (strcmp(word, temp) == 0) { // Word is uppercase
            toLowerCase(temp); // Lowercase for dictionary check
            if (findWordInDictionary(temp)) {
                return false; // Reject "macdonald" if "MacDonald" is the correct form.
            }
            // Correctly handle "MACDONALD" if "MacDonald" is in the dictionary.
            if (findWordInDictionary(firstCap)) {
                return true;
            }
        }
        
        // Check the firstCap against the dictionary for a direct match.
        if (findWordInDictionary(firstCap)) {
            return strcmp(word, firstCap) == 0; // Accepts "MacDonald", rejects "macdonald", "Macdonald".
        }
    }

    // If none of the conditions are met, reject the word.
    return false;
}



bool isPunctuation(char c) {
    return strchr(",.;:?!\")([]{}'`-", c) != NULL;
}
