#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "user/user.h"

പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
  പൂർണ്ണസംഖ്യ i;

  എങ്കിൽ (argc < 2) {
    fprintf(2, "Usage: rm files...\n");
    exit(1);
  }

  വേണ്ടി (i = 1; i < argc; i++) {
    എങ്കിൽ (unlink(argv[i]) < 0) {
      fprintf(2, "rm: %s failed to delete\n", argv[i]);
      നിർത്തുക;
    }
  }

  exit(0);
}
