#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <math.h>


struct LockFreeQueue queue;
int prime_count = 0;

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


typedef struct Node {
    int value;
    struct Node* next;
} Node;

typedef struct LockFreeQueue{
    Node* head;
    Node* tail;
}LockFreeQueue;

void initQueue(LockFreeQueue* q) {
    q = (struct LockFreeQueue*)malloc(sizeof(struct LockFreeQueue));
    q->head = q->tail = NULL;
}

struct Node* createNewNode(int val)
{
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->value = val;
    temp->next = NULL;
    return temp;
}

void enqueue(LockFreeQueue* q, int value){
    // Create a new LL node
    struct Node* newNode = createNewNode(value);
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));

    do {
        temp = q->tail;

        if(__sync_bool_compare_and_swap(&q->tail, NULL, newNode)){ // If queue is empty, then new node is head and tail
            q->head = newNode;
            break;
        }else if ( __sync_bool_compare_and_swap(&q->tail, temp, newNode)) {
            temp->next=newNode;
            break;
        }
    } while(1);
}

int dequeue(LockFreeQueue* q){
    int val;
    if(!q->head){
        return -1;
    }

    struct Node* temp;
    do{
        temp = q->head;
        if(__sync_bool_compare_and_swap(&q->head, temp, q->head->next)){
            val = temp->value;
            if (q->head == NULL){
                q->tail = NULL;
            }
            break;
        }
    } while(1);
    free(temp);
    return val;
}

void destroyQueue(LockFreeQueue* q) {
    while(queue.head != NULL){
        dequeue(q);
    }
}

void* primeCounter(void* arg) {
    int val;
    while ((val = dequeue(&queue)) != -1)
    {
        if(isPrime(val)){
            __sync_add_and_fetch(&prime_count, 1);
        }
    }
    return NULL;
}

int main() {
    const int num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[num_threads];

    // Initialize the queue
    initQueue(&queue);



    // Read numbers from stdin and push them to the queue
    int num;
    while (scanf("%d", &num) != EOF) {
        enqueue(&queue, num);
    }

    // Start prime counting threads
    for (int i = 0; i < num_threads; ++i) {
        int t = pthread_create(&threads[i], NULL, primeCounter, NULL);
    }

    // Join threads
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("%d total primes.\n", prime_count);
    //, atomic_load(&prime_count)

    destroyQueue(&queue);
    return 0;
}
