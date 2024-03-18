#include "spchk.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h> 

// #define MAX_WORDS_IN_DICT 1000000
// #define MAX_WORD_LENGTH 100
// #define BUFFER_SIZE 4096

char dictionary[MAX_WORDS_IN_DICT][MAX_WORD_LENGTH];
int dictionarySize = 0;

// // Function Prototypes
// void loadDictionary(const char* dictionaryPath);
// bool findWordInDictionary(const char* word);
// void processFile(const char* filePath);
// void traverseDir(const char* dirPath);
// //void toLowerCase(char* str);
// void toLowerCase(const char* input, char* output);
// void toUpperCase(char* str);
// bool isAllUpperCase(const char* str);
// bool checkWord(const char* word);
// bool isPunctuation(char c);
// void testStripPunctuation(const char* word, const char* expectedResult);
// void isCapitalizationAcceptable(const char* word, const char* lowercaseVersion);
// bool findLowerCaseInDictionary(const char* word);
// bool checkSpecialUpperCase(const char* word);
// void stripPunctuation(char* word);
// bool checkHyphenatedWord(const char* word);

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

    testStripPunctuation("hello,", "hello");
    testStripPunctuation("world!", "world");
    testStripPunctuation("\"quotation", "quotation");
    testStripPunctuation("(brackets", "brackets");
    testStripPunctuation("{curly", "curly");
    testStripPunctuation("'apostrophe", "apostrophe");
    testStripPunctuation("test.", "test");
    testStripPunctuation(".another", "another");
    testStripPunctuation("'('test'", "test");
    testStripPunctuation("[[word]]", "word");
    testStripPunctuation("''quote", "quote");

    return EXIT_SUCCESS;
}



void loadDictionary(const char* dictionaryPath) {
    FILE* file = fopen(dictionaryPath, "r");
    if (!file) {
        perror("Failed to open dictionary file");
        exit(EXIT_FAILURE);
    }

    while (fgets(dictionary[dictionarySize], MAX_WORD_LENGTH, file) != NULL && dictionarySize < MAX_WORDS_IN_DICT) {
        // Remove the newline character at the end of each word
        dictionary[dictionarySize][strcspn(dictionary[dictionarySize], "\n")] = '\0';
        dictionarySize++;
    }

    if (dictionarySize >= MAX_WORDS_IN_DICT) {
        fprintf(stderr, "Warning: Dictionary size limit reached. Some words may not have been loaded.\n");
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
/*
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
                    stripPunctuation(word);
                    while (wordIndex > 0 && isPunctuation(word[wordIndex - 1])) {
                        word[--wordIndex] = '\0';
                    }

                    // Inside processFile, right before checkWord call
if (strchr(word, '-')) {
    if (!checkHyphenatedWord(word)) {
        printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
    }
} else {
    if (!checkWord(word)) {
        printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
    }
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
                    stripPunctuation(word); // Strip punctuation at the start and end of the word

                    // Determine if the word is hyphenated and check accordingly
                    bool wordIsCorrect = false; // Flag to track if the word is spelled correctly
                    if (strchr(word, '-')) { // If the word contains a hyphen, indicating it's hyphenated
                        wordIsCorrect = checkHyphenatedWord(word);
                    } else { // For non-hyphenated words
                        wordIsCorrect = checkWord(word);
                    }

                    // Print the word if it is incorrect
                    if (!wordIsCorrect) {
                        printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
                    }

                    // Reset for the next word
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
        // This block is redundant and should be removed because the same check is performed in the loop above.
        // Remove the following lines to avoid duplicate code and potential errors.
        // if (!checkWord(word)) {
        //     printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
        // }
    }

    close(fd);
}

*/
// Updated processFile function
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
    char word[MAX_WORD_LENGTH] = {0};
    int wordIndex = 0;
    int wordStartColumn = 1;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; ++i) {
            char c = buffer[i];
            // Check for word characters and hyphens (for hyphenated words)
            if (isalnum(c) || (isWord && c == '-' && i + 1 < bytesRead && isalnum(buffer[i + 1]))) {
                if (!isWord) {
                    isWord = true;
                    wordStartColumn = column; // Mark the start of a new word
                }
                word[wordIndex++] = c;
            } else {
                if (isWord) {
                    word[wordIndex] = '\0'; // Terminate the current word
                    stripPunctuation(word); // Strip punctuation at the beginning and end
                    
                    //if (!checkWord(word)) {
                       // printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
                 //   }
                  // Begin hyphenated word checking logic
        if (strchr(word, '-')) { // If the word contains a hyphen
            if (!checkHyphenatedWord(word)) {
                printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
            }
        } else {
            if (!checkWord(word)) { // Check non-hyphenated word
                printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
            }
        }

                    // Reset for next word
                    wordIndex = 0;
                    isWord = false;
                }

                if (c == '\n') {
                    line++;
                    column = 0; // Reset column at new line
                }
            }
            column++;
        }
    }

    // Handle the last word in the file if it doesn't end with a newline
    if (isWord) {
        word[wordIndex] = '\0'; // Ensure the last word is null-terminated
        stripPunctuation(word); // Strip punctuation at the beginning and end of the word

        // Now, handle the possibly hyphenated or regular last word with punctuation removed
        if (strchr(word, '-')) {
            if (!checkHyphenatedWord(word)) {
                printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
            }
        } else {
            if (!checkWord(word)) {
                printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, word);
            }
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
    char pathBuffer[1024];
    struct stat pathStat;

    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Skip any files or directories that begin with "."
        if (entry->d_name[0] == '.') {
            continue;
        }

        snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", dirPath, entry->d_name);

        if (stat(pathBuffer, &pathStat) != 0) {
            // Optionally handle stat error
            continue;
        }

        if (S_ISDIR(pathStat.st_mode)) {
            // It's a directory, recurse into it
            traverseDir(pathBuffer);
        } else if (S_ISREG(pathStat.st_mode)) {
            // It's a regular file, check if it ends with ".txt"
            char* ext = strrchr(entry->d_name, '.');
            if (ext != NULL && strcmp(ext, ".txt") == 0) {
                processFile(pathBuffer);
            }
        }
    }

    closedir(dir);
}
/*
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
*/

