#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "user/user.h"

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
{
  পূর্ণাঙ্ক i;

  বাবে (i = 1; i < argc; i++) {
    লিখ(1, argv[i], strlen(argv[i]));
    যদি (i + 1 < argc) {
      লিখ(1, " ", 1);
    } অন্যথা {
      লিখ(1, "\n", 1);
    }
  }
  exit(0);
}
