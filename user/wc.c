#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "kernel/fcntl.h"
#അന്തര്ഭാവ "user/user.h"

അക്ഷരം buf[512];

ശൂന്യം
wc(പൂർണ്ണസംഖ്യ fd, അക്ഷരം *name)
{
  പൂർണ്ണസംഖ്യ i, n;
  പൂർണ്ണസംഖ്യ l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  ആകുമ്പോൾ ((n = പഠ(fd, buf, വലുപ്പം(buf))) > 0) {
    വേണ്ടി (i = 0; i < n; i++) {
      c++;
      എങ്കിൽ (buf[i] == '\n')
        l++;
      എങ്കിൽ (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      അല്ലെങ്കിൽ എങ്കിൽ (!inword) {
        w++;
        inword = 1;
      }
    }
  }
  എങ്കിൽ (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }
  printf("%d %d %d %s\n", l, w, c, name);
}

പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
  പൂർണ്ണസംഖ്യ fd, i;

  എങ്കിൽ (argc <= 1) {
    wc(0, "");
    exit(0);
  }

  വേണ്ടി (i = 1; i < argc; i++) {
    എങ്കിൽ ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i]);
    close(fd);
  }
  exit(0);
}
