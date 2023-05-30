# 전역 변수
- `MAX_THREADS`: 최대 스레드 개수를 정의하는 상수입니다.
- `default_threads`: 기본 스레드 개수입니다.
- `default_file_size`: 파일 경로를 저장하기 위한 배열의 초기 크기입니다.
- `output_file`: 출력 파일의 경로입니다. 기본값은 NULL이며, 파일이 지정되지 않은 경우 출력은 표준 출력으로 이루어집니다.
- `file_paths_mutex`: 파일 경로에 대한 동시 액세스를 제어하기 위한 뮤텍스(mutex)입니다.
- `output_fp`: 출력 파일의 파일 포인터입니다.
- `file_count`: 발견된 파일의 개수입니다.
- `search_count`: 디렉토리 탐색 횟수입니다.
- `file_paths_size`: 파일 경로 배열의 크기입니다.
- `file_paths`: 발견된 파일의 경로를 저장하는 배열입니다.
- `file_paths_index`: 파일 경로 배열에서의 현재 인덱스입니다.

# 함수
- `searchDirectory`: 주어진 디렉토리에서 파일을 검색하는 함수입니다.
- `processFile`: 파일을 처리하고 경로를 출력하는 함수입니다.
- `handleSigInt`: SIGINT 신호를 처리하는 함수로, 프로그램을 종료하기 전에 출력 파일을 닫습니다.
- `printProgress`: 주기적으로 발견된 파일 수와 탐색 횟수를 출력하는 함수입니다.
- `threadStart`: 스레드의 시작 함수로, `searchDirectory` 함수를 호출합니다.

`searchDirectory` 함수는 주어진 디렉토리를 열고, 디렉토리 안의 엔트리들을 반복적으로 탐색합니다. 각 엔트리의 종류에 따라 `processFile` 함수를 호출하여 파일을 처리하거나 하위 디렉토리를 재귀적으로 탐색합니다.

`processFile` 함수는 주어진 파일 경로를 처리하고 경로를 출력합니다. 함수는 다음 작업을 수행합니다:

- `file_paths_mutex` 뮤텍스를 잠금 상태로 변경하여 동시 액세스를 제어합니다.
- 출력 파일 (`output_fp`)이 열려 있지 않으면, 지정된 출력 파일 경로 (`output_file`)에 따라 파일을 엽니다. 파일 열기에 실패한 경우 오류 메시지를 출력하고 함수를 종료합니다.
- 파일 경로에서 확장자를 추출합니다.
- `file_paths` 배열이 가득 찬 경우, 배열 크기를 두 배로 늘립니다.
- 파일 경로를 `file_paths` 배열에 추가하고 인덱스를 증가시킵니다.
- 발견된 파일의 수를 증가시킵니다.
- 출력 파일이 표준 출력이 아닌 경우 파일에 파일 경로를 출력합니다.
- `file_paths_mutex` 뮤텍스를 잠금 해제합니다.

`handleSigInt` 함수는 SIGINT

 신호를 처리합니다. 함수는 다음 작업을 수행합니다:

- 후속 SIGINT 신호를 무시하도록 신호 처리를 변경합니다.
- `file_paths_mutex` 뮤텍스를 잠금 상태로 변경하여 동시 액세스를 제어합니다.
- 출력 파일이 표준 출력이 아니고 열려 있으면 파일을 닫습니다.
- `file_paths_mutex` 뮤텍스를 잠금 해제합니다.
- 프로그램을 종료합니다.

`printProgress` 함수는 발견된 파일 수와 탐색 횟수를 주기적으로 출력합니다. 함수는 다음 작업을 수행합니다:

- `file_paths_mutex` 뮤텍스를 잠금 상태로 변경하여 동시 액세스를 제어합니다.
- stderr에 파일 수와 탐색 횟수를 출력합니다.
- SIGALRM 신호에 대한 신호 처리기를 다시 등록합니다.
- 다음 5초 동안 알람을 설정합니다.

`threadStart` 함수는 스레드의 시작 함수입니다. 주어진 디렉토리에서 `searchDirectory` 함수를 호출합니다.

`main` 함수는 프로그램의 진입점입니다. 함수는 다음 작업을 수행합니다:

- 커맨드 라인 인자를 처리합니다. 디렉토리 경로, 스레드 개수 및 출력 파일 경로를 확인합니다.
- 파일 경로 배열을 초기화합니다.
- SIGINT 신호에 대한 처리 방식을 설정합니다.
- 주기적으로 발견된 파일 수와 탐색 횟수를 출력하는 핸들러를 등록합니다.
- 스레드를 생성하고 주어진 디렉토리에서 디렉토리 탐색을 시작합니다.
- 스레드의 종료를 기다립니다.
- 결과 파일을 닫습니다.
- 파일 경로 배열을 해제합니다.



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

