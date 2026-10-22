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
  ために (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  もし (strlen(p) >= DIRSIZ)
    戻る p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[sizeof(buf) - 1] = '\0';
  戻る buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  整数 fd;
  struct dirent de;
  struct stat st;

  もし ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    戻る;
  }

  もし (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    戻る;
  }

  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (整数)st.size);
    break;

  case T_DIR:
    もし (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    ながら (read(fd, &de, sizeof(de)) == sizeof(de)) {
      もし (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      もし (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (整数)st.size);
    }
    break;
  }
  close(fd);
}

整数
メインページ(整数 argc, char *argv[])
{
  整数 i;

  もし (argc < 2) {
    ls(".");
    exit(0);
  }
  ために (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
