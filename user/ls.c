#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/fcntl.h"

अक्षर *
fmtname(अक्षर *path)
{
  स्थिर अक्षर buf[DIRSIZ + 1];
  अक्षर *p;

  // Find first character after last slash.
  साठी (p = path + म_माप(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  जर (म_माप(p) >= DIRSIZ)
    परत p;
  स_हटाओ(buf, p, म_माप(p));
  स_रखो(buf + म_माप(p), ' ', DIRSIZ - म_माप(p));
  buf[आकार(buf) - 1] = '\0';
  परत buf;
}

शून्य
ls(अक्षर *path)
{
  अक्षर buf[512], *p;
  पूर्णांक fd;
  संरचना dirent de;
  संरचना stat st;

  जर ((fd = खोलो(path, O_RDONLY)) < 0) {
    ख_लिखो(2, "ls: cannot open %s\n", path);
    परत;
  }

  जर (ख_स्थिति(fd, &st) < 0) {
    ख_लिखो(2, "ls: cannot stat %s\n", path);
    बंद(fd);
    परत;
  }

  निवड (st.type) {
  स्थिती T_DEVICE:
  स्थिती T_FILE:
    म_लिखो("%s %d %d %d\n", fmtname(path), st.type, st.ino, (पूर्णांक)st.size);
    थांबा;

  स्थिती T_DIR:
    जर (म_माप(path) + 1 + DIRSIZ + 1 > आकार buf) {
      म_लिखो("ls: path too long\n");
      थांबा;
    }
    म_नकल(buf, path);
    p = buf + म_माप(buf);
    *p++ = '/';
    पर्यंत (पठ(fd, &de, आकार(de)) == आकार(de)) {
      जर (de.inum == 0)
        सुरूठेवा;
      स_हटाओ(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      जर (stat(buf, &st) < 0) {
        म_लिखो("ls: cannot stat %s\n", buf);
        सुरूठेवा;
      }
      म_लिखो("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (पूर्णांक)st.size);
    }
    थांबा;
  }
  बंद(fd);
}

पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक i;

  जर (argc < 2) {
    ls(".");
    निकास(0);
  }
  साठी (i = 1; i < argc; i++)
    ls(argv[i]);
  निकास(0);
}
