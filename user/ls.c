#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

var_na *
fmtname(var_na *path)
{
  static var_na buf[DIRSIZ + 1];
  var_na *p;

  // Find first character after last slash.
  สำหรับ (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  ถ้า (strlen(p) >= DIRSIZ)
    คืน p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[parimaa_na(buf) - 1] = '\0';
  คืน buf;
}

void
ls(var_na *path)
{
  var_na buf[512], *p;
  จำนวนเต็ม fd;
  sa_nracanaa dirent de;
  sa_nracanaa stat st;

  ถ้า ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    คืน;
  }

  ถ้า (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    คืน;
  }

  switch (st.type) {
  pak_sa T_DEVICE:
  pak_sa T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (จำนวนเต็ม)st.size);
    bha_nga;

  pak_sa T_DIR:
    ถ้า (strlen(path) + 1 + DIRSIZ + 1 > parimaa_na buf) {
      printf("ls: path too long\n");
      bha_nga;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    ขณะ (read(fd, &de, parimaa_na(de)) == parimaa_na(de)) {
      ถ้า (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      ถ้า (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (จำนวนเต็ม)st.size);
    }
    bha_nga;
  }
  close(fd);
}

จำนวนเต็ม
หลัก(จำนวนเต็ม argc, var_na *argv[])
{
  จำนวนเต็ม i;

  ถ้า (argc < 2) {
    ls(".");
    exit(0);
  }
  สำหรับ (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
