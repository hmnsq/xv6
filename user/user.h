#define SBRK_ERROR ((var_na *)-1)

sa_nracanaa stat;

// system calls
ຈຳນວນເຕັມ fork(void);
ຈຳນວນເຕັມ exit(ຈຳນວນເຕັມ) __attribute__((noreturn));
ຈຳນວນເຕັມ wait(ຈຳນວນເຕັມ *);
ຈຳນວນເຕັມ pipe(ຈຳນວນເຕັມ *);
ຈຳນວນເຕັມ write(ຈຳນວນເຕັມ, const void *, ຈຳນວນເຕັມ);
ຈຳນວນເຕັມ read(ຈຳນວນເຕັມ, void *, ຈຳນວນເຕັມ);
ຈຳນວນເຕັມ close(ຈຳນວນເຕັມ);
ຈຳນວນເຕັມ kill(ຈຳນວນເຕັມ);
ຈຳນວນເຕັມ exec(const var_na *, var_na **);
ຈຳນວນເຕັມ open(const var_na *, ຈຳນວນເຕັມ);
ຈຳນວນເຕັມ mknod(const var_na *, short, short);
ຈຳນວນເຕັມ unlink(const var_na *);
ຈຳນວນເຕັມ fstat(ຈຳນວນເຕັມ fd, sa_nracanaa stat *);
ຈຳນວນເຕັມ link(const var_na *, const var_na *);
ຈຳນວນເຕັມ mkdir(const var_na *);
ຈຳນວນເຕັມ chdir(const var_na *);
ຈຳນວນເຕັມ dup(ຈຳນວນເຕັມ);
ຈຳນວນເຕັມ getpid(void);
var_na *sys_sbrk(ຈຳນວນເຕັມ, ຈຳນວນເຕັມ);
ຈຳນວນເຕັມ pause(ຈຳນວນເຕັມ);
ຈຳນວນເຕັມ uptime(void);
ຈຳນວນເຕັມ sync(void);

// ulib.c
ຈຳນວນເຕັມ stat(const var_na *, sa_nracanaa stat *);
var_na *strcpy(var_na *, const var_na *);
void *memmove(void *, const void *, ຈຳນວນເຕັມ);
var_na *strchr(const var_na *, var_na c);
ຈຳນວນເຕັມ strcmp(const var_na *, const var_na *);
var_na *gets(var_na *, ຈຳນວນເຕັມ max);
uint strlen(const var_na *);
void *memset(void *, ຈຳນວນເຕັມ, uint);
ຈຳນວນເຕັມ atoi(const var_na *);
ຈຳນວນເຕັມ memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
var_na *sbrk(ຈຳນວນເຕັມ);
var_na *sbrklazy(ຈຳນວນເຕັມ);

// printf.c
void fprintf(ຈຳນວນເຕັມ, const var_na *, ...) __attribute__((format(printf, 2, 3)));
void printf(const var_na *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
