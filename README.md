# Command Line
- gcc -o <executable file name> findeq.c -pthread
- ./findeq -t=8 -m=1024 -o=output.txt ./Files

# Global variables
- `MAX_THREADS`: A constant that defines the maximum number of threads.
- `default_threads`: The default number of threads.
- `default_file_size`: The initial size of the file path array.
- `output_file`: The path of the output file. It defaults to `NULL`, and if no file is specified, the output is directed to stdout.
- `file_paths_mutex`: A mutex (mutex) used to control concurrent access to file paths.
- `output_fp`: The file pointer of the output file.
- `file_count`: The number of discovered files.
- `search_count`: The count of directory searches.
- `file_paths_size`: The size of the file path array.
- `file_paths`: An array that stores the paths of discovered files.
- `file_paths_index`: The current index in the file path array.

# Functions
- `searchDirectory`: A function that searches for files in a given directory.
- `processFile`: A function that processes a file, including printing its path.
- `handleSigInt`: A function that handles the SIGINT signal, closing the output file before terminating the program.
- `printProgress`: A function that periodically prints the number of discovered files and search count.
- `threadStart`: The start function for threads, which calls the `searchDirectory` function.

The `searchDirectory` function opens the given directory and iterates over its entries. For each entry, it either calls the `processFile` function to handle a file or recursively searches subdirectories.

The `processFile` function processes the given file path and prints the path. It performs the following tasks:
- Locks the `file_paths_mutex` mutex to control concurrent access.
- If the output file (`output_fp`) is not open, it opens the file based on the specified output file path (`output_file`). If opening the file fails, it prints an error message and returns.
- Extracts the extension from the file path.
- If the `file_paths` array is full, it doubles the array size.
- Adds the file path to the `file_paths` array and increments the index.
- Increments the count of discovered files.
- If the output file is not stdout, it prints the file path to the file.
- Unlocks the `file_paths_mutex` mutex.

The `handleSigInt` function handles the SIGINT signal. It performs the following tasks:
- Changes the signal handling to ignore subsequent SIGINT signals.
- Locks the `file_paths_mutex` mutex to control concurrent access.
- If the output file is not NULL and not stdout, it closes the file.
- Unlocks the `file_paths_mutex` mutex.
- Exits the program.

The `printProgress` function periodically prints the number of discovered files and search count. It performs the following tasks:
- Locks the `file_paths_mutex` mutex to control concurrent access.
- Prints the number of files and search count to stderr.
- Re-registers the signal handler for the SIGALRM signal.
- Sets an alarm for the next 5 seconds.

The `threadStart` function is the start function for threads. It calls the `searchDirectory` function on the given directory.

The `main` function is the entry point of the program. It performs the following tasks:
- Processes the command-line arguments, checking for the directory path, number of threads, and output file path.
- Initializes the file path array.
- Sets the handling for the SIGINT signal.
- Registers a handler to periodically print the number of discovered files and search count.
- Creates threads and starts directory search from the given directory.
- Waits for the threads to finish.
- Closes the result file.
- Frees the file path array.

