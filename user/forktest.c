// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

#define N 1000

शून्य
print(ध्रुव वर्ण *s)
{
  लिख(1, s, म_माप(s));
}

शून्य
forktest(शून्य)
{
  पूर्णाङ्क n, pid;

  print("fork test\n");

  प्रति (n = 0; n < N; n++) {
    pid = विभाजन();
    यदि (pid < 0)
      भङ्ग;
    यदि (pid == 0)
      निकास(0);
  }

  यदि (n == N) {
    print("fork claimed to work N times!\n");
    निकास(1);
  }

  प्रति (; n > 0; n--) {
    यदि (रुको(0) < 0) {
      print("wait stopped early\n");
      निकास(1);
    }
  }

  यदि (रुको(0) != -1) {
    print("wait got too many\n");
    निकास(1);
  }

  print("fork test OK\n");
}

पूर्णाङ्क
main(शून्य)
{
  forktest();
  निकास(0);
}
