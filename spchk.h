#ifndef SPCHK_H
#define SPCHK_H

#define MAX_WORDS_IN_DICT 110000
#define MAX_WORD_LENGTH 100
#define BUFFER_SIZE 1024

#include <stdbool.h>

void loadDictionary(const char* dictionaryPath);
bool findWordInDictionary(const char* word);
void processFile(const char* filePath);
void traverseDir(const char* dirPath);
void toLowerCase(const char* input, char* output);
void toUpperCase(char* str);
bool isAllUpperCase(const char* str);
bool checkWord(const char* word);
bool checkHyphenatedWord(const char* word);
void stripPunctuation(char* word);
bool checkSpecialUpperCase(const char* word);
void testStripPunctuation(const char* input, const char* expectedOutput);

#endif /* SPCHK_H */
