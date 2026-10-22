#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/fs.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"

ವರ್ಣ *
fmtname(ವರ್ಣ *path)
{
  ಸ್ಥಿರ ವರ್ಣ buf[DIRSIZ + 1];
  ವರ್ಣ *p;

  // Find first character after last slash.
  ಪ್ರತಿ (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  ಯದಿ (strlen(p) >= DIRSIZ)
    ಪ್ರತ್ಯಾಗಮ p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[ಪರಿಮಾಣ(buf) - 1] = '\0';
  ಪ್ರತ್ಯಾಗಮ buf;
}

ಶೂನ್ಯ
ls(ವರ್ಣ *path)
{
  ವರ್ಣ buf[512], *p;
  ಪೂರ್ಣಾಙ್ಕ fd;
  ಸಂರಚನಾ dirent de;
  ಸಂರಚನಾ stat st;

  ಯದಿ ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    ಪ್ರತ್ಯಾಗಮ;
  }

  ವಿಕಲ್ಪ (st.type) {
  ಪಕ್ಷ T_DEVICE:
  ಪಕ್ಷ T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (ಪೂರ್ಣಾಙ್ಕ)st.size);
    ಭಙ್ಗ;

  ಪಕ್ಷ T_DIR:
    ಯದಿ (strlen(path) + 1 + DIRSIZ + 1 > ಪರಿಮಾಣ buf) {
      printf("ls: path too long\n");
      ಭಙ್ಗ;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    ಯಾವತ್ (ಪಠ(fd, &de, ಪರಿಮಾಣ(de)) == ಪರಿಮಾಣ(de)) {
      ಯದಿ (de.inum == 0)
        ಅನುವರ್ತ;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      ಯದಿ (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        ಅನುವರ್ತ;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (ಪೂರ್ಣಾಙ್ಕ)st.size);
    }
    ಭಙ್ಗ;
  }
  close(fd);
}

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
  ಪೂರ್ಣಾಙ್ಕ i;

  ಯದಿ (argc < 2) {
    ls(".");
    exit(0);
  }
  ಪ್ರತಿ (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
