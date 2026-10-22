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
  ສຳລັບ (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  ຖ້າ (strlen(p) >= DIRSIZ)
    ສົ່ງຄືນ p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[parimaa_na(buf) - 1] = '\0';
  ສົ່ງຄືນ buf;
}

void
ls(var_na *path)
{
  var_na buf[512], *p;
  ຈຳນວນເຕັມ fd;
  sa_nracanaa dirent de;
  sa_nracanaa stat st;

  ຖ້າ ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    ສົ່ງຄືນ;
  }

  ຖ້າ (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    ສົ່ງຄືນ;
  }

  switch (st.type) {
  pak_sa T_DEVICE:
  pak_sa T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (ຈຳນວນເຕັມ)st.size);
    bha_nga;

  pak_sa T_DIR:
    ຖ້າ (strlen(path) + 1 + DIRSIZ + 1 > parimaa_na buf) {
      printf("ls: path too long\n");
      bha_nga;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    ໃນຂະນະທີ່ (read(fd, &de, parimaa_na(de)) == parimaa_na(de)) {
      ຖ້າ (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      ຖ້າ (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (ຈຳນວນເຕັມ)st.size);
    }
    bha_nga;
  }
  close(fd);
}

ຈຳນວນເຕັມ
main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
  ຈຳນວນເຕັມ i;

  ຖ້າ (argc < 2) {
    ls(".");
    exit(0);
  }
  ສຳລັບ (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
