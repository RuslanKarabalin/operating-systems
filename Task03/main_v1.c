#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct cgetcwd_r {
  int error = 0;
  string path = "";
  int count_of_symlink = 0;
};

cgetcwd_r cgetcwd() {
  // variable for result
  cgetcwd_r result;

  // stat of current working directory
  struct stat cwd_stat;
  if (stat(".", &cwd_stat) == -1) {
    result.error = -1;
    return result;
  }

  // stat of root directory
  struct stat rd_stat;
  if (stat("/", &rd_stat) == -1) {
    result.error = -1;
    return result;
  }

  // stat of checking directory
  struct stat cd_stat;
  // stat of previous checking direcotry
  struct stat pcd_stat;
  // dirstream of checking directory
  DIR *cd_dir;
  do {
    cd_dir = opendir(".");
    if (cd_dir == NULL) {
      result.error = -1;
      return result;
    }
    if (stat(".", &cd_stat) == -1) {
      result.error = -1;
      return result;
    }
    errno = 0;
    struct dirent *ecd_dirent;
    while ((ecd_dirent = readdir(cd_dir)) != NULL) {
      // checking for symbolick link
      struct stat ecd_stat;
      if (lstat(ecd_dirent->d_name, &ecd_stat) == -1) {
        result.error = -1;
        return result;
      }
      if (S_ISLNK(ecd_stat.st_mode)) {
        if (strchr(ecd_dirent->d_name, 'z') != NULL ||
            strchr(ecd_dirent->d_name, 'Z') != NULL) {
          if (stat(ecd_dirent->d_name, &ecd_stat) == -1) {
            result.error = -1;
            return result;
          }
          if (ecd_stat.st_dev == cwd_stat.st_dev &&
              ecd_stat.st_ino == cwd_stat.st_ino) {
            ++result.count_of_symlink;
          }
        }
      } else {
        // checking for path
        if (strcmp(ecd_dirent->d_name, ".") != 0 &&
            strcmp(ecd_dirent->d_name, "..") != 0) {
          struct stat path_stat;
          if (stat(ecd_dirent->d_name, &path_stat) == -1) {
            result.error = -1;
            return result;
          }
          if (path_stat.st_dev == pcd_stat.st_dev &&
              path_stat.st_ino == pcd_stat.st_ino) {
            result.path.insert(0, ecd_dirent->d_name);
            result.path.insert(0, "/");
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
  printf("Error: %s\n", r.error);
  printf("Path: %s\n", r.path);
  printf("Count of symbolic links: %s\n", r.count_of_symlink);
  return 0;
}