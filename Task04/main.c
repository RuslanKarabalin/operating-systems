#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Invalid params count\n");
    return 1;
  }

  if (access(argv[1], R_OK) == -1) {
    fprintf(stderr, "Error can't read from file \"%s\"\n", argv[1]);
    return 1;
  }

  if (access(argv[2], F_OK) == 0) {
    struct stat src_stat;
    if (stat(argv[1], &src_stat) == -1) {
      fprintf(stderr, "Error can't call stat for \"%s\"\n", argv[1]);
      return 1;
    }

    struct stat dest_stat;
    if (stat(argv[2], &dest_stat) == -1) {
      fprintf(stderr, "Error can't call stat for \"%s\"\n", argv[2]);
      return 1;
    } else {
      if (src_stat.st_dev == dest_stat.st_dev &&
          src_stat.st_ino == dest_stat.st_ino) {
        fprintf(stderr, "Error \"%s\" and \"%s\" are the same file\n", argv[1],
                argv[2]);
        return 1;
      }
    }
    char answer = '\0';
    char enter = '\n';
    printf("File already exist, want to overwrite? (y/N)\n");
    do {
      scanf("%c%c", &answer, &enter);
      if (answer == 'n' || answer == 'N') {
        printf("Stop copying\n");
        return 0;
      } else if (answer == 'y' || answer == 'Y') {
        printf("Overwriting\n");
      } else {
        printf("Incorrect value: %c, enter Y or N\n", answer);
      }
    } while (answer != 'y' && answer != 'Y' && answer != 'n' && answer != 'N');
  }

  int src_fd = open(argv[1], O_RDONLY);
  if (src_fd == -1) {
    fprintf(stderr, "Error opening \"%s\" file\n", argv[1]);
    return 1;
  }

  int dest_fd = creat(argv[2], S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (dest_fd == -1) {
    fprintf(stderr, "Error opening \"%s\" file\n", argv[2]);
    return 1;
  }

  char read_buf[BUF_SIZE];
  int bytes;

  char write_buf[BUF_SIZE];
  int bytes_to_write = 0;

  while ((bytes = read(src_fd, read_buf, sizeof(read_buf))) > 0 ||
         bytes_to_write > 0) {
    for (int i = 0; i < bytes; ++i) {
      if (read_buf[i] != 0) {
        write_buf[bytes_to_write++] = read_buf[i];
        if (bytes_to_write == BUF_SIZE) {
          if (write(dest_fd, write_buf, BUF_SIZE) != BUF_SIZE) {
            fprintf(stderr, "Error writing to \"%s\" file\n", argv[2]);
            return 1;
          }
          bytes_to_write = 0;
        }
      }
    }
    if (write(dest_fd, write_buf, bytes_to_write) != bytes_to_write) {
      fprintf(stderr, "Error writing to \"%s\" file\n", argv[2]);
      return 1;
    } else {
      bytes_to_write = 0;
    }
  }

  if (bytes == -1) {
    fprintf(stderr, "Error reading \"%s\" file\n", argv[1]);
    return 1;
  }

  fprintf(stdout, "File copied successfully\n");
  return 0;
}
