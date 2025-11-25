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

class Bridge {
public:
  Bridge(int max_on_bridge_arg) : max_on_bridge{max_on_bridge_arg} {}

  void add(int id, int dir) {
    if (dir == 0) {
      n_cars.push({id, dir});
    } else {
      s_cars.push({id, dir});
    }
  }

  void run() {
    if (n_cars.size() > 0) {
      while (!n_cars.empty()) {
        car_args car_arg = n_cars.front();
        // printf("id:%d, dir:%d\n", car_arg.id, car_arg.direction);
        pthread_t thread;
        pthread_create(&thread, NULL, car, &car_arg);
        n_cars.pop();
        threads.push(thread);
      }
      while (!threads.empty()) {
        pthread_t t = threads.front();
        threads.pop();
        pthread_join(t, NULL);
      }
    }
    if (s_cars.size() > 0) {
      while (!s_cars.empty()) {
        car_args car_arg = s_cars.front();
        // printf("id:%d, dir:%d\n", car_arg.id, car_arg.direction);
        pthread_t thread;
        pthread_create(&thread, NULL, car, &car_arg);
        s_cars.pop();
        threads.push(thread);
      }
      while (!threads.empty()) {
        pthread_t t = threads.front();
        threads.pop();
        pthread_join(t, NULL);
      }
    }
    // printf("%d\n", threads.size());
  }

private:
  int max_on_bridge;
  std::queue<pthread_t> threads;
  std::queue<car_args> n_cars;
  std::queue<car_args> s_cars;
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <max_number_of_cars>\n", argv[0]);
    return 1;
  }

  int max_size = atoi(argv[1]);
  if (max_size < 1) {
    fprintf(stderr, "Max number of cars can't be less than 1\n");
    return 1;
  }
  Bridge bridge(max_size);
  int i = 0;
  while (true) {
    bridge.add(++i, rand() % 2);
    bridge.run();
    // printf("---------------------------------------\n");
  }

  return 0;
}
