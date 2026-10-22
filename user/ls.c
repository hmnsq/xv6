#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"
#অন্তর্ভাব "kernel/fs.h"
#অন্তর্ভাব "kernel/fcntl.h"

বর্ণ *
fmtname(বর্ণ *path)
{
  স্থির বর্ণ buf[DIRSIZ + 1];
  বর্ণ *p;

  // Find first character after last slash.
  প্রতি (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  যদি (strlen(p) >= DIRSIZ)
    প্রত্যাগম p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[পরিমাণ(buf) - 1] = '\0';
  প্রত্যাগম buf;
}

শূন্য
ls(বর্ণ *path)
{
  বর্ণ buf[512], *p;
  পূর্ণাঙ্ক fd;
  সংরচনা dirent de;
  সংরচনা stat st;

  যদি ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    প্রত্যাগম;
  }

  যদি (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    প্রত্যাগম;
  }

  বিকল্প (st.type) {
  পক্ষ T_DEVICE:
  পক্ষ T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (পূর্ণাঙ্ক)st.size);
    ভঙ্গ;

  পক্ষ T_DIR:
    যদি (strlen(path) + 1 + DIRSIZ + 1 > পরিমাণ buf) {
      printf("ls: path too long\n");
      ভঙ্গ;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    যাবত্ (পঠ(fd, &de, পরিমাণ(de)) == পরিমাণ(de)) {
      যদি (de.inum == 0)
        অনুবর্ত;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      যদি (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        অনুবর্ত;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (পূর্ণাঙ্ক)st.size);
    }
    ভঙ্গ;
  }
  close(fd);
}

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, বর্ণ *argv[])
{
  পূর্ণাঙ্ক i;

  যদি (argc < 2) {
    ls(".");
    exit(0);
  }
  প্রতি (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
