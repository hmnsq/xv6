#define SBRK_ERROR ((char *)-1)

struct stat;

// system calls
số_nguyên fork(void);
số_nguyên exit(số_nguyên) __attribute__((noreturn));
số_nguyên wait(số_nguyên *);
số_nguyên pipe(số_nguyên *);
số_nguyên write(số_nguyên, const void *, số_nguyên);
số_nguyên read(số_nguyên, void *, số_nguyên);
số_nguyên close(số_nguyên);
số_nguyên kill(số_nguyên);
số_nguyên exec(const char *, char **);
số_nguyên open(const char *, số_nguyên);
số_nguyên mknod(const char *, short, short);
số_nguyên unlink(const char *);
số_nguyên fstat(số_nguyên fd, struct stat *);
số_nguyên link(const char *, const char *);
số_nguyên mkdir(const char *);
số_nguyên chdir(const char *);
số_nguyên dup(số_nguyên);
số_nguyên getpid(void);
char *sys_sbrk(số_nguyên, số_nguyên);
số_nguyên pause(số_nguyên);
số_nguyên uptime(void);
số_nguyên sync(void);

// ulib.c
số_nguyên stat(const char *, struct stat *);
char *strcpy(char *, const char *);
void *memmove(void *, const void *, số_nguyên);
char *strchr(const char *, char c);
số_nguyên strcmp(const char *, const char *);
char *gets(char *, số_nguyên max);
uint strlen(const char *);
void *memset(void *, số_nguyên, uint);
số_nguyên atoi(const char *);
số_nguyên memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
char *sbrk(số_nguyên);
char *sbrklazy(số_nguyên);

// printf.c
void fprintf(số_nguyên, const char *, ...) __attribute__((format(printf, 2, 3)));
void printf(const char *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
