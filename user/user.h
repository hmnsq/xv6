#define SBRK_ERROR ((char *)-1)

struct stat;

// system calls
ціле fork(void);
ціле exit(ціле) __attribute__((noreturn));
ціле wait(ціле *);
ціле pipe(ціле *);
ціле write(ціле, const void *, ціле);
ціле read(ціле, void *, ціле);
ціле close(ціле);
ціле kill(ціле);
ціле exec(const char *, char **);
ціле open(const char *, ціле);
ціле mknod(const char *, short, short);
ціле unlink(const char *);
ціле fstat(ціле fd, struct stat *);
ціле link(const char *, const char *);
ціле mkdir(const char *);
ціле chdir(const char *);
ціле dup(ціле);
ціле getpid(void);
char *sys_sbrk(ціле, ціле);
ціле pause(ціле);
ціле uptime(void);
ціле sync(void);

// ulib.c
ціле stat(const char *, struct stat *);
char *strcpy(char *, const char *);
void *memmove(void *, const void *, ціле);
char *strchr(const char *, char c);
ціле strcmp(const char *, const char *);
char *gets(char *, ціле max);
uint strlen(const char *);
void *memset(void *, ціле, uint);
ціле atoi(const char *);
ціле memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
char *sbrk(ціле);
char *sbrklazy(ціле);

// printf.c
void fprintf(ціле, const char *, ...) __attribute__((format(printf, 2, 3)));
void printf(const char *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
