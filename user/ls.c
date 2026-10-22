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
  အတွက် (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  အကယ်၍ (strlen(p) >= DIRSIZ)
    ပြန်ပေး p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[parimaa_na(buf) - 1] = '\0';
  ပြန်ပေး buf;
}

void
ls(var_na *path)
{
  var_na buf[512], *p;
  ကိန်းပြည့် fd;
  sa_nracanaa dirent de;
  sa_nracanaa stat st;

  အကယ်၍ ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    ပြန်ပေး;
  }

  အကယ်၍ (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    ပြန်ပေး;
  }

  switch (st.type) {
  pak_sa T_DEVICE:
  pak_sa T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (ကိန်းပြည့်)st.size);
    bha_nga;

  pak_sa T_DIR:
    အကယ်၍ (strlen(path) + 1 + DIRSIZ + 1 > parimaa_na buf) {
      printf("ls: path too long\n");
      bha_nga;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    နေစဉ် (read(fd, &de, parimaa_na(de)) == parimaa_na(de)) {
      အကယ်၍ (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      အကယ်၍ (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (ကိန်းပြည့်)st.size);
    }
    bha_nga;
  }
  close(fd);
}

ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na *argv[])
{
  ကိန်းပြည့် i;

  အကယ်၍ (argc < 2) {
    ls(".");
    exit(0);
  }
  အတွက် (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
