#define SBRK_ERROR ((نویسه *)-1)

ساختار stat;

// system calls
صحیح fork(تهی);
صحیح exit(صحیح) __attribute__((noreturn));
صحیح wait(صحیح *);
صحیح pipe(صحیح *);
صحیح اكتب(صحیح, ثابت تهی *, صحیح);
صحیح اقرأ(صحیح, تهی *, صحیح);
صحیح close(صحیح);
صحیح kill(صحیح);
صحیح exec(ثابت نویسه *, نویسه **);
صحیح open(ثابت نویسه *, صحیح);
صحیح mknod(ثابت نویسه *, short, short);
صحیح unlink(ثابت نویسه *);
صحیح fstat(صحیح fd, ساختار stat *);
صحیح link(ثابت نویسه *, ثابت نویسه *);
صحیح mkdir(ثابت نویسه *);
صحیح chdir(ثابت نویسه *);
صحیح dup(صحیح);
صحیح getpid(تهی);
نویسه *sys_sbrk(صحیح, صحیح);
صحیح pause(صحیح);
صحیح uptime(تهی);
صحیح sync(تهی);

// ulib.c
صحیح stat(ثابت نویسه *, ساختار stat *);
نویسه *strcpy(نویسه *, ثابت نویسه *);
تهی *memmove(تهی *, ثابت تهی *, صحیح);
نویسه *strchr(ثابت نویسه *, نویسه c);
صحیح strcmp(ثابت نویسه *, ثابت نویسه *);
نویسه *gets(نویسه *, صحیح max);
uint strlen(ثابت نویسه *);
تهی *memset(تهی *, صحیح, uint);
صحیح atoi(ثابت نویسه *);
صحیح memcmp(ثابت تهی *, ثابت تهی *, uint);
تهی *memcpy(تهی *, ثابت تهی *, uint);
نویسه *sbrk(صحیح);
نویسه *sbrklazy(صحیح);

// printf.c
تهی fprintf(صحیح, ثابت نویسه *, ...) __attribute__((format(printf, 2, 3)));
تهی printf(ثابت نویسه *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
تهی *malloc(uint);
تهی آزاد(تهی *);
