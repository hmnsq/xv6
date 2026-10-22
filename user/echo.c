#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
  પૂર્ણાંક i;

  માટે (i = 1; i < argc; i++) {
    લિખ(1, argv[i], strlen(argv[i]));
    જો (i + 1 < argc) {
      લિખ(1, " ", 1);
    } નહીંતર {
      લિખ(1, "\n", 1);
    }
  }
  exit(0);
}
