#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

صحيح
main(صحيح argc, حرف **argv)
{
  صحيح i;

  ܐܢ (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  لكل (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
