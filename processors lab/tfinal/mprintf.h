double matod(char *str);
int dprint(double x, int campo, int frac, void (*putc)(int));
int mprintf(void (*putc)(int), const char *formato, ... );
int msprintf(char *buf, const char *formato, ... );
int mscanf(int (*getc)(void), const char *formato, ...);
int msscanf(char *buf, const char *formato, ... );
