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
  𐤀𐤌 (strlen(p) >= DIRSIZ)
    أرجع p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[حجم(buf) - 1] = '\0';
  أرجع buf;
}

فراغ
ls(حرف *path)
{
  حرف buf[512], *p;
  صحيح fd;
  بنية dirent de;
  بنية stat st;

  𐤀𐤌 ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    أرجع;
  }

  𐤀𐤌 (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    أرجع;
  }

  بدل (st.type) {
  حالة T_DEVICE:
  حالة T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (صحيح)st.size);
    اكسر;

  حالة T_DIR:
    𐤀𐤌 (strlen(path) + 1 + DIRSIZ + 1 > حجم buf) {
      printf("ls: path too long\n");
      اكسر;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    طالما (اقرأ(fd, &de, حجم(de)) == حجم(de)) {
      𐤀𐤌 (de.inum == 0)
        استمر;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      𐤀𐤌 (stat(buf, &st) < 0) {
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
main(صحيح argc, حرف *argv[])
{
  صحيح i;

  𐤀𐤌 (argc < 2) {
    ls(".");
    exit(0);
  }
  لكل (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
