#define SBRK_ERROR ((var_na *)-1)

sa_nracanaa stat;

// system calls
ཧྲིལ fork(void);
ཧྲིལ exit(ཧྲིལ) __attribute__((noreturn));
ཧྲིལ wait(ཧྲིལ *);
ཧྲིལ pipe(ཧྲིལ *);
ཧྲིལ write(ཧྲིལ, const void *, ཧྲིལ);
ཧྲིལ read(ཧྲིལ, void *, ཧྲིལ);
ཧྲིལ close(ཧྲིལ);
ཧྲིལ kill(ཧྲིལ);
ཧྲིལ exec(const var_na *, var_na **);
ཧྲིལ open(const var_na *, ཧྲིལ);
ཧྲིལ mknod(const var_na *, short, short);
ཧྲིལ unlink(const var_na *);
ཧྲིལ fstat(ཧྲིལ fd, sa_nracanaa stat *);
ཧྲིལ link(const var_na *, const var_na *);
ཧྲིལ mkdir(const var_na *);
ཧྲིལ chdir(const var_na *);
ཧྲིལ dup(ཧྲིལ);
ཧྲིལ getpid(void);
var_na *sys_sbrk(ཧྲིལ, ཧྲིལ);
ཧྲིལ pause(ཧྲིལ);
ཧྲིལ uptime(void);
ཧྲིལ sync(void);

// ulib.c
ཧྲིལ stat(const var_na *, sa_nracanaa stat *);
var_na *strcpy(var_na *, const var_na *);
void *memmove(void *, const void *, ཧྲིལ);
var_na *strchr(const var_na *, var_na c);
ཧྲིལ strcmp(const var_na *, const var_na *);
var_na *gets(var_na *, ཧྲིལ max);
uint strlen(const var_na *);
void *memset(void *, ཧྲིལ, uint);
ཧྲིལ atoi(const var_na *);
ཧྲིལ memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
var_na *sbrk(ཧྲིལ);
var_na *sbrklazy(ཧྲིལ);

// printf.c
void fprintf(ཧྲིལ, const var_na *, ...) __attribute__((format(printf, 2, 3)));
void printf(const var_na *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
