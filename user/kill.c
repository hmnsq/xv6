#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv)
{
  પૂર્ણાંક i;

  જો (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  માટે (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
