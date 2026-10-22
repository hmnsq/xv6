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
  если (strlen(p) >= DIRSIZ)
    вернуть p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[sizeof(buf) - 1] = '\0';
  вернуть buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  целое fd;
  struct dirent de;
  struct stat st;

  если ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    вернуть;
  }

  если (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    вернуть;
  }

  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (целое)st.size);
    break;

  case T_DIR:
    если (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    пока (read(fd, &de, sizeof(de)) == sizeof(de)) {
      если (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      если (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (целое)st.size);
    }
    break;
  }
  close(fd);
}

целое
основной(целое argc, char *argv[])
{
  целое i;

  если (argc < 2) {
    ls(".");
    exit(0);
  }
  для (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
