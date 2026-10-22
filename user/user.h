#define SBRK_ERROR ((char *)-1)

struct stat;

// system calls
цяло fork(void);
цяло exit(цяло) __attribute__((noreturn));
цяло wait(цяло *);
цяло pipe(цяло *);
цяло write(цяло, const void *, цяло);
цяло read(цяло, void *, цяло);
цяло close(цяло);
цяло kill(цяло);
цяло exec(const char *, char **);
цяло open(const char *, цяло);
цяло mknod(const char *, short, short);
цяло unlink(const char *);
цяло fstat(цяло fd, struct stat *);
цяло link(const char *, const char *);
цяло mkdir(const char *);
цяло chdir(const char *);
цяло dup(цяло);
цяло getpid(void);
char *sys_sbrk(цяло, цяло);
цяло pause(цяло);
цяло uptime(void);
цяло sync(void);

// ulib.c
цяло stat(const char *, struct stat *);
char *strcpy(char *, const char *);
void *memmove(void *, const void *, цяло);
char *strchr(const char *, char c);
цяло strcmp(const char *, const char *);
char *gets(char *, цяло max);
uint strlen(const char *);
void *memset(void *, цяло, uint);
цяло atoi(const char *);
цяло memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
char *sbrk(цяло);
char *sbrklazy(цяло);

// printf.c
void fprintf(цяло, const char *, ...) __attribute__((format(printf, 2, 3)));
void printf(const char *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
