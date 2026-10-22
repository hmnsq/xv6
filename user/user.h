#define SBRK_ERROR ((var_na *)-1)

sa_nracanaa stat;

// system calls
puur_naa_nka fork(void);
puur_naa_nka exit(puur_naa_nka) __attribute__((noreturn));
puur_naa_nka wait(puur_naa_nka *);
puur_naa_nka pipe(puur_naa_nka *);
puur_naa_nka write(puur_naa_nka, const void *, puur_naa_nka);
puur_naa_nka read(puur_naa_nka, void *, puur_naa_nka);
puur_naa_nka close(puur_naa_nka);
puur_naa_nka kill(puur_naa_nka);
puur_naa_nka exec(const var_na *, var_na **);
puur_naa_nka open(const var_na *, puur_naa_nka);
puur_naa_nka mknod(const var_na *, short, short);
puur_naa_nka unlink(const var_na *);
puur_naa_nka fstat(puur_naa_nka fd, sa_nracanaa stat *);
puur_naa_nka link(const var_na *, const var_na *);
puur_naa_nka mkdir(const var_na *);
puur_naa_nka chdir(const var_na *);
puur_naa_nka dup(puur_naa_nka);
puur_naa_nka getpid(void);
var_na *sys_sbrk(puur_naa_nka, puur_naa_nka);
puur_naa_nka pause(puur_naa_nka);
puur_naa_nka uptime(void);
puur_naa_nka sync(void);

// ulib.c
puur_naa_nka stat(const var_na *, sa_nracanaa stat *);
var_na *strcpy(var_na *, const var_na *);
void *memmove(void *, const void *, puur_naa_nka);
var_na *strchr(const var_na *, var_na c);
puur_naa_nka strcmp(const var_na *, const var_na *);
var_na *gets(var_na *, puur_naa_nka max);
uint strlen(const var_na *);
void *memset(void *, puur_naa_nka, uint);
puur_naa_nka atoi(const var_na *);
puur_naa_nka memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
var_na *sbrk(puur_naa_nka);
var_na *sbrklazy(puur_naa_nka);

// printf.c
void fprintf(puur_naa_nka, const var_na *, ...) __attribute__((format(printf, 2, 3)));
void printf(const var_na *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
