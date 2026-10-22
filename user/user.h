#define SBRK_ERROR ((حرف *)-1)

بنية stat;

// system calls
صحيح fork(فراغ);
صحيح exit(صحيح) __attribute__((noreturn));
صحيح wait(صحيح *);
صحيح pipe(صحيح *);
صحيح اكتب(صحيح, ثابت فراغ *, صحيح);
صحيح اقرأ(صحيح, فراغ *, صحيح);
صحيح close(صحيح);
صحيح kill(صحيح);
صحيح exec(ثابت حرف *, حرف **);
صحيح open(ثابت حرف *, صحيح);
صحيح mknod(ثابت حرف *, short, short);
صحيح unlink(ثابت حرف *);
صحيح fstat(صحيح fd, بنية stat *);
صحيح link(ثابت حرف *, ثابت حرف *);
صحيح mkdir(ثابت حرف *);
صحيح chdir(ثابت حرف *);
صحيح dup(صحيح);
صحيح getpid(فراغ);
حرف *sys_sbrk(صحيح, صحيح);
صحيح pause(صحيح);
صحيح uptime(فراغ);
صحيح sync(فراغ);

// ulib.c
صحيح stat(ثابت حرف *, بنية stat *);
حرف *strcpy(حرف *, ثابت حرف *);
فراغ *memmove(فراغ *, ثابت فراغ *, صحيح);
حرف *strchr(ثابت حرف *, حرف c);
صحيح strcmp(ثابت حرف *, ثابت حرف *);
حرف *gets(حرف *, صحيح max);
uint strlen(ثابت حرف *);
فراغ *memset(فراغ *, صحيح, uint);
صحيح atoi(ثابت حرف *);
صحيح memcmp(ثابت فراغ *, ثابت فراغ *, uint);
فراغ *memcpy(فراغ *, ثابت فراغ *, uint);
حرف *sbrk(صحيح);
حرف *sbrklazy(صحيح);

// printf.c
فراغ fprintf(صحيح, ثابت حرف *, ...) __attribute__((format(printf, 2, 3)));
فراغ printf(ثابت حرف *, ...) __attribute__((format(printf, 1, 2)));

// umalloc.c
فراغ *malloc(uint);
فراغ حرر(فراغ *);