void toLowerCase(const char* input, char* output) {
    for (int i = 0; input[i]; i++) {
        output[i] = tolower((unsigned char)input[i]);
    }
    output[strlen(input)] = '\0'; // Null-terminate the output string.
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


bool checkWord(const char* word) {
    char temp[MAX_WORD_LENGTH] = {0};
    char firstCap[MAX_WORD_LENGTH] = {0};
    strcpy(temp, word);
    toLowerCase(temp); // Convert word to lowercase for comparison.

    // Direct match (case-sensitive).
    if (findWordInDictionary(word)) {
        return true;
    }

    // Check if lowercase version exists in the dictionary.
    bool lowercaseFound = findWordInDictionary(temp);
    if (!lowercaseFound) {
        return false; // If the lowercase version isn't found, the word is incorrect.
    }

    // At this point, the lowercase version of the word exists in the dictionary.
    // Now, check for allowed capitalization forms:
    // 1. All uppercase (e.g., "APPLE") is acceptable if the lowercase version is in the dictionary.
    // 2. First letter capitalized (e.g., "Apple") is acceptable if the lowercase version is in the dictionary.
    // 3. All lowercase (e.g., "apple") is directly accepted above with the lowercaseFound check.

    // Generate the first-letter capitalized version of temp
    temp[0] = toupper(temp[0]);
    strcpy(firstCap, temp);

    // The original word is acceptable if it matches the all-uppercase or first-letter capitalized version of temp
    toUpperCase(temp); // Convert temp to all uppercase for comparison
    if (strcmp(word, temp) == 0 || strcmp(word, firstCap) == 0) {
        return true; // The word matches one of the accepted capitalization forms.
    }

    // If none of the conditions are met, reject the word.
    return false;
}
*/
bool findLowerCaseInDictionary(const char* word) {
    char lowercaseWord[MAX_WORD_LENGTH];
    toLowerCase(word, lowercaseWord);
    return findWordInDictionary(lowercaseWord);
}

bool checkSpecialUpperCase(const char* word) {
    // This function checks for special uppercase words like "MACDONALD" when "MacDonald" is in the dictionary.
    char temp[MAX_WORD_LENGTH];
    bool found = false;
    for (int i = 0; i < dictionarySize && !found; ++i) {
        strcpy(temp, dictionary[i]);
        toUpperCase(temp); // Convert the dictionary word to uppercase for comparison.
        if (strcmp(word, temp) == 0) {
            // If the uppercase version of the dictionary word matches the input word,
            // it means the input word is a valid uppercase version of a specially-cased word.
            return true;
        }
    }
    return false; // No special uppercase version found in the dictionary.
}

bool checkWord(const char* word) {
    char temp[MAX_WORD_LENGTH] = {0};
    //char firstCap[MAX_WORD_LENGTH] = {0};
    bool isExactMatch = findWordInDictionary(word);

    // Handle exact match (case-sensitive).
    if (isExactMatch) {
        return true;
    }

    // Handle fully uppercase word.
    toLowerCase(word, temp); // Convert to lowercase for dictionary check.
    bool isLowercaseMatch = findWordInDictionary(temp);
    if (isAllUpperCase(word) && isLowercaseMatch) {
        return true;
    }

    // Insert the refined logic for handling first-letter capitalized words.
    if (isLowercaseMatch) {
        char firstCap[MAX_WORD_LENGTH] = {0};
        strcpy(firstCap, temp);
        firstCap[0] = toupper(firstCap[0]); // Ensure first letter is uppercase.
        for (int i = 1; word[i] != '\0'; i++) {
            firstCap[i] = tolower(word[i]); // The rest are lowercase to match the input.
        }

        if (strcmp(word, firstCap) == 0) {
            return true; // Accepts correctly capitalized words like "Hello".
        }
    }


    // Handle special case-sensitive words.
    if (isAllUpperCase(word)) {
        // Check if a special uppercase version exists for case-sensitive words.
        return checkSpecialUpperCase(word);
    }

    // Handle first-letter-capitalized words.
    // Convert first letter to uppercase and the rest to lowercase to match "MacDonald" like patterns.
    temp[0] = toupper(temp[0]);
    for (int i = 1; temp[i] != '\0'; i++) {
        temp[i] = tolower(temp[i]);
    }
    bool isFirstLetterCapMatch = strcmp(word, temp) == 0 && findWordInDictionary(temp);
    if (isFirstLetterCapMatch) {
        return true;
    }
    

    // If none of the conditions are met, the word is considered incorrect.
    return false;
}

/*
bool checkWord(const char* word) {
    char lowercaseVersion[MAX_WORD_LENGTH] = {0};
    toLowerCase(word, lowercaseVersion);

    // Exact match check (covers case-sensitive matches directly).
    if (findWordInDictionary(word)) {
        return true;
    }

    // Special handling for all-uppercase words.
    if (isAllUpperCase(word)) {
        for (int i = 0; i < dictionarySize; ++i) {
            char dictWordLower[MAX_WORD_LENGTH] = {0};
            toLowerCase(dictionary[i], dictWordLower);
            if (strcmp(dictWordLower, lowercaseVersion) == 0) {
                // Convert first letter to uppercase, the rest stays lowercase.
                dictWordLower[0] = toupper(dictWordLower[0]);
                if (strcmp(dictWordLower, dictionary[i]) == 0) {
                    // If the dictionary word has special capitalization, accept the uppercase input.
                    return true;
                }
                break;
            }
        }
    }

    // Accept "hello", "Hello", and "HELLO" variations if "hello" is in the dictionary.
    if (findLowerCaseInDictionary(word)) {
        char firstLetterCap[MAX_WORD_LENGTH] = {0};
        strcpy(firstLetterCap, lowercaseVersion);
        firstLetterCap[0] = toupper(firstLetterCap[0]);
        if (strcmp(word, lowercaseVersion) == 0 || strcmp(word, firstLetterCap) == 0 || isAllUpperCase(word)) {
            return true;
        }
    }

    // Reject any other form.
    return false;
}

*/

bool isPunctuation(char c) {
    return strchr(",.;:?!\")([]{}'`-,", c) != NULL;
}



void stripPunctuation(char* word) {
    size_t length = strlen(word);
    
    // Ignore trailing punctuation
    while (length > 0 && isPunctuation(word[length - 1])) {
        word[length - 1] = '\0';
        length--;
    }
    
    // Ignore quotation marks and brackets at the start of a word
    size_t start = 0;
    while (start < length && (word[start] == '\'' || word[start] == '\"' || word[start] == '(' || word[start] == '[' || word[start] == '{')) {
        start++;
    }
    
    if (start > 0) {
        memmove(word, word + start, length - start + 1); // Include the null terminator
    }

    
}

void testStripPunctuation(const char* word, const char* expectedResult) {
    char testWord[100];
    strcpy(testWord, word);
    stripPunctuation(testWord);
    
    if (strcmp(testWord, expectedResult) == 0) {
        //printf("Test passed: %s\n", word);
    } else {
        //printf("Test failed: %s (Expected: %s, Actual: %s)\n", word, expectedResult, testWord);
    }
}

/*
bool checkHyphenatedWord(const char* word) {
    // New function to check each component of a hyphenated word
    char copy[MAX_WORD_LENGTH];
    strcpy(copy, word);
    char *token = strtok(copy, "-");
    while (token) {
        if (!checkWord(token)) {
            return false;
        }
        token = strtok(NULL, "-");
    }
    return true;
}
*/
bool checkHyphenatedWord(const char* word) {
    char temp[MAX_WORD_LENGTH];
    strcpy(temp, word); // Make a copy of the word to work with
    
    char* token = strtok(temp, "-"); // Split the word at hyphens
    while (token != NULL) {
        char tempWord[MAX_WORD_LENGTH] = {0};
        strcpy(tempWord, token); // Copy the token to tempWord for processing
        
        // Check each component of the hyphenated word
        if (!findWordInDictionary(tempWord)) {
            return false; // If any component is not in the dictionary, the whole word is incorrect
        }
        token = strtok(NULL, "-"); // Move to the next component
    }
    return true; // All components were found in the dictionary
}