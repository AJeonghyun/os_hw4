#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

#define MAX_THREADS 64

int default_threads = 1;
int default_file_size = 1024;
char *output_file = NULL;
pthread_mutex_t file_paths_mutex = PTHREAD_MUTEX_INITIALIZER;
FILE *output_fp = NULL;
int file_count = 0;
int search_count = 0;
int file_paths_size = 1024;
char **file_paths;
int file_paths_index = 0;

void searchDirectory(void *dir_path);
void processFile(const char *path);
void handleSigInt(int sig);
void printProgress(int signum);

void searchDirectory(void *dir_path)
{
    DIR *dir = opendir((char *)dir_path);
    if (dir == NULL)
    {
        perror("Failed to open directory");
        exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char full_path[default_file_size];
        snprintf(full_path, default_file_size, "%s/%s", (char *)dir_path, entry->d_name);

        if (entry->d_type == DT_DIR)
        {
            // Found a subdirectory, recursively search inside it
            searchDirectory(full_path);
        }
        else
        {
            processFile(full_path);
        }
    }

    closedir(dir);
}

void processFile(const char *path)
{
    pthread_mutex_lock(&file_paths_mutex);
    if (output_fp == NULL)
    {
        if (output_file != NULL)
        {
            output_fp = fopen(output_file, "w");
            if (output_fp == NULL)
            {
                fprintf(stderr, "Failed to open output file: %s\n", output_file);
                pthread_mutex_unlock(&file_paths_mutex);
                return;
            }
        }
        else
        {
            output_fp = stdout;
        }
    }

    char *extension = strrchr(path, '.');
    if (extension != NULL)
    {
        extension++; // Move past the dot (.)
    }

    if (file_paths_index == file_paths_size)
    {
        file_paths_size *= 2;
        file_paths = realloc(file_paths, file_paths_size * sizeof(char *));
        if (file_paths == NULL)
        {
            fprintf(stderr, "Failed to allocate memory\n");
            pthread_mutex_unlock(&file_paths_mutex);
            exit(1);
        }
    }

    file_paths[file_paths_index] = strdup(path);
    file_paths_index++;
    file_count++; // Increase file_count for each new file discovered

    if (output_fp != stdout)
    {
        fprintf(output_fp, "%s\n", path);
    }

    pthread_mutex_unlock(&file_paths_mutex);
}

void handleSigInt(int sig)
{
    signal(sig, SIG_IGN); // Ignore subsequent SIGINT signals
    pthread_mutex_lock(&file_paths_mutex);
    if (output_fp != NULL && output_fp != stdout)
    {
        fclose(output_fp);
    }
    pthread_mutex_unlock(&file_paths_mutex);
    exit(0);
}

void printProgress(int signum)
{
    pthread_mutex_lock(&file_paths_mutex);
    fprintf(stderr, "Files found: %d\n", file_count);
    fprintf(stderr, "Search count: %d\n", search_count);
    pthread_mutex_unlock(&file_paths_mutex);
    signal(SIGALRM, printProgress); // Re-register the signal handler
    alarm(5); // Set alarm for the next 5 seconds
}

void *threadStart(void *dir_path)
{
    searchDirectory(dir_path);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [OPTION] DIR\n", argv[0]);
        return 1;
    }

    file_paths = calloc(file_paths_size, sizeof(char *));
    if (file_paths == NULL)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "-t=", 3) == 0)
        {
            default_threads = atoi(argv[i] + 3);
            if (default_threads < 1 || default_threads > MAX_THREADS)
            {
                fprintf(stderr, "Invalid number of threads: %d\n", default_threads);
                return 1;
            }
        }
        else if (strncmp(argv[i], "-m=", 3) == 0)
        {
            default_file_size = atoi(argv[i] + 3);
            if (default_file_size < 1)
            {
                fprintf(stderr, "Invalid maximum size: %d\n", default_file_size);
                return 1;
            }
        }
        else if (strncmp(argv[i], "-o=", 3) == 0)
        {
            output_file = argv[i] + 3;
        }
    }

    char *dir_path = argv[argc - 1];

    signal(SIGINT, handleSigInt);
    signal(SIGALRM, printProgress);
    alarm(5); // Set alarm for the first 5 seconds

    pthread_t threads[MAX_THREADS];
    for (int i = 0; i < default_threads; i++)
    {
        pthread_create(&threads[i], NULL, threadStart, dir_path);
    }

    for (int i = 0; i < default_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_lock(&file_paths_mutex);
    if (output_fp != NULL && output_fp != stdout)
    {
        fclose(output_fp);
    }
    pthread_mutex_unlock(&file_paths_mutex);

    // Print the result array with grouped file paths
    printf("[\n");

    char *current_extension = NULL;
    for (int i = 0; i < file_paths_index; i++)
    {
        char *extension = strrchr(file_paths[i], '.');
        if (extension != NULL)
        {
            extension++; // Move past the dot (.)
        }

        if (current_extension == NULL || strcmp(current_extension, extension) != 0)
        {
            // Start a new group
            if (current_extension != NULL)
            {
                printf("\n],\n");
            }
            printf("[\n");
            current_extension = extension;
        }
        else
        {
            printf(",\n");
        }

        printf("%s", file_paths[i]);
    }

    if (current_extension != NULL)
    {
        printf("\n]\n");
    }

    printf("]\n");

    // Free allocated memory
    for (int i = 0; i < file_paths_index; i++)
    {
        free(file_paths[i]);
    }
    free(file_paths);

    return 0;
}
