#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "user/user.h"
#समावेश "kernel/fs.h"
#समावेश "kernel/fcntl.h"

अक्षर *
fmtname(अक्षर *path)
{
  अटल अक्षर buf[DIRSIZ + 1];
  अक्षर *p;

  // Find first character after last slash.
  क्रम (p = path + म_माप(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  अगर (म_माप(p) >= DIRSIZ)
    वापस p;
  स_हटाओ(buf, p, म_माप(p));
  स_रखो(buf + म_माप(p), ' ', DIRSIZ - म_माप(p));
  buf[माप(buf) - 1] = '\0';
  वापस buf;
}

व्योम
ls(अक्षर *path)
{
  अक्षर buf[512], *p;
  पूर्णांक fd;
  काष्ठा dirent de;
  काष्ठा stat st;

  अगर ((fd = खोलो(path, O_RDONLY)) < 0) {
    ख_लिखो(2, "ls: cannot open %s\n", path);
    वापस;
  }

  अगर (ख_स्थिति(fd, &st) < 0) {
    ख_लिखो(2, "ls: cannot stat %s\n", path);
    बंद(fd);
    वापस;
  }

  चयन (st.type) {
  हाल T_DEVICE:
  हाल T_FILE:
    म_लिखो("%s %d %d %d\n", fmtname(path), st.type, st.ino, (पूर्णांक)st.आकार);
    अवरोध;

  हाल T_DIR:
    अगर (म_माप(path) + 1 + DIRSIZ + 1 > माप buf) {
      म_लिखो("ls: path too long\n");
      अवरोध;
    }
    म_नकल(buf, path);
    p = buf + म_माप(buf);
    *p++ = '/';
    जबतक (पढ़ो(fd, &de, माप(de)) == माप(de)) {
      अगर (de.inum == 0)
        जारी;
      स_हटाओ(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      अगर (stat(buf, &st) < 0) {
        म_लिखो("ls: cannot stat %s\n", buf);
        जारी;
      }
      म_लिखो("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (पूर्णांक)st.आकार);
    }
    अवरोध;
  }
  बंद(fd);
}

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक i;

  अगर (argc < 2) {
    ls(".");
    निकास(0);
  }
  क्रम (i = 1; i < argc; i++)
    ls(argv[i]);
  निकास(0);
}
