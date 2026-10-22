#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/fcntl.h"

वर्ण *
fmtname(वर्ण *मार्गः)
{
  स्थिर वर्ण buf[DIRSIZ + 1];
  वर्ण *p;

  // Find first character after last slash.
  प्रति (p = मार्गः + वाक्यदैर्घ्यम्(मार्गः); p >= मार्गः && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  यदि (वाक्यदैर्घ्यम्(p) >= DIRSIZ)
    प्रत्यागम p;
  memmove(buf, p, वाक्यदैर्घ्यम्(p));
  स्मृतिं_पूरय(buf + वाक्यदैर्घ्यम्(p), ' ', DIRSIZ - वाक्यदैर्घ्यम्(p));
  buf[परिमाण(buf) - 1] = '\0';
  प्रत्यागम buf;
}

शून्य
ls(वर्ण *मार्गः)
{
  वर्ण buf[512], *p;
  पूर्णाङ्क fd;
  संरचना dirent de;
  संरचना stat st;

  यदि ((fd = उद्घाटय(मार्गः, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", मार्गः);
    प्रत्यागम;
  }

  यदि (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", मार्गः);
    close(fd);
    प्रत्यागम;
  }

  विकल्प (st.type) {
  पक्ष T_DEVICE:
  पक्ष T_FILE:
    प्रारूप_लिख("%s %d %d %d\n", fmtname(मार्गः), st.type, st.ino, (पूर्णाङ्क)st.आकार);
    भङ्ग;

  पक्ष T_DIR:
    यदि (वाक्यदैर्घ्यम्(मार्गः) + 1 + DIRSIZ + 1 > परिमाण buf) {
      प्रारूप_लिख("ls: path too long\n");
      भङ्ग;
    }
    वाक्यं_प्रतिलिख(buf, मार्गः);
    p = buf + वाक्यदैर्घ्यम्(buf);
    *p++ = '/';
    यावत् (पठ(fd, &de, परिमाण(de)) == परिमाण(de)) {
      यदि (de.inum == 0)
        अनुवर्त;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      यदि (stat(buf, &st) < 0) {
        प्रारूप_लिख("ls: cannot stat %s\n", buf);
        अनुवर्त;
      }
      प्रारूप_लिख("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (पूर्णाङ्क)st.आकार);
    }
    भङ्ग;
  }
  close(fd);
}

पूर्णाङ्क
मुख्यम्(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क i;

  यदि (argc < 2) {
    ls(".");
    निर्गच्छ(0);
  }
  प्रति (i = 1; i < argc; i++)
    ls(argv[i]);
  निर्गच्छ(0);
}
