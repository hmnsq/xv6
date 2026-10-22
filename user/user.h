#define SBRK_ERROR ((var_na *)-1)

sa_nracanaa stat;

// system calls
จำนวนเต็ม fork(void);
จำนวนเต็ม exit(จำนวนเต็ม) __attribute__((noreturn));
จำนวนเต็ม wait(จำนวนเต็ม *);
จำนวนเต็ม pipe(จำนวนเต็ม *);
จำนวนเต็ม write(จำนวนเต็ม, const void *, จำนวนเต็ม);
จำนวนเต็ม read(จำนวนเต็ม, void *, จำนวนเต็ม);
จำนวนเต็ม close(จำนวนเต็ม);
จำนวนเต็ม kill(จำนวนเต็ม);
จำนวนเต็ม exec(const var_na *, var_na **);
จำนวนเต็ม open(const var_na *, จำนวนเต็ม);
จำนวนเต็ม mknod(const var_na *, short, short);
จำนวนเต็ม unlink(const var_na *);
จำนวนเต็ม fstat(จำนวนเต็ม fd, sa_nracanaa stat *);
จำนวนเต็ม link(const var_na *, const var_na *);
จำนวนเต็ม mkdir(const var_na *);
จำนวนเต็ม chdir(const var_na *);
จำนวนเต็ม dup(จำนวนเต็ม);
จำนวนเต็ม getpid(void);
var_na *sys_sbrk(จำนวนเต็ม, จำนวนเต็ม);
จำนวนเต็ม pause(จำนวนเต็ม);
จำนวนเต็ม uptime(void);
จำนวนเต็ม sync(void);

// ulib.c
จำนวนเต็ม stat(const var_na *, sa_nracanaa stat *);
var_na *strcpy(var_na *, const var_na *);
void *memmove(void *, const void *, จำนวนเต็ม);
var_na *strchr(const var_na *, var_na c);
จำนวนเต็ม strcmp(const var_na *, const var_na *);
var_na *gets(var_na *, จำนวนเต็ม max);
uint strlen(const var_na *);
void *memset(void *, จำนวนเต็ม, uint);
จำนวนเต็ม atoi(const var_na *);
จำนวนเต็ม memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
var_na *sbrk(จำนวนเต็ม);
var_na *sbrklazy(จำนวนเต็ม);

// printf.c
void fprintf(จำนวนเต็ม, const var_na *, ...) __attribute__((format(printf, 2, 3)));
void printf(const var_na *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
