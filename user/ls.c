#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/fcntl.h"

حرف *
fmtname(حرف *مسیر)
{
  ساكن حرف buf[DIRSIZ + 1];
  حرف *p;

  // Find first character after last slash.
  لكل (p = مسیر + طول_رشته(مسیر); p >= مسیر && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  إذا (طول_رشته(p) >= DIRSIZ)
    أرجع p;
  memmove(buf, p, طول_رشته(p));
  memset(buf + طول_رشته(p), ' ', DIRSIZ - طول_رشته(p));
  buf[حجم(buf) - 1] = '\0';
  أرجع buf;
}

فراغ
ls(حرف *مسیر)
{
  حرف buf[512], *p;
  صحيح fd;
  بنية dirent de;
  بنية stat st;

  إذا ((fd = open(مسیر, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", مسیر);
    أرجع;
  }

  إذا (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", مسیر);
    close(fd);
    أرجع;
  }

  بدل (st.type) {
  حالة T_DEVICE:
  حالة T_FILE:
    چاپ_کن("%s %d %d %d\n", fmtname(مسیر), st.type, st.ino, (صحيح)st.اندازه);
    اكسر;

  حالة T_DIR:
    إذا (طول_رشته(مسیر) + 1 + DIRSIZ + 1 > حجم buf) {
      چاپ_کن("ls: path too long\n");
      اكسر;
    }
    strcpy(buf, مسیر);
    p = buf + طول_رشته(buf);
    *p++ = '/';
    طالما (اقرأ(fd, &de, حجم(de)) == حجم(de)) {
      إذا (de.inum == 0)
        استمر;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      إذا (stat(buf, &st) < 0) {
        چاپ_کن("ls: cannot stat %s\n", buf);
        استمر;
      }
      چاپ_کن("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (صحيح)st.اندازه);
    }
    اكسر;
  }
  close(fd);
}

صحيح
اصلی(صحيح argc, حرف *argv[])
{
  صحيح i;

  إذا (argc < 2) {
    ls(".");
    خروج(0);
  }
  لكل (i = 1; i < argc; i++)
    ls(argv[i]);
  خروج(0);
}
