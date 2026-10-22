#define SBRK_ERROR ((var_na *)-1)

sa_nracanaa stat;

// system calls
පූර්ණ_සංඛ්‍යා fork(void);
පූර්ණ_සංඛ්‍යා exit(පූර්ණ_සංඛ්‍යා) __attribute__((noreturn));
පූර්ණ_සංඛ්‍යා wait(පූර්ණ_සංඛ්‍යා *);
පූර්ණ_සංඛ්‍යා pipe(පූර්ණ_සංඛ්‍යා *);
පූර්ණ_සංඛ්‍යා write(පූර්ණ_සංඛ්‍යා, const void *, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා read(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා close(පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා kill(පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා exec(const var_na *, var_na **);
පූර්ණ_සංඛ්‍යා open(const var_na *, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා mknod(const var_na *, short, short);
පූර්ණ_සංඛ්‍යා unlink(const var_na *);
පූර්ණ_සංඛ්‍යා fstat(පූර්ණ_සංඛ්‍යා fd, sa_nracanaa stat *);
පූර්ණ_සංඛ්‍යා link(const var_na *, const var_na *);
පූර්ණ_සංඛ්‍යා mkdir(const var_na *);
පූර්ණ_සංඛ්‍යා chdir(const var_na *);
පූර්ණ_සංඛ්‍යා dup(පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා getpid(void);
var_na *sys_sbrk(පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා pause(පූර්ණ_සංඛ්‍යා);
පූර්ණ_සංඛ්‍යා uptime(void);
පූර්ණ_සංඛ්‍යා sync(void);

// ulib.c
පූර්ණ_සංඛ්‍යා stat(const var_na *, sa_nracanaa stat *);
var_na *strcpy(var_na *, const var_na *);
void *memmove(void *, const void *, පූර්ණ_සංඛ්‍යා);
var_na *strchr(const var_na *, var_na c);
පූර්ණ_සංඛ්‍යා strcmp(const var_na *, const var_na *);
var_na *gets(var_na *, පූර්ණ_සංඛ්‍යා max);
uint strlen(const var_na *);
void *memset(void *, පූර්ණ_සංඛ්‍යා, uint);
පූර්ණ_සංඛ්‍යා atoi(const var_na *);
පූර්ණ_සංඛ්‍යා memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
var_na *sbrk(පූර්ණ_සංඛ්‍යා);
var_na *sbrklazy(පූර්ණ_සංඛ්‍යා);

// printf.c
void fprintf(පූර්ණ_සංඛ්‍යා, const var_na *, ...) __attribute__((format(printf, 2, 3)));
void printf(const var_na *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
