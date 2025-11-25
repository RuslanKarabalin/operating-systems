#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t north_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t south_cond = PTHREAD_COND_INITIALIZER;

int north_waiting = 0;
int south_waiting = 0;
int north_crossing = 0;
int south_crossing = 0;

struct car_args {
  int id;
  int direction; // 0 - north, 1 - south
};

void *car(void *arg) {
  car_args args = *((car_args *)arg);
  printf("id:%d, dir:%d\n", args.id, args.direction);
  pthread_mutex_lock(&mutex);
  if (args.direction == 0) {
    north_waiting++;
    if (south_crossing > 0 || (south_waiting > 0 && north_crossing == 0)) {
      printf("%d North Car is waiting\n", args.id);
      pthread_cond_wait(&north_cond, &mutex);
    }
    north_waiting--;
    printf("%d North Car drives onto the bridge\n", args.id);
    north_crossing++;
  } else {
    south_waiting++;
    if (north_crossing > 0 || (north_waiting > 0 && south_crossing == 0)) {
      printf("%d South Car is waiting\n", args.id);
      pthread_cond_wait(&south_cond, &mutex);
    }
    south_waiting--;
    printf("%d South Car drives onto the bridge\n", args.id);
    south_crossing++;
  }
  pthread_mutex_unlock(&mutex);

  if (args.direction == 0) {
    printf("%d North Car is crossing the bridge\n", args.id);
  } else {
    printf("%d South Car is crossing the bridge\n", args.id);
  }
  sleep(1);

  pthread_mutex_lock(&mutex);
  if (args.direction == 0) {
    north_crossing--;
    printf("%d North Car leaves the bridge\n", args.id);
    if (north_crossing == 0 && south_waiting > 0) {
      pthread_cond_broadcast(&south_cond);
    } else {
      pthread_cond_broadcast(&north_cond);
    }
  } else {
    south_crossing--;
    printf("%d South Car leaves the bridge\n", args.id);
    if (south_crossing == 0 && north_waiting > 0) {
      pthread_cond_broadcast(&north_cond);
    } else {
      pthread_cond_broadcast(&south_cond);
    }
  }
  pthread_mutex_unlock(&mutex);
  return NULL;
}

pthread_mutex_t pc_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pc_cond = PTHREAD_COND_INITIALIZER;
std::vector<pthread_t> threads;
int max_in_queue;

class Producer {
public:
  void add(int id, int dir) {
    pthread_mutex_lock(&pc_mutex);
    if (threads.size() + 1 <= max_in_queue) {
      car_args car_arg{id, dir};
      pthread_t thread;
      pthread_create(&thread, NULL, car, &car_arg);
      threads.emplace_back(thread);
      pthread_cond_broadcast(&pc_cond);
    } else {
      pthread_cond_wait(&pc_cond, &pc_mutex);
    }
    pthread_mutex_unlock(&pc_mutex);
  }

private:
};

class Consumer {
public:
  void run() {
    pthread_mutex_lock(&pc_mutex);
    if (threads.size() == 3) {
      for (auto &&i : threads) {
        pthread_join(i, NULL);
        threads.erase(threads.begin() + i);
      }
      pthread_cond_broadcast(&pc_cond);
    } else {
      pthread_cond_wait(&pc_cond, &pc_mutex);
    }
    pthread_mutex_unlock(&pc_mutex);
  }

private:
};

int main(int argc, char *argv[]) {
  max_in_queue = 10;
  Producer prod;
  Consumer cons;
  int i = 0;
  while (true) {
    prod.add(++i, rand() % 2);
    cons.run();
  }

  return 0;
}
