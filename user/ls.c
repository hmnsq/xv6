#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"
#شامل "kernel/fs.h"
#شامل "kernel/fcntl.h"

حرف *
fmtname(حرف *راستہ)
{
  ساکن حرف buf[DIRSIZ + 1];
  حرف *p;

  // Find first character after last slash.
  برائے (p = راستہ + لڑی_لمبائی(راستہ); p >= راستہ && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  اگر (لڑی_لمبائی(p) >= DIRSIZ)
    واپس p;
  memmove(buf, p, لڑی_لمبائی(p));
  memset(buf + لڑی_لمبائی(p), ' ', DIRSIZ - لڑی_لمبائی(p));
  buf[حجم(buf) - 1] = '\0';
  واپس buf;
}

خالی
ls(حرف *راستہ)
{
  حرف buf[512], *p;
  صحیح fd;
  ساخت dirent de;
  ساخت stat st;

  اگر ((fd = open(راستہ, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", راستہ);
    واپس;
  }

  اگر (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", راستہ);
    close(fd);
    واپس;
  }

  انتخاب (st.type) {
  حالت T_DEVICE:
  حالت T_FILE:
    لکھو("%s %d %d %d\n", fmtname(راستہ), st.type, st.ino, (صحیح)st.سائز);
    توڑو;

  حالت T_DIR:
    اگر (لڑی_لمبائی(راستہ) + 1 + DIRSIZ + 1 > حجم buf) {
      لکھو("ls: path too long\n");
      توڑو;
    }
    strcpy(buf, راستہ);
    p = buf + لڑی_لمبائی(buf);
    *p++ = '/';
    جب (اقرأ(fd, &de, حجم(de)) == حجم(de)) {
      اگر (de.inum == 0)
        جاری;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      اگر (stat(buf, &st) < 0) {
        لکھو("ls: cannot stat %s\n", buf);
        جاری;
      }
      لکھو("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (صحیح)st.سائز);
    }
    توڑو;
  }
  close(fd);
}

صحیح
مین(صحیح argc, حرف *argv[])
{
  صحیح i;

  اگر (argc < 2) {
    ls(".");
    باہر_نکلو(0);
  }
  برائے (i = 1; i < argc; i++)
    ls(argv[i]);
  باہر_نکلو(0);
}
