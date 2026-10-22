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
  प्रति (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  यदि (strlen(p) >= DIRSIZ)
    प्रत्यागम p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[परिमाण(buf) - 1] = '\0';
  प्रत्यागम buf;
}

शून्य
ls(वर्ण *path)
{
  वर्ण buf[512], *p;
  पूर्णाङ्क fd;
  संरचना dirent de;
  संरचना stat st;

  यदि ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    प्रत्यागम;
  }

  यदि (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    प्रत्यागम;
  }

  विकल्प (st.type) {
  पक्ष T_DEVICE:
  पक्ष T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (पूर्णाङ्क)st.size);
    भङ्ग;

  पक्ष T_DIR:
    यदि (strlen(path) + 1 + DIRSIZ + 1 > परिमाण buf) {
      printf("ls: path too long\n");
      भङ्ग;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    यावत् (पठ(fd, &de, परिमाण(de)) == परिमाण(de)) {
      यदि (de.inum == 0)
        अनुवर्त;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      यदि (stat(buf, &st) < 0) {
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

  यदि (argc < 2) {
    ls(".");
    exit(0);
  }
  प्रति (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
