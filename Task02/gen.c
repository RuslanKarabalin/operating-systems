#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

void generateRandomNumbersFile(const char *fileName, size_t fileSizeBytes) {
  FILE *file = fopen(fileName, "wb");
  if (file == NULL) {
    perror("fopen");
    return;
  }

  size_t bytesWritten = 0;
  srand(time(NULL));

  while (bytesWritten < fileSizeBytes) {
    uint64_t randomNumber = ((uint64_t)rand() << 32) | rand();
    randomNumber = randomNumber % 1000000000000ULL;
    char byteString[32];
    snprintf(byteString, sizeof(byteString), "%lu", (unsigned long)randomNumber);
    size_t len = strlen(byteString);
    size_t bytesToWrite = len < (fileSizeBytes - bytesWritten) ? len : (fileSizeBytes - bytesWritten);
    fwrite(byteString, 1, bytesToWrite, file);
    bytesWritten += bytesToWrite;
  }

  fclose(file);
}

int main() {
  size_t targetSizeBytes = 256 * 1024 * 1024;

  const char *fileName = "file0";
  size_t fileSize = targetSizeBytes;
  generateRandomNumbersFile(fileName, fileSize);

  return 0;
}
