#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h> // Include the stdbool.h header




#define MAX_WORDS_IN_DICT 100000
#define MAX_WORD_LENGTH 100
#define BUFFER_SIZE 4096

char dictionary[MAX_WORDS_IN_DICT][MAX_WORD_LENGTH];
int dictionarySize = 0;

void loadDictionary(const char* dictionaryPath);
bool findWordInDictionary(const char* word);
void processFile(const char* filePath);
void traverseDir(const char* dirPath);
void toLowerCase(char* str);
bool isAllUpperCase(const char* str);
bool checkWord(const char* word);
void stripPunctuationAndCheckWord(char* word, const char* filePath, int line, int wordStartColumn);


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
    char word[MAX_WORD_LENGTH];
    int wordIndex = 0;
    int wordStartColumn = 1;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; ++i) {
            char c = buffer[i];
            if (isalnum(c) || c == '\'' || c == '-' || c == '(' || c == '[' || c == '{') {
                if (wordIndex == 0) wordStartColumn = column; // Mark the start of a new word
                word[wordIndex++] = c;
            } else {
                if (wordIndex > 0) {
                    word[wordIndex] = '\0'; // Null-terminate the current word
                    stripPunctuationAndCheckWord(word, filePath, line, wordStartColumn);
                    wordIndex = 0;
                }
                if (c == '\n') {
                    line++;
                    column = 0; // Reset column at the start of a new line
                }
            }
            column++;
        }
    }

    // Check for the last word in the buffer
    if (wordIndex > 0) {
        word[wordIndex] = '\0';
        stripPunctuationAndCheckWord(word, filePath, line, wordStartColumn);
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
        if (entry->d_name[0] == '.') continue; // Skip hidden files and directories

        char pathBuffer[1024];
        snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", dirPath, entry->d_name);

        if (entry->d_type == DT_DIR) {
            traverseDir(pathBuffer);
        } else if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt")) {
            processFile(pathBuffer);
        }
    }

    closedir(dir);
}

void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}


bool isAllUpperCase(const char* str) {
    for (int i = 0; str[i]; i++) {
        if (islower(str[i])) {
            return false;
        }
    }
    return true;
}

bool checkWord(const char* word) {
    if (findWordInDictionary(word)) {
        return true;
    }
    // If the word is all uppercase or starts with an uppercase letter, try checking its lowercase version.
    if (isupper(word[0]) || isAllUpperCase(word)) {
        char temp[MAX_WORD_LENGTH];
        strcpy(temp, word);
        toLowerCase(temp);
        return findWordInDictionary(temp);
    }
    return false;
}


void stripPunctuationAndCheckWord(char* word, const char* filePath, int line, int wordStartColumn) {
    // Strip punctuation from the start and end of the word
    char *start = word, *end = word + strlen(word) - 1;
    while (ispunct(*start)) start++;
    while (end > start && ispunct(*end)) end--;
    *(end + 1) = '\0';

    if (!checkWord(start)) {
        printf("%s (%d, %d): %s\n", filePath, line, wordStartColumn, start);
    }
}