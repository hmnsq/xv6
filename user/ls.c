#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *
fmtname(char *path)
{
  static char buf[DIRSIZ + 1];
  char *p;

  // Find first character after last slash.
  для (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  якщо (strlen(p) >= DIRSIZ)
    повернути p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[sizeof(buf) - 1] = '\0';
  повернути buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  ціле fd;
  struct dirent de;
  struct stat st;

  якщо ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    повернути;
  }

  якщо (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    повернути;
  }

  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (ціле)st.size);
    break;

  case T_DIR:
    якщо (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    поки (read(fd, &de, sizeof(de)) == sizeof(de)) {
      якщо (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      якщо (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (ціле)st.size);
    }
    break;
  }
  close(fd);
}

ціле
main(ціле argc, char *argv[])
{
  ціле i;

  якщо (argc < 2) {
    ls(".");
    exit(0);
  }
  для (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
