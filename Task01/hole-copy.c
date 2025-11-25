#include <fcntl.h>
#include <stdio.h>

char buf1[] = "abcdefghijklmnop";
char buf2[] = "ABCDEFGHIJKLMNOP";

int main(void) {
  int fd; // variable for file descriptor

  //   creat: return -1 in error case
  //   Default file access permissions for new files
  //      S_IRUSR | read permisson for user
  //      S_IWUSR | write pertmisson for user
  //      S_IRGRP | read permisson for group
  //      S_IROTH | read permisson for other`
  //      rw-r--r--
  if ((fd = creat("file.hole", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
    printf("ошибка вызова creat");
    return 1;
  }

  // write: return count of writen bites, -1 in error case
  if (write(fd, buf1, 16) != 16) {
    printf("ошибка записи buf1");
    return 1;
  }

  // lseek: return -1 in error
  //  SEEK_SET		/* Seek from beginning of file.  */
  //  SEEK_CUR		/* Seek from current position.  */
  //  SEEK_END		/* Seek from end of file.  */
  if (lseek(fd, 16368, SEEK_SET) == -1) {
    printf("ошибка вызова lseek");
    return 1;
  }

  if (write(fd, buf2, 16) != 16) {
    printf("ошибка записи buf2");
    return 1;
  }

  // block size - 4096
  // segment size - 512

  return 0;
}