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

bool checkWord(const char* word) {
    char temp[MAX_WORD_LENGTH];
    strcpy(temp, word);
    toLowerCase(temp);

    if (findWordInDictionary(word) || findWordInDictionary(temp)) {
        return true;
    }

    if (isupper(word[0])) {
        temp[0] = toupper(temp[0]);
        if (findWordInDictionary(temp)) return true;

        toUpperCase(temp);
        if (findWordInDictionary(temp)) return true;
    }

    return false;
}

bool isPunctuation(char c) {
    return strchr(",.;:?!\")([]{}'`-", c) != NULL;
}
