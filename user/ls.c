#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"
#అన్తర్భావ "kernel/fs.h"
#అన్తర్భావ "kernel/fcntl.h"

అక్షరం *
fmtname(అక్షరం *మార్గం)
{
  స్థిర అక్షరం buf[DIRSIZ + 1];
  అక్షరం *p;

  // Find first character after last slash.
  కొరకు (p = మార్గం + అక్షరశ్రేణి_పొడవు(మార్గం); p >= మార్గం && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  ఒకవేళ (అక్షరశ్రేణి_పొడవు(p) >= DIRSIZ)
    తిరిగి p;
  memmove(buf, p, అక్షరశ్రేణి_పొడవు(p));
  memset(buf + అక్షరశ్రేణి_పొడవు(p), ' ', DIRSIZ - అక్షరశ్రేణి_పొడవు(p));
  buf[పరిమాణం(buf) - 1] = '\0';
  తిరిగి buf;
}

శూన్య
ls(అక్షరం *మార్గం)
{
  అక్షరం buf[512], *p;
  పూర్ణాంకం fd;
  నిర్మాణం dirent de;
  నిర్మాణం stat st;

  ఒకవేళ ((fd = open(మార్గం, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", మార్గం);
    తిరిగి;
  }

  ఒకవేళ (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", మార్గం);
    close(fd);
    తిరిగి;
  }

  ఎంపిక (st.type) {
  స్థితి T_DEVICE:
  స్థితి T_FILE:
    రాయి("%s %d %d %d\n", fmtname(మార్గం), st.type, st.ino, (పూర్ణాంకం)st.కొలత);
    ఆపు;

  స్థితి T_DIR:
    ఒకవేళ (అక్షరశ్రేణి_పొడవు(మార్గం) + 1 + DIRSIZ + 1 > పరిమాణం buf) {
      రాయి("ls: path too long\n");
      ఆపు;
    }
    strcpy(buf, మార్గం);
    p = buf + అక్షరశ్రేణి_పొడవు(buf);
    *p++ = '/';
    అయితే (పఠ(fd, &de, పరిమాణం(de)) == పరిమాణం(de)) {
      ఒకవేళ (de.inum == 0)
        కొనసాగు;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      ఒకవేళ (stat(buf, &st) < 0) {
        రాయి("ls: cannot stat %s\n", buf);
        కొనసాగు;
      }
      రాయి("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (పూర్ణాంకం)st.కొలత);
    }
    ఆపు;
  }
  close(fd);
}

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  పూర్ణాంకం i;

  ఒకవేళ (argc < 2) {
    ls(".");
    నిష్క్రమించు(0);
  }
  కొరకు (i = 1; i < argc; i++)
    ls(argv[i]);
  నిష్క్రమించు(0);
}
