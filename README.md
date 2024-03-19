# Spelling Checker (`spchk`)

## Authors

- Jack Arteaga - NetID: ja1327
- Pranav Chundi - NetID: prc83

## Project Overview

The `spchk` program is a spelling checker tool designed for the CS 214 course. It reads a dictionary file and checks the spelling of words in one or more text files or directories, reporting any misspellings. This project aims to provide hands-on experience with POSIX file and directory processing functions.

## MetaData

- **Course:** CS 214, Spring 2024
- **Instructor:** David Menendez
- **Due Date:** March 18, 2024

- Jack Arteaga - NetID: ja1327
- Pranav Chundi - NetID: prc83

## Header Structure (`spchk.h`)

Defines constants and function prototypes essential for the operation of `spchk`. It includes:

- Maximum words in the dictionary
- Maximum word length
- Buffer size for file reading
- Prototypes for functions like `loadDictionary`, `processFile`, and `checkWord`

## Functions

Core functionalities of `spchk` include:

- **`loadDictionary`**: Loads words from the specified dictionary file into an array.
- **`findWordInDictionary`**: Searches for a given word in the dictionary.
- **`processFile`**: Processes a single text file to identify and report misspelled words.
- **`traverseDir`**: Recursively traverses directories to process `.txt` files.
- **`toLowerCase` and `toUpperCase`**: Utility functions for case conversion.
- **`stripPunctuation`**: Removes leading and trailing punctuation from words.
- **`checkWord`**: Checks if a word is spelled correctly according to the dictionary and assignment rules.

## Functions

### `loadDictionary(const char* dictionaryPath)`

- **Description**: Loads words from the specified dictionary file into a global array. Each word is stripped of its newline character before being stored.
- **Parameters**: `dictionaryPath` - Path to the dictionary file.

### `findWordInDictionary(const char* word)`

- **Description**: Searches for a given word in the dictionary.
- **Parameters**: `word` - The word to search for in the dictionary.
- **Returns**: `true` if the word is found, `false` otherwise.

### `processFile(const char* filePath)`

- **Description**: Reads the content of a file and checks each word for spelling errors. It correctly handles hyphenated words and ignores punctuation at the beginning and end of words.
- **Parameters**: `filePath` - Path to the text file to be processed.

### `traverseDir(const char* dirPath)`

- **Description**: Recursively traverses directories starting from the given path, processing all `.txt` files found. Ignores files and directories starting with a dot (`.`).
- **Parameters**: `dirPath` - Path to the directory to traverse.

### `toLowerCase(const char* input, char* output)`

- **Description**: Converts a string to lowercase, useful for case-insensitive comparison with the dictionary.
- **Parameters**: `input` - The input string to convert. `output` - The output buffer where the lowercase string will be stored.

### `isAllUpperCase(const char* str)`

- **Description**: Checks if a given string is in uppercase.
- **Parameters**: `str` - The string to check.

### `checkWord(const char* word)`

- **Description**: Checks if a word exists in the dictionary, considering various capitalization forms and special cases.
- **Parameters**: `word` - The word to check in the dictionary.

### `stripPunctuation(char* word)`

- **Description**: Removes leading and trailing punctuation from a word, preparing it for spelling checks.
- **Parameters**: `word` - The word from which to strip punctuation.

### `testStripPunctuation(const char* word, const char* expectedResult)`

- **Description**: A test function to verify the correctness of `stripPunctuation`.
- **Parameters**: `word` - The word to test. `expectedResult` - The expected result after stripping punctuation.

### `checkHyphenatedWord(const char* word)`

- **Description**: Checks each component of a hyphenated word against the dictionary.
- **Parameters**: `word` - The hyphenated word to check.


## Algorithm

1. **Initialization**: Load the dictionary from the specified file.
2. **File Processing**: For each text file or directory specified:
   - If it's a directory, recursively process all `.txt` files, ignoring those starting with `.`.
   - If it's a file, read its content, breaking it into words and checking each word against the dictionary.
3. **Error Reporting**: For each misspelled word, report the filename, line, and column number.
4. **Finalization**: Exit with `EXIT_SUCCESS` if all files were processed without errors; otherwise, `EXIT_FAILURE`.

# Analysis of Algorithms and Data Structures in `spchk`

## Data Structures

### Dictionary Storage

- **Array of Strings**: The dictionary is stored in a global 2D character array `dictionary[MAX_WORDS_IN_DICT][MAX_WORD_LENGTH]`. `MAX_WORDS_IN_DICT` and `MAX_WORD_LENGTH` define the maximum numbers of words and the maximum length of any word that the dictionary can contain, respectively. This structure allows direct access to any word based on its index, offering efficient word lookup operations, though it operates with linear search complexity.

## Algorithms

### Dictionary Loading (`loadDictionary`)

- **Linear File Reading**: Utilizes `fgets` to read the dictionary file line by line, storing each word into the `dictionary` array. It ensures that newline characters at the end of each word are removed, facilitating accurate word comparisons between the dictionary words and words from the input files.

### Word Lookup (`findWordInDictionary`)

- **Linear Search**: For a given word, this function performs a linear search through the `dictionary` array to check for the word's presence. While this method is straightforward, it has a time complexity of O(n), making it less efficient for searches in large dictionaries. Implementing a binary search could optimize this, reducing the time complexity to O(log n), but would require the dictionary to be sorted.

