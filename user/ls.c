#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"
#شامل "kernel/fs.h"
#شامل "kernel/fcntl.h"

نویسه *
fmtname(نویسه *path)
{
  ایستا نویسه buf[DIRSIZ + 1];
  نویسه *p;

  // Find first character after last slash.
  برای (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  اگر (strlen(p) >= DIRSIZ)
    بازگشت p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[اندازه(buf) - 1] = '\0';
  بازگشت buf;
}

تهی
ls(نویسه *path)
{
  نویسه buf[512], *p;
  صحیح fd;
  ساختار dirent de;
  ساختار stat st;

  اگر ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    بازگشت;
  }

  اگر (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    بازگشت;
  }

  انتخاب (st.type) {
  حالت T_DEVICE:
  حالت T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (صحیح)st.size);
    بشکن;

  حالت T_DIR:
    اگر (strlen(path) + 1 + DIRSIZ + 1 > اندازه buf) {
      printf("ls: path too long\n");
      بشکن;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    وقتی (اقرأ(fd, &de, اندازه(de)) == اندازه(de)) {
      اگر (de.inum == 0)
        ادامه;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      اگر (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        ادامه;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (صحیح)st.size);
    }
    بشکن;
  }
  close(fd);
}

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  صحیح i;

  اگر (argc < 2) {
    ls(".");
    exit(0);
  }
  برای (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
