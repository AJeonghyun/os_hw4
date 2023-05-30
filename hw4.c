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

int max_threads = 1;
int max_size = 1024;
char *output_file = NULL;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
FILE *output_fp = NULL;
int file_count = 0;
int search_count = 0;
int result_array_size = 1024;
char **result_array;
int result_array_index = 0;

#ifndef DT_DIR
#define DT_DIR 4
#endif

void *search_directory(void *arg);
void process_file(const char *path);
void handle_sigint(int sig);
void print_progress(int signum);

void *search_directory(void *dir_path)
{
    DIR *dir = opendir(dir_path);
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

        char full_path[max_size];
        snprintf(full_path, max_size, "%s/%s", (char *)dir_path, entry->d_name);

        if (entry->d_type == DT_DIR)
        {
            // Found a subdirectory, recursively search inside it
            search_directory(full_path);
        }
        else
        {
            process_file(full_path);
        }
    }

    closedir(dir);
}

void process_file(const char *path)
{
    pthread_mutex_lock(&result_mutex);
    if (output_fp == NULL)
    {
        if (output_file != NULL)
        {
            output_fp = fopen(output_file, "w");
            if (output_fp == NULL)
            {
                fprintf(stderr, "Failed to open output file: %s\n", output_file);
                pthread_mutex_unlock(&result_mutex);
                return;
            }
        }
        else
        {
            output_fp = stdout;
        }
    }

    if (file_count == result_array_size)
    {
        result_array_size *= 2;
        result_array = realloc(result_array, result_array_size * sizeof(char *));
        if (result_array == NULL)
        {
            fprintf(stderr, "Failed to allocate memory\n");
            pthread_mutex_unlock(&result_mutex);
            exit(1);
        }
    }

    result_array[file_count] = strdup(path);
    file_count++;

    pthread_mutex_unlock(&result_mutex);
}

void handle_sigint(int sig)
{
    signal(sig, SIG_IGN); // Ignore subsequent SIGINT signals
    pthread_mutex_lock(&result_mutex);
    if (output_fp != NULL && output_fp != stdout)
    {
        fclose(output_fp);
    }
    pthread_mutex_unlock(&result_mutex);
    exit(0);
}

void print_progress(int signum)
{
    pthread_mutex_lock(&result_mutex);
    fprintf(stderr, "Files found: %d\n", file_count);
    pthread_mutex_unlock(&result_mutex);
    alarm(5); // Set alarm for the next 5 seconds
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [OPTION] DIR\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "-t=", 3) == 0)
        {
            max_threads = atoi(argv[i] + 3);
            if (max_threads < 1 || max_threads > MAX_THREADS)
            {
                fprintf(stderr, "Invalid number of threads: %d\n", max_threads);
                return 1;
            }
        }
        else if (strncmp(argv[i], "-m=", 3) == 0)
        {
            max_size = atoi(argv[i] + 3);
            if (max_size < 1)
            {
                fprintf(stderr, "Invalid maximum size: %d\n", max_size);
                return 1;
            }
        }
        else if (strncmp(argv[i], "-o=", 3) == 0)
        {
            output_file = argv[i] + 3;
        }
    }

    char *dir_path = argv[argc - 1];

    signal(SIGINT, handle_sigint);
    signal(SIGALRM, print_progress);
    alarm(5); // Set alarm for the first 5 seconds

    pthread_t threads[MAX_THREADS];
    for (int i = 0; i < max_threads; i++)
    {
        pthread_create(&threads[i], NULL, search_directory, (void *)dir_path);
    }

    for (int i = 0; i < max_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_lock(&result_mutex);
    if (output_fp != NULL && output_fp != stdout)
    {
        fclose(output_fp);
    }
    pthread_mutex_unlock(&result_mutex);

    // Print the result array
    printf("[\n");
    for (int i = 0; i < file_count; i++)
    {
        printf("%s", result_array[i]);
        if (i < file_count - 1)
        {
            printf(",\n");
        }
    }
    printf("\n]\n");

    // Free allocated memory
    for (int i = 0; i < file_count; i++)
    {
        free(result_array[i]);
    }
    free(result_array);

    return 0;
}
