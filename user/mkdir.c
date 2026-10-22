#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
  પૂર્ણાંક i;

  જો (argc < 2) {
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  માટે (i = 1; i < argc; i++) {
    જો (mkdir(argv[i]) < 0) {
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      તોડો;
    }
  }

  exit(0);
}
