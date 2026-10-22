#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "user/user.h"
#അന്തര്ഭാവ "kernel/fs.h"
#അന്തര്ഭാവ "kernel/fcntl.h"

അക്ഷരം *
fmtname(അക്ഷരം *path)
{
  സ്ഥിതം അക്ഷരം buf[DIRSIZ + 1];
  അക്ഷരം *p;

  // Find first character after last slash.
  വേണ്ടി (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  എങ്കിൽ (strlen(p) >= DIRSIZ)
    തിരികെ p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[വലുപ്പം(buf) - 1] = '\0';
  തിരികെ buf;
}

ശൂന്യം
ls(അക്ഷരം *path)
{
  അക്ഷരം buf[512], *p;
  പൂർണ്ണസംഖ്യ fd;
  ഘടന dirent de;
  ഘടന stat st;

  എങ്കിൽ ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    തിരികെ;
  }

  എങ്കിൽ (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    തിരികെ;
  }

  തിരഞ്ഞെടുപ്പ് (st.type) {
  അവസ്ഥ T_DEVICE:
  അവസ്ഥ T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (പൂർണ്ണസംഖ്യ)st.size);
    നിർത്തുക;

  അവസ്ഥ T_DIR:
    എങ്കിൽ (strlen(path) + 1 + DIRSIZ + 1 > വലുപ്പം buf) {
      printf("ls: path too long\n");
      നിർത്തുക;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    ആകുമ്പോൾ (പഠ(fd, &de, വലുപ്പം(de)) == വലുപ്പം(de)) {
      എങ്കിൽ (de.inum == 0)
        തുടരുക;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      എങ്കിൽ (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        തുടരുക;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (പൂർണ്ണസംഖ്യ)st.size);
    }
    നിർത്തുക;
  }
  close(fd);
}

പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
  പൂർണ്ണസംഖ്യ i;

  എങ്കിൽ (argc < 2) {
    ls(".");
    exit(0);
  }
  വേണ്ടി (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
