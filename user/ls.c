#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"
#અન્તર્ભાવ "kernel/fs.h"
#અન્તર્ભાવ "kernel/fcntl.h"

અક્ષર *
fmtname(અક્ષર *path)
{
  સ્થિર અક્ષર buf[DIRSIZ + 1];
  અક્ષર *p;

  // Find first character after last slash.
  માટે (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  જો (strlen(p) >= DIRSIZ)
    પાછા p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[કદ(buf) - 1] = '\0';
  પાછા buf;
}

શૂન્ય
ls(અક્ષર *path)
{
  અક્ષર buf[512], *p;
  પૂર્ણાંક fd;
  સંરચના dirent de;
  સંરચના stat st;

  જો ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    પાછા;
  }

  જો (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    પાછા;
  }

  પસંદગી (st.type) {
  સ્થિતિ T_DEVICE:
  સ્થિતિ T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (પૂર્ણાંક)st.size);
    તોડો;

  સ્થિતિ T_DIR:
    જો (strlen(path) + 1 + DIRSIZ + 1 > કદ buf) {
      printf("ls: path too long\n");
      તોડો;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    જ્યારે (પઠ(fd, &de, કદ(de)) == કદ(de)) {
      જો (de.inum == 0)
        ચાલુ;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      જો (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        ચાલુ;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (પૂર્ણાંક)st.size);
    }
    તોડો;
  }
  close(fd);
}

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
  પૂર્ણાંક i;

  જો (argc < 2) {
    ls(".");
    exit(0);
  }
  માટે (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
