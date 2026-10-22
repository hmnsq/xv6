#define SBRK_ERROR ((char *)-1)

struct stat;

// system calls
целое fork(void);
целое exit(целое) __attribute__((noreturn));
целое wait(целое *);
целое pipe(целое *);
целое write(целое, const void *, целое);
целое read(целое, void *, целое);
целое close(целое);
целое kill(целое);
целое exec(const char *, char **);
целое open(const char *, целое);
целое mknod(const char *, short, short);
целое unlink(const char *);
целое fstat(целое fd, struct stat *);
целое link(const char *, const char *);
целое mkdir(const char *);
целое chdir(const char *);
целое dup(целое);
целое getpid(void);
char *sys_sbrk(целое, целое);
целое pause(целое);
целое uptime(void);
целое sync(void);

// ulib.c
целое stat(const char *, struct stat *);
char *strcpy(char *, const char *);
void *memmove(void *, const void *, целое);
char *strchr(const char *, char c);
целое strcmp(const char *, const char *);
char *gets(char *, целое max);
uint strlen(const char *);
void *memset(void *, целое, uint);
целое atoi(const char *);
целое memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
char *sbrk(целое);
char *sbrklazy(целое);

// printf.c
void fprintf(целое, const char *, ...) __attribute__((format(printf, 2, 3)));
void printf(const char *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
