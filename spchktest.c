#include "dictionary.h"
#include "file_reader.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <dictionary_path> <file1> [file2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    load_dictionary(argv[1]);
    for (int i = 2; i < argc; i++) {
        process_file(argv[i]);
    }
    // Add any necessary cleanup and return an appropriate exit status
    return EXIT_SUCCESS;
}
