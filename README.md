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

## Algorithm

1. **Initialization**: Load the dictionary from the specified file.
2. **File Processing**: For each text file or directory specified:
   - If it's a directory, recursively process all `.txt` files, ignoring those starting with `.`.
   - If it's a file, read its content, breaking it into words and checking each word against the dictionary.
3. **Error Reporting**: For each misspelled word, report the filename, line, and column number.
4. **Finalization**: Exit with `EXIT_SUCCESS` if all files were processed without errors; otherwise, `EXIT_FAILURE`.

## Test Files

The project includes several test files to validate the functionality of `spchk`, such as `basic_test.txt`, `cap.txt`, `hypen.txt`, `new.txt`, `punc.txt`, and `kanye.txt`. These files contain various test cases, including words with hyphens, punctuation, capitalization variations, and actual lyrics for real-world testing.

## Building and Running

Compile `spchk` using the provided `Makefile` with the command `make`. To run the spell checker:

```bash
./spchk <dictionary_path> <file_or_directory_path>...


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