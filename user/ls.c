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
  සඳහා (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  නම් (strlen(p) >= DIRSIZ)
    ආපසු p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[parimaa_na(buf) - 1] = '\0';
  ආපසු buf;
}

void
ls(var_na *path)
{
  var_na buf[512], *p;
  පූර්ණ_සංඛ්‍යා fd;
  sa_nracanaa dirent de;
  sa_nracanaa stat st;

  නම් ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    ආපසු;
  }

  නම් (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    ආපසු;
  }

  switch (st.type) {
  pak_sa T_DEVICE:
  pak_sa T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (පූර්ණ_සංඛ්‍යා)st.size);
    bha_nga;

  pak_sa T_DIR:
    නම් (strlen(path) + 1 + DIRSIZ + 1 > parimaa_na buf) {
      printf("ls: path too long\n");
      bha_nga;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    අතරතුර (read(fd, &de, parimaa_na(de)) == parimaa_na(de)) {
      නම් (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      නම් (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (පූර්ණ_සංඛ්‍යා)st.size);
    }
    bha_nga;
  }
  close(fd);
}

පූර්ණ_සංඛ්‍යා
main(පූර්ණ_සංඛ්‍යා argc, var_na *argv[])
{
  පූර්ණ_සංඛ්‍යා i;

  නම් (argc < 2) {
    ls(".");
    exit(0);
  }
  සඳහා (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
