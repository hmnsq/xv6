// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

#define N 1000

शून्य
print(ध्रुव वर्ण *s)
{
  लिख(1, s, strlen(s));
}

शून्य
forktest(शून्य)
{
  पूर्णाङ्क n, pid;

  print("fork test\n");

  अत्थाय (n = 0; n < N; n++) {
    pid = fork();
    सचे (pid < 0)
      भङ्ग;
    सचे (pid == 0)
      exit(0);
  }

  सचे (n == N) {
    print("fork claimed to work N times!\n");
    exit(1);
  }

  अत्थाय (; n > 0; n--) {
    सचे (wait(0) < 0) {
      print("wait stopped early\n");
      exit(1);
    }
  }

  सचे (wait(0) != -1) {
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

पूर्णाङ्क
main(शून्य)
{
  forktest();
  exit(0);
}
