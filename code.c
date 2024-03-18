#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h> // For stat()
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#define TABLE_SIZE 127
#define MAX_WORD_LENGTH 256
#define BUFFER_SIZE 4096

// Function to hash words for the hashmap
#define hash(word) ((unsigned int)(word)[0] % TABLE_SIZE)

// Node structure for the linked list in the hashmap
typedef struct Node {
    char* word;
    struct Node* next;
} Node;

// Hashmap as an array of pointers to Node, serving as the dictionary
Node* hashmap[TABLE_SIZE] = {NULL};

// Function Prototypes
void loadDictionary(const char* dictionaryPath);
void insertWord(char* word);
Node* createNode(char* word);
Node* findWordInDictionary(const char* word);
void processFile(const char* filePath);
void traverseFileSystem(const char* basePath);
void reportError(const char* word, const char* filePath, int lineNum, int colNum);
void checkSpelling(const char* word, const char* filePath, int lineNum, int colNum);
bool isValidChar(char c);
void checkAndCleanWord(char* word, const char* filePath, int lineNum, int colNum);
void extractAndCheckWords(const char* filePath);

// Implementations
Node* createNode(char* word) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->word = strdup(word); // Make a copy of the word
    newNode->next = NULL;
    return newNode;
}

void insertWord(char* word) {
    Node* newNode = createNode(word);
    unsigned int index = hash(word);
    if (!hashmap[index] || strcmp(word, hashmap[index]->word) < 0) {
        newNode->next = hashmap[index];
        hashmap[index] = newNode;
    } else {
        Node* current = hashmap[index];
        while (current->next && strcmp(word, current->next->word) > 0) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void loadDictionary(const char* dictionaryPath) {
    FILE* file = fopen(dictionaryPath, "r");
    if (!file) {
        perror("Error opening the dictionary file");
        exit(EXIT_FAILURE);
    }
    char word[MAX_WORD_LENGTH];
    while (fgets(word, MAX_WORD_LENGTH, file)) {
        word[strcspn(word, "\n")] = '\0'; // Remove newline character
        insertWord(word);
    }
    fclose(file);
}

Node* findWordInDictionary(const char* word) {
    unsigned int index = hash(word);
    for (Node* current = hashmap[index]; current != NULL; current = current->next) {
        if (strcmp(current->word, word) == 0) {
            return current; // Word found
        }
    }
    return NULL; // Word not found
}

void checkSpelling(const char* word, const char* filePath, int lineNum, int colNum) {
    char temp[MAX_WORD_LENGTH];
    // Check the word in its original form (handles case-sensitive exact matches)
    if (findWordInDictionary(word)) {
        return; // Correctly spelled
    }

    // Handle initial capital or all caps
    bool isAllCaps = true;
    for (int i = 1; word[i]; ++i) {
        if (islower(word[i])) {
            isAllCaps = false;
            break;
        }
    }

    if (isupper(word[0]) && !isAllCaps) {
        // Convert to lowercase for initial capital case
        strcpy(temp, word);
        temp[0] = tolower(temp[0]);
        if (findWordInDictionary(temp)) {
            return; // Correctly spelled
        }
    }

    if (isAllCaps) {
        // Convert all to lowercase for all caps case
        for (int i = 0; word[i]; ++i) {
            temp[i] = tolower(word[i]);
        }
        temp[strlen(word)] = '\0';
        if (findWordInDictionary(temp)) {
            return; // Correctly spelled
        }
    }

    // If none of the above, report the error
    reportError(word, filePath, lineNum, colNum);
}


void reportError(const char* word, const char* filePath, int lineNum, int colNum) {
    printf("%s (%d, %d): %s\n", filePath, lineNum, colNum, word);
}

bool isValidChar(char c) {
    return isalnum(c) || c == '\'' || c == '-'; // Consider alphanumeric characters, apostrophes, and hyphens
}

void checkAndCleanWord(char* word, const char* filePath, int lineNum, int startCol) {
    char cleanWord[MAX_WORD_LENGTH] = {0};
    int j = 0;
    // Remove leading and trailing punctuation
    for (int i = 0; word[i] != '\0'; ++i) {
        if (isValidChar(word[i])) {
            cleanWord[j++] = tolower(word[i]); // Convert to lowercase
        }
    }
    cleanWord[j] = '\0';
    if (j > 0) { // Check non-empty words
        checkSpelling(cleanWord, filePath, lineNum, startCol);
    }
}

void extractAndCheckWords(const char* filePath) {
    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        perror("Error opening the file");
        return;
    }
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    char word[MAX_WORD_LENGTH];
    int wordIndex = 0, lineNum = 1, colNum = 1, wordStartCol = 1;
    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        for (int i = 0; i < bytesRead; ++i) {
            if (isValidChar(buffer[i])) {
                if (wordIndex == 0) wordStartCol = colNum; // Mark start of the word
                word[wordIndex++] = buffer[i];
            } else {
                if (wordIndex > 0) {
                    word[wordIndex] = '\0'; // Null-terminate the word
                    checkAndCleanWord(word, filePath, lineNum, wordStartCol);
                    wordIndex = 0; // Reset for the next word
                }
                if (buffer[i] == '\n') {
                    lineNum++;
                    colNum = 0; // Reset column number at new line
                }
            }
            colNum++;
        }
    }
    if (wordIndex > 0) { // Check last word in the buffer
        word[wordIndex] = '\0';
        checkAndCleanWord(word, filePath, lineNum, wordStartCol);
    }
    close(fd);
}

void processFile(const char* filePath) {
    extractAndCheckWords(filePath);
}

void traverseFileSystem(const char* basePath) {
    DIR* dir = opendir(basePath);
    if (!dir) {
        return; // Unable to open directory
    }
    struct dirent* dp;
    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_name[0] == '.') continue; // Skip hidden files and directories
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", basePath, dp->d_name);
        struct stat pathStat;
        stat(path, &pathStat);
        if (S_ISDIR(pathStat.st_mode)) {
            traverseFileSystem(path); // Recursively traverse directories
        } else if (S_ISREG(pathStat.st_mode) && strstr(dp->d_name, ".txt")) {
            processFile(path); // Process .txt files
        }
    }
    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <dictionary_path> <text_file_or_directory_path>...\n", argv[0]);
        return 1;
    }

    loadDictionary(argv[1]);

    for (int i = 2; i < argc; i++) {
        struct stat pathStat;
        stat(argv[i], &pathStat);
        if (S_ISDIR(pathStat.st_mode)) {
            traverseFileSystem(argv[i]);
        } else if (S_ISREG(pathStat.st_mode)) {
            processFile(argv[i]);
        } else {
            printf("Unsupported file type: %s\n", argv[i]);
        }
    }

    // Memory cleanup for hashmap not included, but consider implementing for production code

    return 0;
}
