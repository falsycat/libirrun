extern int printf(const char* restrict format, ...);

int jitmain(int argc, char** argv) {
  if (argc != 3) return 1;

  int len = 0;
  while (argv[2][len]) ++len;

  printf("%d\n", len);
  return 0;
}
