#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"

ਅੱਖਰ *
fmtname(ਅੱਖਰ *ਰਸਤਾ)
{
  ਸਥਿਰ ਅੱਖਰ buf[DIRSIZ + 1];
  ਅੱਖਰ *p;

  // Find first character after last slash.
  ਲਈ (p = ਰਸਤਾ + ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ); p >= ਰਸਤਾ && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  ਜੇ (ਲੜੀ_ਲੰਬਾਈ(p) >= DIRSIZ)
    ਵਾਪਸ p;
  memmove(buf, p, ਲੜੀ_ਲੰਬਾਈ(p));
  memset(buf + ਲੜੀ_ਲੰਬਾਈ(p), ' ', DIRSIZ - ਲੜੀ_ਲੰਬਾਈ(p));
  buf[ਆਕਾਰ(buf) - 1] = '\0';
  ਵਾਪਸ buf;
}

ਖਾਲੀ
ls(ਅੱਖਰ *ਰਸਤਾ)
{
  ਅੱਖਰ buf[512], *p;
  ਪੂਰਨ fd;
  ਬਣਤਰ dirent de;
  ਬਣਤਰ stat st;

  ਜੇ ((fd = open(ਰਸਤਾ, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", ਰਸਤਾ);
    ਵਾਪਸ;
  }

  ਜੇ (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", ਰਸਤਾ);
    close(fd);
    ਵਾਪਸ;
  }

  ਚੋਣ (st.type) {
  ਹਾਲਤ T_DEVICE:
  ਹਾਲਤ T_FILE:
    ਲਿਖੋ("%s %d %d %d\n", fmtname(ਰਸਤਾ), st.type, st.ino, (ਪੂਰਨ)st.ਮਾਪ);
    ਰੋਕੋ;

  ਹਾਲਤ T_DIR:
    ਜੇ (ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ) + 1 + DIRSIZ + 1 > ਆਕਾਰ buf) {
      ਲਿਖੋ("ls: path too long\n");
      ਰੋਕੋ;
    }
    strcpy(buf, ਰਸਤਾ);
    p = buf + ਲੜੀ_ਲੰਬਾਈ(buf);
    *p++ = '/';
    ਜਦੋਂ (ਪਠ(fd, &de, ਆਕਾਰ(de)) == ਆਕਾਰ(de)) {
      ਜੇ (de.inum == 0)
        ਜਾਰੀ;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      ਜੇ (stat(buf, &st) < 0) {
        ਲਿਖੋ("ls: cannot stat %s\n", buf);
        ਜਾਰੀ;
      }
      ਲਿਖੋ("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (ਪੂਰਨ)st.ਮਾਪ);
    }
    ਰੋਕੋ;
  }
  close(fd);
}

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
  ਪੂਰਨ i;

  ਜੇ (argc < 2) {
    ls(".");
    ਬਾਹਰ_ਨਿਕਲੋ(0);
  }
  ਲਈ (i = 1; i < argc; i++)
    ls(argv[i]);
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}
