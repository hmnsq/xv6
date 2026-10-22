#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/fcntl.h"

حرف *
fmtname(حرف *لار)
{
  ساكن حرف buf[DIRSIZ + 1];
  حرف *p;

  // Find first character after last slash.
  لكل (p = لار + مزي_اوږدوالی(لار); p >= لار && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  إذا (مزي_اوږدوالی(p) >= DIRSIZ)
    أرجع p;
  memmove(buf, p, مزي_اوږدوالی(p));
  memset(buf + مزي_اوږدوالی(p), ' ', DIRSIZ - مزي_اوږدوالی(p));
  buf[حجم(buf) - 1] = '\0';
  أرجع buf;
}

فراغ
ls(حرف *لار)
{
  حرف buf[512], *p;
  صحيح fd;
  بنية dirent de;
  بنية stat st;

  إذا ((fd = open(لار, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", لار);
    أرجع;
  }

  إذا (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", لار);
    close(fd);
    أرجع;
  }

  بدل (st.type) {
  حالة T_DEVICE:
  حالة T_FILE:
    چاپ_کړه("%s %d %d %d\n", fmtname(لار), st.type, st.ino, (صحيح)st.اندازه);
    اكسر;

  حالة T_DIR:
    إذا (مزي_اوږدوالی(لار) + 1 + DIRSIZ + 1 > حجم buf) {
      چاپ_کړه("ls: path too long\n");
      اكسر;
    }
    strcpy(buf, لار);
    p = buf + مزي_اوږدوالی(buf);
    *p++ = '/';
    طالما (اقرأ(fd, &de, حجم(de)) == حجم(de)) {
      إذا (de.inum == 0)
        استمر;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      إذا (stat(buf, &st) < 0) {
        چاپ_کړه("ls: cannot stat %s\n", buf);
        استمر;
      }
      چاپ_کړه("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (صحيح)st.اندازه);
    }
    اكسر;
  }
  close(fd);
}

صحيح
اصلي(صحيح argc, حرف *argv[])
{
  صحيح i;

  إذا (argc < 2) {
    ls(".");
    وتل(0);
  }
  لكل (i = 1; i < argc; i++)
    ls(argv[i]);
  وتل(0);
}
