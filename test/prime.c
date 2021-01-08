extern int printf(const char* restrict format, ...);

int jitmain(int argc, char** argv) {
  if (argc != 3) return 1;

  const char* itr = argv[2];
  int x = 0;
  while (*itr) {
    if (*itr < '0' || *itr > '9') return 1;
    x *= 10;
    x += *itr - '0';
    ++itr;
  }

  int y = x;
  while (--y) if (x%y == 0) break;

  if (y) {
    printf("%d is not prime number\n", x);
  } else {
    printf("%d is prime number\n", x);
  }
  return 0;
}
