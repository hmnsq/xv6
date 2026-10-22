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
  cho (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  nếu (strlen(p) >= DIRSIZ)
    trả_về p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[sizeof(buf) - 1] = '\0';
  trả_về buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  số_nguyên fd;
  struct dirent de;
  struct stat st;

  nếu ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    trả_về;
  }

  nếu (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    trả_về;
  }

  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (số_nguyên)st.size);
    break;

  case T_DIR:
    nếu (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    trong_khi (read(fd, &de, sizeof(de)) == sizeof(de)) {
      nếu (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      nếu (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (số_nguyên)st.size);
    }
    break;
  }
  close(fd);
}

số_nguyên
main(số_nguyên argc, char *argv[])
{
  số_nguyên i;

  nếu (argc < 2) {
    ls(".");
    exit(0);
  }
  cho (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
