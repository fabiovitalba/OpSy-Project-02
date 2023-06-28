#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "jobs.h"
#include "filereader.h"

#define FILENAME "jobs.txt"
#define N_THREADS 3
#define VERBOSE 0

/**
 * Struct defining the parameters passed to a thread composed by:
 * the id of the thread (the integer index),
 * the pointer to the file from which jobs are fetched,
 * a pointer to the mutex used to synchronize the threads.
 */
struct thread_params {
    // index of the thread
    int thread_id;
    // pointer to the file from which jobs should be read
    FILE *jobs_file;
    // pointer to the mutex used to synchronize the threads
    pthread_mutex_t *mutex;
};

/**
 * Reads the next job from the file and outputs both the job type and its parameters.
 * @param params: pointer to the struct thread_params
 * @param job_type: pointer to a single character where the method stores
 * the type of job that needs to be performed. This will either be 's', 'm', 'f' or 'p'
 * (if a valid job is found), 0 if there are no more jobs, and -1 if an
 * invalid job is read from the file
 * @return a pointer to a string with the parameters of the job,
 * e.g., "3*4" for an 'm' job. This is dynamically
 * allocated and must be freed by the user when not needed anymore
 */
char* get_next_job(struct thread_params *params, char* job_type);

/**
 * Runs a job depending on the job_type in input and then calls another function
 * that actually runs the job.
 * @param params: pointer to the struct thread_params
 * @param job_type: character indicating the type of job
 * @param job_parameters: a string with the parameters of the job, taken from the jobs file
 */
void run_job(struct thread_params *params, char job_type, char *job_parameters);

/**
 * The following set of functions parse the parameters of the job and actually run the job
 * @param params: pointer to the struct thread_params
 * @param job_parameters: the string with the parameters for the job read from the jobs file
 */
void run_sleep(struct thread_params *params, char *job_parameters);

void run_math(struct thread_params *params, char *job_parameters);

void run_factorization(struct thread_params *params, char *job_parameters);

void run_prime(struct thread_params *params, char *job_parameters);

/**
 * This is the thread function. The aim of a thread is to read
 * from the file calling get_next_job() to obtain:
 * i) the job to be performed, and
 * ii) its relative parameters.
 * After reading the file, the thread runs the job, and prints the result.
 * The thread should use a mutex to synchronize
 * concurrent access both to the file and to the output (to avoid multiple threads
 * printing together using printf).
 * A thread terminates its infinite loop when no more jobs are found.
 * @param parameters: pointer to the struct thread_params
 * @return NULL
 */
void *thread(void *parameters);

int main() {
    FILE *file = open_file(FILENAME);
    pthread_mutex_t global_mutex;
    pthread_mutex_init(&global_mutex, NULL);

    pthread_t threads[N_THREADS];

    struct thread_params **params = malloc(sizeof(struct thread_params) * N_THREADS);
    for(int i = 0; i < N_THREADS; i++) {
        params[i] = malloc(sizeof(struct thread_params));
        params[i]->jobs_file = file;
        params[i]->mutex = &global_mutex;
        params[i]->thread_id = i + 1;

        if (VERBOSE) {
            printf("launching thread %d\n",params[i]->thread_id);
        }
        pthread_create(&(threads[i]),NULL,thread,params[i]);
    }
    
    for(int i = 0; i < N_THREADS; i++) {
        if (VERBOSE) {
            printf("waiting for thread %d\n",params[i]->thread_id);
        }
        pthread_join(threads[i],NULL);
        free(params[i]);
    }

    free(params);
    pthread_mutex_destroy(&global_mutex);
    close_file(file);
    if (VERBOSE) {
        printf("program completed.\n");
    }

    return 0;
}

void *thread(void *parameters) {
    struct thread_params *params = parameters;
    char job_type = -1;
    while(job_type != 0) {
        // Shortly lock the mutex, read the next line and release the mutex again.
        pthread_mutex_lock(params->mutex);
        char* job_params = get_next_job(params,&job_type);
        pthread_mutex_unlock(params->mutex);

        // If a valid job was acquired, execute the job.
        // Mutex locking is done inside each job, based on necessity.
        if (job_type > 0) {
            run_job(params,job_type,job_params);
            free(job_params);
        } else {
            if (job_type == 0) {
                pthread_mutex_lock(params->mutex);
                printf("Thread %d: No further jobs to be processed. Terminating.\n",params->thread_id);
                pthread_mutex_unlock(params->mutex);
            } else {
                pthread_mutex_lock(params->mutex);
                printf("Thread %d: Error getting next job.\n",params->thread_id);
                pthread_mutex_unlock(params->mutex);
            }
        }
    }
    return NULL;
}

char* get_next_job(struct thread_params *params, char* job_type) {
    // get a line from the file
    char *job_parameters;
    char *line = read_line(params->jobs_file);
    // return if no more line
    if (line == NULL) {
        *job_type = 0;
        return NULL;
    }
    // pick the first character which indicates the type of job
    *job_type = line[0];
    free(line);
    // read another line with job parameters
    job_parameters = read_line(params->jobs_file);
    switch (*job_type) {
        case 's':
        case 'm':
        case 'f':
        case 'p':
            return job_parameters;
        default:
            *job_type = -1;
            free(job_parameters);
            return NULL;
    }
}

void run_job(struct thread_params *params, char job_type, char *job_parameters) {
    switch (job_type) {
        case 's':
            run_sleep(params, job_parameters);
            break;
        case 'm':
            run_math(params, job_parameters);
            break;
        case 'f':
            run_factorization(params, job_parameters);
            break;
        case 'p':
            run_prime(params, job_parameters);
            break;
    }
}

void run_sleep(struct thread_params *params, char *job_parameters) {
    int seconds;
    if (sscanf(job_parameters, "%d", &seconds) == 1) {
        pthread_mutex_lock(params->mutex);
        printf("Thread %d: Running sleep(%d)\n",params->thread_id,seconds);
        pthread_mutex_unlock(params->mutex);

        job_sleep(seconds);

        pthread_mutex_lock(params->mutex);
        printf("Thread %d: Job terminated.\n",params->thread_id);
        pthread_mutex_unlock(params->mutex);
    } else {
        pthread_mutex_lock(params->mutex);
        printf("Thread %d: Error parsing sleep job parameters\n",params->thread_id);
        pthread_mutex_unlock(params->mutex);
    }
}

void run_math(struct thread_params *params, char *job_parameters) {
    float operand_a, operand_b;
    char operation;
    int operation_valid = 0;
    if (sscanf(job_parameters, "%f%c%f", &operand_a, &operation, &operand_b) == 3) {
        switch(operation) {
            case '+':
            case '-':
            case '*':
            case '/': {
                operation_valid = 1;
                pthread_mutex_lock(params->mutex);

                printf("Thread %d: Running math(%f %c %f)\n",params->thread_id,operand_a,operation,operand_b);
                pthread_mutex_unlock(params->mutex);
                float result = job_math(operand_a,operand_b,operation);
                
                pthread_mutex_lock(params->mutex);
                printf("Thread %d: Job terminated. Result: %f\n",params->thread_id,result);
                pthread_mutex_unlock(params->mutex);
                break;
            }
        }
    }

    if (operation_valid == 0) {
        pthread_mutex_lock(params->mutex);
        printf("Thread %d: Error parsing math job parameters\n",params->thread_id);
        pthread_mutex_unlock(params->mutex);
    }
}

void run_factorization(struct thread_params *params, char *job_parameters) {
    int number, size;
    if (sscanf(job_parameters,"%d",&number) == 1) {
        pthread_mutex_lock(params->mutex);
        printf("Thread %d: Running factorization(%d)\n",params->thread_id,number);
        pthread_mutex_unlock(params->mutex);
        int *factors = job_factorization(number, &size);
        pthread_mutex_lock(params->mutex);
        printf("Thread %d: Job terminated. Result: ",params->thread_id);
        for(int i = 0; i < size; i++) {
            printf("%d ",factors[i]);
        }
        printf("\n");
        pthread_mutex_unlock(params->mutex);
        free(factors);
    } else {
        pthread_mutex_lock(params->mutex);
        printf("Thread %d: Error parsing factorization job parameters\n",params->thread_id);
        pthread_mutex_unlock(params->mutex);
    }
}

void run_prime(struct thread_params *params, char *job_parameters) {
    int number;
    if (sscanf(job_parameters,"%d",&number) == 1) {
        pthread_mutex_lock(params->mutex);
        printf("Thread %d: Running prime(%d)\n",params->thread_id,number);
        pthread_mutex_unlock(params->mutex);
        int next_prime = job_prime(number);
        pthread_mutex_lock(params->mutex);
        printf("Thread %d: Job terminated. Result: %d\n",params->thread_id,next_prime);
        pthread_mutex_unlock(params->mutex);
    } else {
        pthread_mutex_lock(params->mutex);
        printf("Thread %d: Error parsing prime job parameters\n",params->thread_id);
        pthread_mutex_unlock(params->mutex);
    }
}
