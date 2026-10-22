#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/fs.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"

ಅಕ್ಷರ *
fmtname(ಅಕ್ಷರ *ಮಾರ್ಗ)
{
  ಸ್ಥಿರ ಅಕ್ಷರ buf[DIRSIZ + 1];
  ಅಕ್ಷರ *p;

  // Find first character after last slash.
  ಗಾಗಿ (p = ಮಾರ್ಗ + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ); p >= ಮಾರ್ಗ && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p) >= DIRSIZ)
    ಹಿಂತಿರುಗಿ p;
  memmove(buf, p, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p));
  memset(buf + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p), ' ', DIRSIZ - ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p));
  buf[ಗಾತ್ರ(buf) - 1] = '\0';
  ಹಿಂತಿರುಗಿ buf;
}

ಶೂನ್ಯ
ls(ಅಕ್ಷರ *ಮಾರ್ಗ)
{
  ಅಕ್ಷರ buf[512], *p;
  ಪೂರ್ಣಾಂಕ fd;
  ರಚನೆ dirent de;
  ರಚನೆ stat st;

  ಆದರೆ ((fd = open(ಮಾರ್ಗ, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", ಮಾರ್ಗ);
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", ಮಾರ್ಗ);
    close(fd);
    ಹಿಂತಿರುಗಿ;
  }

  ಆಯ್ಕೆ (st.type) {
  ಸ್ಥಿತಿ T_DEVICE:
  ಸ್ಥಿತಿ T_FILE:
    ಬರೆ("%s %d %d %d\n", fmtname(ಮಾರ್ಗ), st.type, st.ino, (ಪೂರ್ಣಾಂಕ)st.ಗಾತ್ರ_2);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ T_DIR:
    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ) + 1 + DIRSIZ + 1 > ಗಾತ್ರ buf) {
      ಬರೆ("ls: path too long\n");
      ನಿಲ್ಲಿಸು;
    }
    strcpy(buf, ಮಾರ್ಗ);
    p = buf + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf);
    *p++ = '/';
    ಆಗ (ಪಠ(fd, &de, ಗಾತ್ರ(de)) == ಗಾತ್ರ(de)) {
      ಆದರೆ (de.inum == 0)
        ಮುಂದುವರಿ;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      ಆದರೆ (stat(buf, &st) < 0) {
        ಬರೆ("ls: cannot stat %s\n", buf);
        ಮುಂದುವರಿ;
      }
      ಬರೆ("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (ಪೂರ್ಣಾಂಕ)st.ಗಾತ್ರ_2);
    }
    ನಿಲ್ಲಿಸು;
  }
  close(fd);
}

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
  ಪೂರ್ಣಾಂಕ i;

  ಆದರೆ (argc < 2) {
    ls(".");
    ನಿರ್ಗಮಿಸು(0);
  }
  ಗಾಗಿ (i = 1; i < argc; i++)
    ls(argv[i]);
  ನಿರ್ಗಮಿಸು(0);
}
