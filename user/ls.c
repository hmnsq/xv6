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
  लागि (p = path + म_माप(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  यदि (म_माप(p) >= DIRSIZ)
    फर्काऊ p;
  स_हटाओ(buf, p, म_माप(p));
  स_रखो(buf + म_माप(p), ' ', DIRSIZ - म_माप(p));
  buf[परिमाण(buf) - 1] = '\0';
  फर्काऊ buf;
}

शून्य
ls(वर्ण *path)
{
  वर्ण buf[512], *p;
  पूर्णाङ्क fd;
  संरचना dirent de;
  संरचना stat st;

  यदि ((fd = खोलो(path, O_RDONLY)) < 0) {
    ख_लिखो(2, "ls: cannot open %s\n", path);
    फर्काऊ;
  }

  यदि (ख_स्थिति(fd, &st) < 0) {
    ख_लिखो(2, "ls: cannot stat %s\n", path);
    बंद(fd);
    फर्काऊ;
  }

  विकल्प (st.type) {
  पक्ष T_DEVICE:
  पक्ष T_FILE:
    म_लिखो("%s %d %d %d\n", fmtname(path), st.type, st.ino, (पूर्णाङ्क)st.size);
    रोक;

  पक्ष T_DIR:
    यदि (म_माप(path) + 1 + DIRSIZ + 1 > परिमाण buf) {
      म_लिखो("ls: path too long\n");
      रोक;
    }
    म_नकल(buf, path);
    p = buf + म_माप(buf);
    *p++ = '/';
    जबसम्म (पठ(fd, &de, परिमाण(de)) == परिमाण(de)) {
      यदि (de.inum == 0)
        जारी;
      स_हटाओ(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      यदि (stat(buf, &st) < 0) {
        म_लिखो("ls: cannot stat %s\n", buf);
        जारी;
      }
      म_लिखो("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (पूर्णाङ्क)st.size);
    }
    रोक;
  }
  बंद(fd);
}

पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क i;

  यदि (argc < 2) {
    ls(".");
    निकास(0);
  }
  लागि (i = 1; i < argc; i++)
    ls(argv[i]);
  निकास(0);
}