### File Processing (`processFile`)

- **Buffered Reading and State Machine**: Reads chunks of the input file into a buffer and iterates through each character to identify word boundaries, effectively using a simple state machine. This method handles large files and complex text structures efficiently but requires meticulous management of word boundaries and buffer edges.

### Punctuation Stripping (`stripPunctuation`)

- **In-place Modification**: Implements trimming of leading and trailing punctuation from words by adjusting the start pointer and null-terminating strings earlier. This preparation step is crucial for ensuring words are accurately compared against the dictionary entries.

### Hyphenated Word Checking (`checkHyphenatedWord`)

- **Tokenization and Recursion**: Employs `strtok` to split hyphenated words into their constituent parts and recursively checks each part against the dictionary. While this approach effectively handles hyphenated words, ensuring all components are valid, it modifies the input string in place, which may not be ideal in all cases.



## Test Files

The project includes several test files to validate the functionality of `spchk`, such as `basic_test.txt`, `cap.txt`, `hypen.txt`, `new.txt`, `punc.txt`, and `kanye.txt`. These files contain various test cases, including words with hyphens, punctuation, capitalization variations, and actual lyrics for real-world testing.

## Building and Running

Compile `spchk` using the provided `Makefile` with the command `make`. To run the spell checker:

```bash
./spchk <dictionary_path> <file_or_directory_path>...

## Installation and Running Instructions

### Compiling the Program

To compile the program, use the `make` command which reads the `Makefile` and compiles all the necessary files:

```bash
make

./spchk <dict_path> <test_file>

test 1: ./spchk /usr/share/dict/words basic_test.txt
test 2: ./spchk /usr/share/dict/words cap.txt
test 3: ./spchk /usr/share/dict/words case_sensitivity_test.txt
test 4: ./spchk /usr/share/dict/words empty.txt
test 5: ./spchk /usr/share/dict/words hypen.txt
test 6: ./spchk /usr/share/dict/words hypen1.txt
test 7: ./spchk /usr/share/dict/words large_file_test.txt
test 8: ./spchk /usr/share/dict/words mixed_content_test.txt
test 9: ./spchk /usr/share/dict/words jcole.txt
test 10: ./spchk /usr/share/dict/words kanye.txt
test 11: ./spchk /usr/share/dict/words slim_shady.txt
test 12: ./spchk /usr/share/dict/words test2.txt
test 13: ./spchk /usr/share/dict/words testfile1.txt
test 14: ./spchk /usr/share/dict/words punc.txt
test 15: ./spchk /usr/share/dict/words punctuation_hyphens_test.txt
test 16: ./spchk /usr/share/dict/words new.txt
test 17: ./spchk /usr/share/dict/words test

combined part 1: ./spchk /usr/share/dict/words test new.txt punctuation_hyphens_test.txt punc.txt testfile1.txt test2.txt mixed_content_test.txt large_file_test.txt hypen1.txt hypen.txt empty.txt case_sensitivity_test.txt cap.txt  basic_test.txt

combined part 2(songs): ./spchk /usr/share/dict/words jcole.txt kanye.txt slim_shady.txt
 


## Code Layout

The `spchk` project is organized into the following key components to ensure clarity and modularity:

- **Main Application (`spchk.c`)**: Serves as the entry point, handling command-line arguments, initiating dictionary loading, processing text files or directories, and running predefined tests.

- **Header File (`spchk.h`)**: Defines constants, function prototypes, and includes necessary libraries, acting as the interface for the application's functionality.

- **Makefile (`Makefile.txt`)**: Provides instructions for compiling the project, specifying compiler flags for warnings, memory debugging, and optimizations.

## Test Plan/Tests

The `spchk` program undergoes rigorous testing to cover a wide range of scenarios:

- **Basic**: Verifies the spelling checker's core functionality.
- **Directory Traversal**: Tests recursive processing of `.txt` files in directories.
- **Case Sensitivity**: Ensures the correct handling of case variations.
- **Punctuation & Hyphens**: Assesses the program's ability to deal with punctuation and hyphenated words.
- **Non-existent File**: Checks the program's error messaging for non-existent files.
- **Empty File**: Ensures proper handling of empty files.
- **Non-alphanumeric**: Tests handling of words with non-alphanumeric characters.
- **Mixed**: Combines various test scenarios into one to evaluate the program's resilience.
- **Large File**: Tests performance and memory usage with large text files.
- **Songs (`kanye.txt`)**: Uses complex real-world text structures for testing.
- **Extra Tests**: Additional tests designed to uncover edge cases.

#Strategy/testing/features


## Correctness

`spchk` achieves high correctness through:

- Rigorous error checking and handling.
- Comprehensive testing across a variety of text files and scenarios.
- Adherence to specified project requirements, particularly in handling edge cases.

## Design

Key design principles of `spchk` include:

- **Clarity**: Code is written to be easily readable, with meaningful names and comments.
- **Modularity**: Functionality is divided into specific, well-defined functions, facilitating maintenance and scalability.
- **Error Checking**: The application robustly handles errors, ensuring graceful failures and informative messages.
- **Documentation**: Both in-code comments and the README.md provide clear documentation on usage, functionality, and testing.

This design approach ensures that `spchk` is not only effective and reliable but also easy to understand, maintain, and extend.