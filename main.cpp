#include <array>
#include <atomic>
#include <zconf.h>
#include <semaphore.h>

const int philosophers_number = 5;

sem_t chopsticks[philosophers_number];

int leftChopstick(int i) {
    return i;
}

int rightChopstick(int i) {
    return (i + 1) % 5;
}

void *philosopherJob(void *arg) {
    bool eat = true;
    int id = (int) (uintptr_t) arg;
    while (eat) {
        usleep(400000);
        if (leftChopstick(id) < rightChopstick(id)) {
            printf("  Filozof %i, podnosi lewą pałeczkę\n", id);
            sem_wait(&chopsticks[leftChopstick(id)]);
            printf("  Filozof %i, podnosi prawą pałeczkę\n", id);
            sem_wait(&chopsticks[rightChopstick(id)]);
        } else {
            printf("  Filozof %i, podnosi prawą pałeczkę\n", id);
            sem_wait(&chopsticks[rightChopstick(id)]);
            printf("  Filozof %i, podnosi lewą pałeczkę\n", id);
            sem_wait(&chopsticks[leftChopstick(id)]);
        }
        printf("  Filozof %i, filozof podniósł obie pałeczki!\n", id);

        // Eat
        printf("Filozof %i, zaczyna jedzenie!\n", id);
        eat = false;
        usleep(400000);

        // Release chopsticks following the resource hierarchy
        if (leftChopstick(id) < rightChopstick(id)) {
            printf("  Filozof %i, odkłada prawą pałeczkę\n", id);
            sem_post(&chopsticks[rightChopstick(id)]);
            printf("  Filozof %i, odkłada lewą pałeczkę\n", id);
            sem_post(&chopsticks[leftChopstick(id)]);
        } else {
            printf("  Filozof %i, odkłada lewą pałeczkę\n", id);
            sem_post(&chopsticks[leftChopstick(id)]);
            printf("  Filozof %i, odkłada prawą pałeczkę\n", id);
            sem_post(&chopsticks[rightChopstick(id)]);
        }
        printf("~~~~~~~~  Filozof %i, odłożył obie pałeczki i zakończył jedzenie!!1\n", id);
    }
    return nullptr;
}

int main() {
    auto *philosophersThreadsIds = static_cast<pthread_t *>(malloc(philosophers_number * sizeof(pthread_t)));

    for (int i = 0; i < philosophers_number; ++i) {
        sem_init(&chopsticks[i], 0, 1);
        pthread_create(&philosophersThreadsIds[i], nullptr, philosopherJob, (void *) (uintptr_t) i);
    }

    // Wait for philosophers to finish
    for (int i = 0; i < philosophers_number; ++i) {
        pthread_join(philosophersThreadsIds[i], nullptr);
    }

    free(philosophersThreadsIds);
    for (int i = 0; i < philosophers_number; ++i) {
        sem_destroy(&chopsticks[i]);
    }

    return 0;
}
