#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/fcntl.h"

حرف *
fmtname(حرف *رستو)
{
  ساكن حرف buf[DIRSIZ + 1];
  حرف *p;

  // Find first character after last slash.
  لكل (p = رستو + لڙي_ڊيگهه(رستو); p >= رستو && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  إذا (لڙي_ڊيگهه(p) >= DIRSIZ)
    أرجع p;
  memmove(buf, p, لڙي_ڊيگهه(p));
  memset(buf + لڙي_ڊيگهه(p), ' ', DIRSIZ - لڙي_ڊيگهه(p));
  buf[حجم(buf) - 1] = '\0';
  أرجع buf;
}

فراغ
ls(حرف *رستو)
{
  حرف buf[512], *p;
  صحيح fd;
  بنية dirent de;
  بنية stat st;

  إذا ((fd = open(رستو, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", رستو);
    أرجع;
  }

  إذا (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", رستو);
    close(fd);
    أرجع;
  }

  بدل (st.type) {
  حالة T_DEVICE:
  حالة T_FILE:
    لکو("%s %d %d %d\n", fmtname(رستو), st.type, st.ino, (صحيح)st.سائيز);
    اكسر;

  حالة T_DIR:
    إذا (لڙي_ڊيگهه(رستو) + 1 + DIRSIZ + 1 > حجم buf) {
      لکو("ls: path too long\n");
      اكسر;
    }
    strcpy(buf, رستو);
    p = buf + لڙي_ڊيگهه(buf);
    *p++ = '/';
    طالما (اقرأ(fd, &de, حجم(de)) == حجم(de)) {
      إذا (de.inum == 0)
        استمر;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      إذا (stat(buf, &st) < 0) {
        لکو("ls: cannot stat %s\n", buf);
        استمر;
      }
      لکو("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (صحيح)st.سائيز);
    }
    اكسر;
  }
  close(fd);
}

صحيح
مکيه(صحيح argc, حرف *argv[])
{
  صحيح i;

  إذا (argc < 2) {
    ls(".");
    ٻاهر_نڪرو(0);
  }
  لكل (i = 1; i < argc; i++)
    ls(argv[i]);
  ٻاهر_نڪرو(0);
}
