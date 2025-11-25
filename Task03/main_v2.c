#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_PATH 4096

typedef struct {
  int error;
  char path[MAX_PATH];
  int count_of_symlink;
} cgetcwd_r;

void print_cgetcwd_r(cgetcwd_r r) {
  printf("Error: %d\n", r.error);
  printf("Path: %s\n", r.path);
  printf("Count of symbolic links: %d\n", r.count_of_symlink);
}

void path_insert(char *path, const char *str) {
  size_t str_len = strlen(str);
  size_t path_len = strlen(path);
  if (str_len + path_len >= MAX_PATH) {
    return;
  }
  memmove(path + str_len, path, path_len + 1);
  memcpy(path, str, str_len);
}

cgetcwd_r cgetcwd() {
  cgetcwd_r result = {0};
  result.path[0] = '\0';

  struct stat cwd_stat;
  if (stat(".", &cwd_stat) == -1) {
    result.error = -1;
    return result;
  }

  struct stat rd_stat;
  if (stat("/", &rd_stat) == -1) {
    result.error = -1;
    return result;
  }

  struct stat cd_stat;
  struct stat pcd_stat = {0};
  DIR *cd_dir;

  do {
    cd_dir = opendir(".");
    if (cd_dir == NULL) {
      result.error = -1;
      return result;
    }
    if (stat(".", &cd_stat) == -1) {
      result.error = -1;
      closedir(cd_dir);
      return result;
    }
    errno = 0;
    struct dirent *ecd_dirent;
    while ((ecd_dirent = readdir(cd_dir)) != NULL) {
      struct stat ecd_stat;
      if (lstat(ecd_dirent->d_name, &ecd_stat) == -1) {
        result.error = -1;
        closedir(cd_dir);
        return result;
      }
      if (!S_ISLNK(ecd_stat.st_mode)) {
        if (strcmp(ecd_dirent->d_name, ".") != 0 &&
            strcmp(ecd_dirent->d_name, "..") != 0) {
          struct stat path_stat;
          if (stat(ecd_dirent->d_name, &path_stat) == -1) {
            result.error = -1;
            closedir(cd_dir);
            return result;
          }
          if (path_stat.st_dev == pcd_stat.st_dev &&
              path_stat.st_ino == pcd_stat.st_ino) {
            path_insert(result.path, ecd_dirent->d_name);
            path_insert(result.path, "/");
          }
        }
      }
    }
    pcd_stat = cd_stat;
    if (closedir(cd_dir) == -1) {
      result.error = -1;
      return result;
    }
    if (errno != 0) {
      result.error = -1;
      return result;
    }
    if (chdir("..") == -1) {
      result.error = -1;
      return result;
    }
  } while (cd_stat.st_dev != rd_stat.st_dev ||
           cd_stat.st_ino != rd_stat.st_ino);
  if (errno != 0) {
    result.error = -1;
    return result;
  }
  return result;
}

int main() {
  cgetcwd_r r = cgetcwd();
  print_cgetcwd_r(r);
  return 0;
}
