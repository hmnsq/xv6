#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fs.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"

ଅକ୍ଷର *
fmtname(ଅକ୍ଷର *path)
{
  ସ୍ଥିର ଅକ୍ଷର buf[DIRSIZ + 1];
  ଅକ୍ଷର *p;

  // Find first character after last slash.
  ପାଇଁ (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  ଯଦି (strlen(p) >= DIRSIZ)
    ଫେରନ୍ତୁ p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[ଆକାର(buf) - 1] = '\0';
  ଫେରନ୍ତୁ buf;
}

ଶୂନ୍ୟ
ls(ଅକ୍ଷର *path)
{
  ଅକ୍ଷର buf[512], *p;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
  ସଂରଚନା dirent de;
  ସଂରଚନା stat st;

  ଯଦି ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    ଫେରନ୍ତୁ;
  }

  ଯଦି (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    ଫେରନ୍ତୁ;
  }

  ବାଛ (st.type) {
  ସ୍ଥିତି T_DEVICE:
  ସ୍ଥିତି T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)st.size);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି T_DIR:
    ଯଦି (strlen(path) + 1 + DIRSIZ + 1 > ଆକାର buf) {
      printf("ls: path too long\n");
      ଭାଙ୍ଗ;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    ଯେତେବେଳେ (ପଠ(fd, &de, ଆକାର(de)) == ଆକାର(de)) {
      ଯଦି (de.inum == 0)
        ଜାରି;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      ଯଦି (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        ଜାରି;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)st.size);
    }
    ଭାଙ୍ଗ;
  }
  close(fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ଯଦି (argc < 2) {
    ls(".");
    exit(0);
  }
  ପାଇଁ (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
