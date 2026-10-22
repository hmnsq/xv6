#define SBRK_ERROR ((char *)-1)

struct stat;

// system calls
정수 fork(void);
정수 exit(정수) __attribute__((noreturn));
정수 wait(정수 *);
정수 pipe(정수 *);
정수 write(정수, const void *, 정수);
정수 read(정수, void *, 정수);
정수 close(정수);
정수 kill(정수);
정수 exec(const char *, char **);
정수 open(const char *, 정수);
정수 mknod(const char *, short, short);
정수 unlink(const char *);
정수 fstat(정수 fd, struct stat *);
정수 link(const char *, const char *);
정수 mkdir(const char *);
정수 chdir(const char *);
정수 dup(정수);
정수 getpid(void);
char *sys_sbrk(정수, 정수);
정수 pause(정수);
정수 uptime(void);
정수 sync(void);

// ulib.c
정수 stat(const char *, struct stat *);
char *strcpy(char *, const char *);
void *memmove(void *, const void *, 정수);
char *strchr(const char *, char c);
정수 strcmp(const char *, const char *);
char *gets(char *, 정수 max);
uint strlen(const char *);
void *memset(void *, 정수, uint);
정수 atoi(const char *);
정수 memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
char *sbrk(정수);
char *sbrklazy(정수);

// printf.c
void fprintf(정수, const char *, ...) __attribute__((format(printf, 2, 3)));
void printf(const char *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
