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
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  ᮚᮦᮔ᮪ (strlen(p) >= DIRSIZ)
    ᮝᮜᮤ p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[parimaa_na(buf) - 1] = '\0';
  ᮝᮜᮤ buf;
}

void
ls(var_na *path)
{
  var_na buf[512], *p;
  puur_naa_nka fd;
  sa_nracanaa dirent de;
  sa_nracanaa stat st;

  ᮚᮦᮔ᮪ ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    ᮝᮜᮤ;
  }

  ᮚᮦᮔ᮪ (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    ᮝᮜᮤ;
  }

  switch (st.type) {
  pak_sa T_DEVICE:
  pak_sa T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (puur_naa_nka)st.size);
    bha_nga;

  pak_sa T_DIR:
    ᮚᮦᮔ᮪ (strlen(path) + 1 + DIRSIZ + 1 > parimaa_na buf) {
      printf("ls: path too long\n");
      bha_nga;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    ᮞᮨᮓᮨᮜ (read(fd, &de, parimaa_na(de)) == parimaa_na(de)) {
      ᮚᮦᮔ᮪ (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      ᮚᮦᮔ᮪ (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (puur_naa_nka)st.size);
    }
    bha_nga;
  }
  close(fd);
}

puur_naa_nka
main(puur_naa_nka argc, var_na *argv[])
{
  puur_naa_nka i;

  ᮚᮦᮔ᮪ (argc < 2) {
    ls(".");
    exit(0);
  }
  for (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
