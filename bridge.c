#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NORTH 0
#define SOUTH 1
#define MAX_CONSECUTIVE 3
#define NUM_FARMERS 10

static pthread_mutex_t lock;
static pthread_mutex_t rand_lock;
static pthread_cond_t north_cond, south_cond;
static int north_on_bridge = 0;
static int south_on_bridge = 0;
static int north_waiting = 0;
static int south_waiting = 0;
static int turn = NORTH;
static int consecutive_count = 0;

void error_exit(const char *msg, int err) {
    fprintf(stderr, "%s: %d\n", msg, err);
    exit(EXIT_FAILURE);
}

int safe_rand(int min, int max) {
    pthread_mutex_lock(&rand_lock);
    int r = rand() % (max - min + 1) + min;
    pthread_mutex_unlock(&rand_lock);
    return r;
}

void *northbound(void *arg) {
    int id = *((int *)arg);
    free(arg);

    pthread_mutex_lock(&lock);
    while (south_on_bridge > 0 || 
           (turn == SOUTH && south_waiting > 0) ||
           (turn == NORTH && consecutive_count >= MAX_CONSECUTIVE && south_waiting > 0)) {
        north_waiting++;
        printf("Northbound farmer %d waiting...\n", id);
        pthread_cond_wait(&north_cond, &lock);
        north_waiting--;
    }

    north_on_bridge++;
    if (turn == NORTH) {
        consecutive_count++;
    } else {
        turn = NORTH;
        consecutive_count = 1;
    }
    printf("Northbound farmer %d started crossing. [%d on bridge]\n", id, north_on_bridge);
    pthread_mutex_unlock(&lock);

    sleep(safe_rand(1, 3));

    pthread_mutex_lock(&lock);
    north_on_bridge--;
    printf("Northbound farmer %d finished crossing. [%d on bridge]\n", id, north_on_bridge);
    
    if (north_on_bridge == 0) {
        if (south_waiting > 0) {
            turn = SOUTH;
            consecutive_count = 0;
            pthread_cond_broadcast(&south_cond);
        } else if (north_waiting > 0) {
            pthread_cond_broadcast(&north_cond);
        }
    }
    pthread_mutex_unlock(&lock);

    return NULL;
}

void *southbound(void *arg) {
    int id = *((int *)arg);
    free(arg);

    pthread_mutex_lock(&lock);
    while (north_on_bridge > 0 || 
           (turn == NORTH && north_waiting > 0) ||
           (turn == SOUTH && consecutive_count >= MAX_CONSECUTIVE && north_waiting > 0)) {
        south_waiting++;
        printf("Southbound farmer %d waiting...\n", id);
        pthread_cond_wait(&south_cond, &lock);
        south_waiting--;
    }

    south_on_bridge++;
    if (turn == SOUTH) {
        consecutive_count++;
    } else {
        turn = SOUTH;
        consecutive_count = 1;
    }
    printf("Southbound farmer %d started crossing. [%d on bridge]\n", id, south_on_bridge);
    pthread_mutex_unlock(&lock);

    sleep(safe_rand(1, 3));

    pthread_mutex_lock(&lock);
    south_on_bridge--;
    printf("Southbound farmer %d finished crossing. [%d on bridge]\n", id, south_on_bridge);
    
    if (south_on_bridge == 0) {
        if (north_waiting > 0) {
            turn = NORTH;
            consecutive_count = 0;
            pthread_cond_broadcast(&north_cond);
        } else if (south_waiting > 0) {
            pthread_cond_broadcast(&south_cond);
        }
    }
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main(void) {
    pthread_t threads[2 * NUM_FARMERS];
    int err;

    srand(time(NULL));

    err = pthread_mutex_init(&lock, NULL);
    if (err != 0) error_exit("mutex_init failed", err);
    
    err = pthread_mutex_init(&rand_lock, NULL);
    if (err != 0) error_exit("rand_mutex_init failed", err);
    
    err = pthread_cond_init(&north_cond, NULL);
    if (err != 0) error_exit("north_cond_init failed", err);
    
    err = pthread_cond_init(&south_cond, NULL);
    if (err != 0) error_exit("south_cond_init failed", err);

    int created_north = 0;
    int created_south = 0;
    
    for (int i = 0; i < 2 * NUM_FARMERS; i++) {
        if ((i % 2 == 0 && created_north < NUM_FARMERS) || created_south >= NUM_FARMERS) {
            int *id = malloc(sizeof(int));
            if (!id) error_exit("malloc failed", 1);
            *id = created_north++;
            err = pthread_create(&threads[i], NULL, northbound, id);
            if (err != 0) error_exit("pthread_create north failed", err);
        } else {
            int *id = malloc(sizeof(int));
            if (!id) error_exit("malloc failed", 1);
            *id = created_south++;
            err = pthread_create(&threads[i], NULL, southbound, id);
            if (err != 0) error_exit("pthread_create south failed", err);
        }
        usleep(safe_rand(50000, 150000));
    }

    for (int i = 0; i < 2 * NUM_FARMERS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&rand_lock);
    pthread_cond_destroy(&north_cond);
    pthread_cond_destroy(&south_cond);

    return 0;
}