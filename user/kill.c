#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, বর্ণ **argv)
{
  পূর্ণাঙ্ক i;

  যদি (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  প্রতি (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
