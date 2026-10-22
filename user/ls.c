#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"
#அந்தர்பாவ "kernel/fs.h"
#அந்தர்பாவ "kernel/fcntl.h"

எழுத்து *
fmtname(எழுத்து *path)
{
  நிலையான எழுத்து buf[DIRSIZ + 1];
  எழுத்து *p;

  // Find first character after last slash.
  க்கு (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  எனில் (strlen(p) >= DIRSIZ)
    திரும்பு p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[அளவு(buf) - 1] = '\0';
  திரும்பு buf;
}

வெறுமை
ls(எழுத்து *path)
{
  எழுத்து buf[512], *p;
  முழு fd;
  கட்டமைப்பு dirent de;
  கட்டமைப்பு stat st;

  எனில் ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    திரும்பு;
  }

  எனில் (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    திரும்பு;
  }

  தேர்வு (st.type) {
  நிலை T_DEVICE:
  நிலை T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (முழு)st.size);
    நிறுத்து;

  நிலை T_DIR:
    எனில் (strlen(path) + 1 + DIRSIZ + 1 > அளவு buf) {
      printf("ls: path too long\n");
      நிறுத்து;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    போது (பட(fd, &de, அளவு(de)) == அளவு(de)) {
      எனில் (de.inum == 0)
        தொடர்;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      எனில் (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        தொடர்;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (முழு)st.size);
    }
    நிறுத்து;
  }
  close(fd);
}

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  முழு i;

  எனில் (argc < 2) {
    ls(".");
    exit(0);
  }
  க்கு (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
