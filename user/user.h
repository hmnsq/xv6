#define SBRK_ERROR ((char *)-1)

struct stat;

// system calls
整数 fork(void);
整数 exit(整数) __attribute__((noreturn));
整数 wait(整数 *);
整数 pipe(整数 *);
整数 write(整数, const void *, 整数);
整数 read(整数, void *, 整数);
整数 close(整数);
整数 kill(整数);
整数 exec(const char *, char **);
整数 open(const char *, 整数);
整数 mknod(const char *, short, short);
整数 unlink(const char *);
整数 fstat(整数 fd, struct stat *);
整数 link(const char *, const char *);
整数 mkdir(const char *);
整数 chdir(const char *);
整数 dup(整数);
整数 getpid(void);
char *sys_sbrk(整数, 整数);
整数 pause(整数);
整数 uptime(void);
整数 sync(void);

// ulib.c
整数 stat(const char *, struct stat *);
char *strcpy(char *, const char *);
void *memmove(void *, const void *, 整数);
char *strchr(const char *, char c);
整数 strcmp(const char *, const char *);
char *gets(char *, 整数 max);
uint strlen(const char *);
void *memset(void *, 整数, uint);
整数 atoi(const char *);
整数 memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
char *sbrk(整数);
char *sbrklazy(整数);

// printf.c
void fprintf(整数, const char *, ...) __attribute__((format(printf, 2, 3)));
void printf(const char *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
