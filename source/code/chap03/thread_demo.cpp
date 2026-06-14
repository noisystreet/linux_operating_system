#include <iostream>
#include <pthread.h>

const int N = 1'000'000;
long counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_func(void*) {
    for (int i = 0; i < N; ++i) {
        pthread_mutex_lock(&mutex);
        ++counter;
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, thread_func, nullptr);
    pthread_create(&t2, nullptr, thread_func, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);

    std::cout << "counter = " << counter
              << " (期望 " << 2 * N << ")\n";
    return 0;
}
