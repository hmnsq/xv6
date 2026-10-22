#define SBRK_ERROR ((char *)-1)

struct stat;

// system calls
цео fork(void);
цео exit(цео) __attribute__((noreturn));
цео wait(цео *);
цео pipe(цео *);
цео write(цео, const void *, цео);
цео read(цео, void *, цео);
цео close(цео);
цео kill(цео);
цео exec(const char *, char **);
цео open(const char *, цео);
цео mknod(const char *, short, short);
цео unlink(const char *);
цео fstat(цео fd, struct stat *);
цео link(const char *, const char *);
цео mkdir(const char *);
цео chdir(const char *);
цео dup(цео);
цео getpid(void);
char *sys_sbrk(цео, цео);
цео pause(цео);
цео uptime(void);
цео sync(void);

// ulib.c
цео stat(const char *, struct stat *);
char *strcpy(char *, const char *);
void *memmove(void *, const void *, цео);
char *strchr(const char *, char c);
цео strcmp(const char *, const char *);
char *gets(char *, цео max);
uint strlen(const char *);
void *memset(void *, цео, uint);
цео atoi(const char *);
цео memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
char *sbrk(цео);
char *sbrklazy(цео);

// printf.c
void fprintf(цео, const char *, ...) __attribute__((format(printf, 2, 3)));
void printf(const char *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
