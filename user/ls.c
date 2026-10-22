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
  за (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  ако (strlen(p) >= DIRSIZ)
    врати p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[sizeof(buf) - 1] = '\0';
  врати buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  цео fd;
  struct dirent de;
  struct stat st;

  ако ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    врати;
  }

  ако (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    врати;
  }

  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (цео)st.size);
    break;

  case T_DIR:
    ако (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    док (read(fd, &de, sizeof(de)) == sizeof(de)) {
      ако (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      ако (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (цео)st.size);
    }
    break;
  }
  close(fd);
}

цео
main(цео argc, char *argv[])
{
  цео i;

  ако (argc < 2) {
    ls(".");
    exit(0);
  }
  за (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
