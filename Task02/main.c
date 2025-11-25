#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr,
            "Не верное количество параметров, требуется как минимум один "
            "параметр - размер буфера!\n");
    return 1;
  }

  int buf_size = atoi(argv[1]);
  if (buf_size <= 0) {
    fprintf(stderr, "Размер буфера должен быть больше 0!\n");
    return 1;
  }

  char buffer[buf_size];

  int cycles = 0;
  int bytes_count;
  while ((bytes_count = read(STDIN_FILENO, buffer, buf_size)) > 0) {
    write(STDOUT_FILENO, buffer, bytes_count);
    cycles++;
  }

  fprintf(stderr, "Всего циклов: %d\n", cycles);

  return 0;
}
