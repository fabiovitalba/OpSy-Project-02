#ifndef JOBS_H
#define JOBS_H

/**
 * Performs a sleep operation
 * @param time amount of time in seconds
 */
void job_sleep(int time);

/**
 * Performs a mathematical operation between two numbers
 * @param a first number
 * @param b second number
 * @param operation a character indicating the operation, which can be + - * /
 * @return the result of the given operation. If the operation is invalid the function returns 0
 */
float job_math(float a, float b, char operation);

/**
 * Computes the n-th prime number
 * @param n the index of the prime number to compute (1-based)
 * @return the n-th prime number
 */
int job_prime(int n);

/**
 * Performs the prime decomposition of a number
 * @param number number for which prime factors should be found. The number should be greater or equal than 2
 * @param size number of factors (size of the returned array)
 * @return a dynamically allocated array with "size" elements with all the prime factors or null if the given number
 * is invalid
 */
int* job_factorization(int number, int *size);

#endif
