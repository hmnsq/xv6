#define SBRK_ERROR ((var_na *)-1)

sa_nracanaa stat;

// system calls
ចំនួនគត់ fork(void);
ចំនួនគត់ exit(ចំនួនគត់) __attribute__((noreturn));
ចំនួនគត់ wait(ចំនួនគត់ *);
ចំនួនគត់ pipe(ចំនួនគត់ *);
ចំនួនគត់ write(ចំនួនគត់, const void *, ចំនួនគត់);
ចំនួនគត់ read(ចំនួនគត់, void *, ចំនួនគត់);
ចំនួនគត់ close(ចំនួនគត់);
ចំនួនគត់ kill(ចំនួនគត់);
ចំនួនគត់ exec(const var_na *, var_na **);
ចំនួនគត់ open(const var_na *, ចំនួនគត់);
ចំនួនគត់ mknod(const var_na *, short, short);
ចំនួនគត់ unlink(const var_na *);
ចំនួនគត់ fstat(ចំនួនគត់ fd, sa_nracanaa stat *);
ចំនួនគត់ link(const var_na *, const var_na *);
ចំនួនគត់ mkdir(const var_na *);
ចំនួនគត់ chdir(const var_na *);
ចំនួនគត់ dup(ចំនួនគត់);
ចំនួនគត់ getpid(void);
var_na *sys_sbrk(ចំនួនគត់, ចំនួនគត់);
ចំនួនគត់ pause(ចំនួនគត់);
ចំនួនគត់ uptime(void);
ចំនួនគត់ sync(void);

// ulib.c
ចំនួនគត់ stat(const var_na *, sa_nracanaa stat *);
var_na *strcpy(var_na *, const var_na *);
void *memmove(void *, const void *, ចំនួនគត់);
var_na *strchr(const var_na *, var_na c);
ចំនួនគត់ strcmp(const var_na *, const var_na *);
var_na *gets(var_na *, ចំនួនគត់ max);
uint strlen(const var_na *);
void *memset(void *, ចំនួនគត់, uint);
ចំនួនគត់ atoi(const var_na *);
ចំនួនគត់ memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
var_na *sbrk(ចំនួនគត់);
var_na *sbrklazy(ចំនួនគត់);

// printf.c
void fprintf(ចំនួនគត់, const var_na *, ...) __attribute__((format(printf, 2, 3)));
void printf(const var_na *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
