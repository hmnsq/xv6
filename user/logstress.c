#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "kernel/fcntl.h"
#അന്തര്ഭാവ "user/user.h"

// Stress xv6 logging system by having several processes writing
// concurrently to their own file (e.g., logstress f1 f2 f3 f4)

#define BUFSZ 500

അക്ഷരം buf[BUFSZ];

പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv)
{
  പൂർണ്ണസംഖ്യ fd, n;
  എണ്ണൽ { N = 250, SZ = 2000 };

  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < argc; i++) {
    പൂർണ്ണസംഖ്യ pid1 = fork();
    എങ്കിൽ (pid1 < 0) {
      printf("%s: fork failed\n", argv[0]);
      exit(1);
    }
    എങ്കിൽ (pid1 == 0) {
      fd = open(argv[i], O_CREATE | O_RDWR);
      എങ്കിൽ (fd < 0) {
        printf("%s: create %s failed\n", argv[0], argv[i]);
        exit(1);
      }
      memset(buf, '0' + i, SZ);
      വേണ്ടി (i = 0; i < N; i++) {
        എങ്കിൽ ((n = ലിഖ(fd, buf, SZ)) != SZ) {
          printf("write failed %d\n", n);
          exit(1);
        }
      }
      exit(0);
    }
  }
  പൂർണ്ണസംഖ്യ xstatus;
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < argc; i++) {
    wait(&xstatus);
    എങ്കിൽ (xstatus != 0)
      exit(xstatus);
  }
  തിരികെ 0;
}
