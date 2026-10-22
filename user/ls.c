#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"
#অন্তর্ভাব "kernel/fs.h"
#অন্তর্ভাব "kernel/fcntl.h"

অক্ষর *
fmtname(অক্ষর *path)
{
  স্থির অক্ষর buf[DIRSIZ + 1];
  অক্ষর *p;

  // Find first character after last slash.
  জন্য (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  যদি (strlen(p) >= DIRSIZ)
    ফেরত p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[আকার(buf) - 1] = '\0';
  ফেরত buf;
}

শূন্য
ls(অক্ষর *path)
{
  অক্ষর buf[512], *p;
  পূর্ণসংখ্যা fd;
  কাঠামো dirent de;
  কাঠামো stat st;

  যদি ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    ফেরত;
  }

  যদি (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    ফেরত;
  }

  নির্বাচন (st.type) {
  ক্ষেত্র T_DEVICE:
  ক্ষেত্র T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (পূর্ণসংখ্যা)st.আকার_2);
    বিরতি;

  ক্ষেত্র T_DIR:
    যদি (strlen(path) + 1 + DIRSIZ + 1 > আকার buf) {
      printf("ls: path too long\n");
      বিরতি;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    যখন (পঠ(fd, &de, আকার(de)) == আকার(de)) {
      যদি (de.inum == 0)
        চালু;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      যদি (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        চালু;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (পূর্ণসংখ্যা)st.আকার_2);
    }
    বিরতি;
  }
  close(fd);
}

পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
  পূর্ণসংখ্যা i;

  যদি (argc < 2) {
    ls(".");
    exit(0);
  }
  জন্য (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
