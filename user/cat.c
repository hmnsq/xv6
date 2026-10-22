#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/fcntl.h"
#അന്തര്ഭാവ "user/user.h"

അക്ഷരം buf[512];

ശൂന്യം
cat(പൂർണ്ണസംഖ്യ fd)
{
  പൂർണ്ണസംഖ്യ n;

  ആകുമ്പോൾ ((n = പഠ(fd, buf, വലുപ്പം(buf))) > 0) {
    എങ്കിൽ (ലിഖ(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  എങ്കിൽ (n < 0) {
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
  പൂർണ്ണസംഖ്യ fd, i;

  എങ്കിൽ (argc <= 1) {
    cat(0);
    exit(0);
  }

  വേണ്ടി (i = 1; i < argc; i++) {
    എങ്കിൽ ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
