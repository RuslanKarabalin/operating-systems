#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

char buf1[] = "abcdefghijklmnop";
char buf2[] = "ABCDEFGHIJKLMNOP";

int main(void) {
  int fd;

  if ((fd = creat("file.hole", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
    printf("ошибка вызова creat");
    return 1;
  }

  // first
  if (write(fd, buf1, 16) != 16) {
    printf("ошибка записи buf1");
    return 1;
  }

  if (lseek(fd, 2048, SEEK_SET) == -1) {
    printf("ошибка вызова lseek");
    return 1;
  }

  if (write(fd, buf2, 16) != 16) {
    printf("ошибка записи buf2");
    return 1;
  }

  // second
  if (lseek(fd, 4096, SEEK_SET) == -1) {
    printf("ошибка вызова lseek");
    return 1;
  }

  if (write(fd, buf1, 16) != 16) {
    printf("ошибка записи buf1");
    return 1;
  }

  if (lseek(fd, 6144, SEEK_SET) == -1) {
    printf("ошибка вызова lseek");
    return 1;
  }

  if (write(fd, buf2, 16) != 16) {
    printf("ошибка записи buf2");
    return 1;
  }

  return 0;
}