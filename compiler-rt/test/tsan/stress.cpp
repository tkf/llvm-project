// RUN: %clangxx_tsan -O1 %s -o %t && %env_tsan_opts="flush_memory_ms=1 flush_symbolizer_ms=1 memory_limit_mb=1" %run %t 2>&1 | FileCheck %s
#include "test.h"
#include <fcntl.h>
#include <string.h>

volatile long stop;
long atomic, read_only;
int fds[2];

__attribute__((noinline)) void *SecondaryThread(void *x) {
  __atomic_fetch_add(&atomic, 1, __ATOMIC_ACQ_REL);
  return NULL;
}

void *Thread(void *x) {
  const int me = (long)x;
  volatile long sink = 0;
  while (!stop) {
    // If me == 0, we do all of the following,
    // otherwise only 1 type of action.
    if (me == 0 || me == 1) {
      // just read the stop variable
    }
    if (me == 0 || me == 2) {
      __atomic_store_n(&atomic, 1, __ATOMIC_RELEASE);
    }
    if (me == 0 || me == 3) {
      sink += __atomic_fetch_add(&atomic, 1, __ATOMIC_ACQ_REL);
    }
    if (me == 0 || me == 4) {
      SecondaryThread(NULL);
    }
    if (me == 0 || me == 5) {
      write(fds[1], fds, 1);
    }
    if (me == 0 || me == 6) {
      char buf[2];
      read(fds[0], &buf, sizeof(buf));
    }
    if (me == 0 || me == 7) {
      pthread_t th;
      pthread_create(&th, NULL, SecondaryThread, NULL);
      pthread_join(th, NULL);
    }
    if (me == 0 || me == 8) {
      long buf;
      memcpy(&buf, &read_only, sizeof(buf));
      sink += buf;
    }
    // If you add more actions, update kActions in main.
  }
  return NULL;
}

int main() {
  ANNOTATE_BENIGN_RACE(stop);
  if (pipe(fds))
    exit((perror("pipe"), 1));
  if (fcntl(fds[0], F_SETFL, O_NONBLOCK))
    exit((perror("fcntl"), 1));
  if (fcntl(fds[1], F_SETFL, O_NONBLOCK))
    exit((perror("fcntl"), 1));
  const int kActions = 9;
  const int kMultiplier = 4;
  pthread_t t[kActions * kMultiplier];
  for (int i = 0; i < kActions * kMultiplier; i++)
    pthread_create(&t[i], NULL, Thread, (void *)(long)(i % kActions));
  sleep(5);
  stop = 1;
  for (int i = 0; i < kActions * kMultiplier; i++)
    pthread_join(t[i], NULL);
  fprintf(stderr, "DONE\n");
  return 0;
}

// CHECK-NOT: ThreadSanitizer:
// CHECK: DONE
// CHECK-NOT: ThreadSanitizer:
