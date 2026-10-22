#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/fcntl.h"

حرف *
fmtname(حرف *رستہ)
{
  ساكن حرف buf[DIRSIZ + 1];
  حرف *p;

  // Find first character after last slash.
  لكل (p = رستہ + لڑی_لمبائی(رستہ); p >= رستہ && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  إذا (لڑی_لمبائی(p) >= DIRSIZ)
    أرجع p;
  memmove(buf, p, لڑی_لمبائی(p));
  memset(buf + لڑی_لمبائی(p), ' ', DIRSIZ - لڑی_لمبائی(p));
  buf[حجم(buf) - 1] = '\0';
  أرجع buf;
}

فراغ
ls(حرف *رستہ)
{
  حرف buf[512], *p;
  صحيح fd;
  بنية dirent de;
  بنية stat st;

  إذا ((fd = open(رستہ, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", رستہ);
    أرجع;
  }

  إذا (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", رستہ);
    close(fd);
    أرجع;
  }

  بدل (st.type) {
  حالة T_DEVICE:
  حالة T_FILE:
    لکھو("%s %d %d %d\n", fmtname(رستہ), st.type, st.ino, (صحيح)st.ناپ);
    اكسر;

  حالة T_DIR:
    إذا (لڑی_لمبائی(رستہ) + 1 + DIRSIZ + 1 > حجم buf) {
      لکھو("ls: path too long\n");
      اكسر;
    }
    strcpy(buf, رستہ);
    p = buf + لڑی_لمبائی(buf);
    *p++ = '/';
    طالما (اقرأ(fd, &de, حجم(de)) == حجم(de)) {
      إذا (de.inum == 0)
        استمر;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      إذا (stat(buf, &st) < 0) {
        لکھو("ls: cannot stat %s\n", buf);
        استمر;
      }
      لکھو("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (صحيح)st.ناپ);
    }
    اكسر;
  }
  close(fd);
}

صحيح
مکھ(صحيح argc, حرف *argv[])
{
  صحيح i;

  إذا (argc < 2) {
    ls(".");
    باہر_نکلو(0);
  }
  لكل (i = 1; i < argc; i++)
    ls(argv[i]);
  باہر_نکلو(0);
}
