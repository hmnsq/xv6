#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/fcntl.h"

वर्ण *
fmtname(वर्ण *path)
{
  स्थिर वर्ण buf[DIRSIZ + 1];
  वर्ण *p;

  // Find first character after last slash.
  अत्थाय (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  सचे (strlen(p) >= DIRSIZ)
    पटिनिवत्त p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[परिमाण(buf) - 1] = '\0';
  पटिनिवत्त buf;
}

शून्य
ls(वर्ण *path)
{
  वर्ण buf[512], *p;
  पूर्णाङ्क fd;
  संरचना dirent de;
  संरचना stat st;

  सचे ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    पटिनिवत्त;
  }

  सचे (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    पटिनिवत्त;
  }

  विकल्प (st.type) {
  पक्ष T_DEVICE:
  पक्ष T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (पूर्णाङ्क)st.size);
    भङ्ग;

  पक्ष T_DIR:
    सचे (strlen(path) + 1 + DIRSIZ + 1 > परिमाण buf) {
      printf("ls: path too long\n");
      भङ्ग;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    याव (पठ(fd, &de, परिमाण(de)) == परिमाण(de)) {
      सचे (de.inum == 0)
        अनुवर्त;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      सचे (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        अनुवर्त;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (पूर्णाङ्क)st.size);
    }
    भङ्ग;
  }
  close(fd);
}

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क i;

  सचे (argc < 2) {
    ls(".");
    exit(0);
  }
  अत्थाय (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
