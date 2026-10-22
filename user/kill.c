#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "user/user.h"

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, ৱর্ণ **argv)
{
  পূর্ণাঙ্ক i;

  যদি (argc < 2) {
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  বাবে (i = 1; i < argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}
