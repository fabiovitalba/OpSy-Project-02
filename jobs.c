#include <stdlib.h>
#include <unistd.h>
#include "jobs.h"

#define SIZE_INCREASE 3

int is_prime(int number) {
  for (int i = 2; i * i <= number; i++)
    if (number % i == 0)
      return 0;
  return 1;
}

int next_prime(int current_prime) {
  do {
    current_prime++;
  }
  while (!is_prime(current_prime));
  return current_prime;
}

int job_prime(int n) {
  int prime = 2;
  if (n < 1)
    return 0;
  for (int i = 1; i < n; i++)
    prime = next_prime(prime);
  return prime;
}

void job_sleep(int time) {
  sleep(time);
}

float job_math(float a, float b, char operation) {
  switch (operation) {
  case '+':
    return a + b;
  case '-':
    return a - b;
  case '*':
    return a * b;
  case '/':
    return a / b;
  default:
    return 0;
  }
}

int *job_factorization(int number, int *size) {
  int *factors;
  int array_size = SIZE_INCREASE;
  int factor = 2;
  
  if (number < 2)
    return NULL;
  
  factors = (int *) malloc(sizeof(int) * SIZE_INCREASE);
  *size = 0;
  
  while (number != 1) {
    // check whether the number is divisible by the factor
    if (number % factor == 0) {
      // if there is no more space available, resize the array
      if (*size == array_size) {
        array_size += SIZE_INCREASE;
        factors = (int *) realloc(factors, sizeof(int) * array_size);
      }
      // add the factor to the list of prime factors
      factors[*size] = factor;
      (*size)++;
      number = number / factor;
    }
    else {
      // if not a factor, move to the next prime number
      factor = next_prime(factor);
    }
  }
  return factors;
}
