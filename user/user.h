#define SBRK_ERROR ((var_na *)-1)

sa_nracanaa stat;

// system calls
ကိန်းပြည့် fork(void);
ကိန်းပြည့် exit(ကိန်းပြည့်) __attribute__((noreturn));
ကိန်းပြည့် wait(ကိန်းပြည့် *);
ကိန်းပြည့် pipe(ကိန်းပြည့် *);
ကိန်းပြည့် write(ကိန်းပြည့်, const void *, ကိန်းပြည့်);
ကိန်းပြည့် read(ကိန်းပြည့်, void *, ကိန်းပြည့်);
ကိန်းပြည့် close(ကိန်းပြည့်);
ကိန်းပြည့် kill(ကိန်းပြည့်);
ကိန်းပြည့် exec(const var_na *, var_na **);
ကိန်းပြည့် open(const var_na *, ကိန်းပြည့်);
ကိန်းပြည့် mknod(const var_na *, short, short);
ကိန်းပြည့် unlink(const var_na *);
ကိန်းပြည့် fstat(ကိန်းပြည့် fd, sa_nracanaa stat *);
ကိန်းပြည့် link(const var_na *, const var_na *);
ကိန်းပြည့် mkdir(const var_na *);
ကိန်းပြည့် chdir(const var_na *);
ကိန်းပြည့် dup(ကိန်းပြည့်);
ကိန်းပြည့် getpid(void);
var_na *sys_sbrk(ကိန်းပြည့်, ကိန်းပြည့်);
ကိန်းပြည့် pause(ကိန်းပြည့်);
ကိန်းပြည့် uptime(void);
ကိန်းပြည့် sync(void);

// ulib.c
ကိန်းပြည့် stat(const var_na *, sa_nracanaa stat *);
var_na *strcpy(var_na *, const var_na *);
void *memmove(void *, const void *, ကိန်းပြည့်);
var_na *strchr(const var_na *, var_na c);
ကိန်းပြည့် strcmp(const var_na *, const var_na *);
var_na *gets(var_na *, ကိန်းပြည့် max);
uint strlen(const var_na *);
void *memset(void *, ကိန်းပြည့်, uint);
ကိန်းပြည့် atoi(const var_na *);
ကိန်းပြည့် memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
var_na *sbrk(ကိန်းပြည့်);
var_na *sbrklazy(ကိန်းပြည့်);

// printf.c
void fprintf(ကိန်းပြည့်, const var_na *, ...) __attribute__((format(printf, 2, 3)));
void printf(const var_na *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
void *malloc(uint);
void free(void *);
