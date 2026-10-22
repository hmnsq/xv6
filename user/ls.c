#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/fcntl.h"

حرف *
fmtname(حرف *path)
{
  ساكن حرف buf[DIRSIZ + 1];
  حرف *p;

  // Find first character after last slash.
  لكل (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  إذا (strlen(p) >= DIRSIZ)
    إرجاع p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[حجم(buf) - 1] = '\0';
  إرجاع buf;
}

فراغ
ls(حرف *path)
{
  حرف buf[512], *p;
  صحيح fd;
  بنية dirent de;
  بنية stat st;

  إذا ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    إرجاع;
  }

  إذا (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    إرجاع;
  }

  بدل (st.type) {
  حالة T_DEVICE:
  حالة T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (صحيح)st.size);
    اكسر;

  حالة T_DIR:
    إذا (strlen(path) + 1 + DIRSIZ + 1 > حجم buf) {
      printf("ls: path too long\n");
      اكسر;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    بينما (اقرأ(fd, &de, حجم(de)) == حجم(de)) {
      إذا (de.inum == 0)
        استمر;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      إذا (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        استمر;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (صحيح)st.size);
    }
    اكسر;
  }
  close(fd);
}

صحيح
الرئيسية(صحيح argc, حرف *argv[])
{
  صحيح i;

  إذا (argc < 2) {
    ls(".");
    exit(0);
  }
  لكل (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
