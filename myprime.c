#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

#define BATCH_SIZE 100

// Original isPrime function
// bool isPrime(int n) {
//     if (n <= 1) {
//         return false;
//     }
//     for (int i = 2; i * i <= n; i++) {
//         if (n % i == 0) {
//             return false;
//         }
//     }
//     return true;
// }


bool isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    if (n <= 3) {
        return true;
    }
    if (n % 2 == 0 || n % 3 == 0) {
        return false;
    }
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}


// Queue structure for batch processing
typedef struct Queue {
    int* data;
    int capacity;
    int front;
    int rear;
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;

Queue queue;
atomic_int prime_count = 0;
atomic_bool done = false;

void initQueue(Queue* q, int capacity) {
    q->data = (int*)malloc(capacity * sizeof(int));
    q->capacity = capacity;
    q->front = 0;
    q->rear = -1;
    q->size = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void destroyQueue(Queue* q) {
    free(q->data);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

void push(Queue* q, int value) {
    pthread_mutex_lock(&q->mutex);
    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = value;
    q->size++;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

int popBatch(Queue* q, int* batch, int batch_size) {
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0 && !done) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    int count = 0;
    while (q->size > 0 && count < batch_size) {
        batch[count++] = q->data[q->front];
        q->front = (q->front + 1) % q->capacity;
        q->size--;
    }
    pthread_mutex_unlock(&q->mutex);
    return count;
}

void* primeCounter(void* arg) {
    int batch[BATCH_SIZE];
    while (!done || queue.size > 0) {
        int count = popBatch(&queue, batch, BATCH_SIZE);
        for (int i = 0; i < count; i++) {
            if (isPrime(batch[i])) {
                atomic_fetch_add(&prime_count, 1);
            }
        }
    }
    return NULL;
}

int main() {
    const int num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[num_threads];

    // Initialize the queue with a capacity of 10 times the batch size
    initQueue(&queue, 10 * BATCH_SIZE);

    // Start prime counting threads
    for (int i = 0; i < num_threads; ++i) {
        pthread_create(&threads[i], NULL, primeCounter, NULL);
    }

    // Read numbers from stdin and push them to the queue
    int num;
    while (scanf("%d", &num) != EOF) {
        push(&queue, num);
    }

    done = true;
    pthread_cond_broadcast(&queue.cond);

    // Join threads
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("%d total primes.\n", atomic_load(&prime_count));

    destroyQueue(&queue);
    return 0;
}
