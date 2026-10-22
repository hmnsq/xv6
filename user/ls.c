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
  위해 (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  만약 (strlen(p) >= DIRSIZ)
    반환 p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[sizeof(buf) - 1] = '\0';
  반환 buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  정수 fd;
  struct dirent de;
  struct stat st;

  만약 ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    반환;
  }

  만약 (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    반환;
  }

  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (정수)st.size);
    break;

  case T_DIR:
    만약 (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    동안 (read(fd, &de, sizeof(de)) == sizeof(de)) {
      만약 (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      만약 (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (정수)st.size);
    }
    break;
  }
  close(fd);
}

정수
main(정수 argc, char *argv[])
{
  정수 i;

  만약 (argc < 2) {
    ls(".");
    exit(0);
  }
  위해 (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
