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
  សម្រាប់ (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  ប្រសិនបើ (strlen(p) >= DIRSIZ)
    ត្រឡប់ p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[parimaa_na(buf) - 1] = '\0';
  ត្រឡប់ buf;
}

void
ls(var_na *path)
{
  var_na buf[512], *p;
  ចំនួនគត់ fd;
  sa_nracanaa dirent de;
  sa_nracanaa stat st;

  ប្រសិនបើ ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    ត្រឡប់;
  }

  ប្រសិនបើ (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    ត្រឡប់;
  }

  switch (st.type) {
  pak_sa T_DEVICE:
  pak_sa T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (ចំនួនគត់)st.size);
    bha_nga;

  pak_sa T_DIR:
    ប្រសិនបើ (strlen(path) + 1 + DIRSIZ + 1 > parimaa_na buf) {
      printf("ls: path too long\n");
      bha_nga;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    ខណៈពេល (read(fd, &de, parimaa_na(de)) == parimaa_na(de)) {
      ប្រសិនបើ (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      ប្រសិនបើ (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (ចំនួនគត់)st.size);
    }
    bha_nga;
  }
  close(fd);
}

ចំនួនគត់
main(ចំនួនគត់ argc, var_na *argv[])
{
  ចំនួនគត់ i;

  ប្រសិនបើ (argc < 2) {
    ls(".");
    exit(0);
  }
  សម្រាប់ (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
