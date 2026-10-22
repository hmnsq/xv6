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
  ཆེད་དུ (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  གལ་ཏེ (strlen(p) >= DIRSIZ)
    ཕྱིར p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[parimaa_na(buf) - 1] = '\0';
  ཕྱིར buf;
}

void
ls(var_na *path)
{
  var_na buf[512], *p;
  ཧྲིལ fd;
  sa_nracanaa dirent de;
  sa_nracanaa stat st;

  གལ་ཏེ ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    ཕྱིར;
  }

  གལ་ཏེ (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    ཕྱིར;
  }

  switch (st.type) {
  pak_sa T_DEVICE:
  pak_sa T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (ཧྲིལ)st.size);
    bha_nga;

  pak_sa T_DIR:
    གལ་ཏེ (strlen(path) + 1 + DIRSIZ + 1 > parimaa_na buf) {
      printf("ls: path too long\n");
      bha_nga;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    སྐབས (read(fd, &de, parimaa_na(de)) == parimaa_na(de)) {
      གལ་ཏེ (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      གལ་ཏེ (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (ཧྲིལ)st.size);
    }
    bha_nga;
  }
  close(fd);
}

ཧྲིལ
གཙོ(ཧྲིལ argc, var_na *argv[])
{
  ཧྲིལ i;

  གལ་ཏེ (argc < 2) {
    ls(".");
    exit(0);
  }
  ཆེད་དུ (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
