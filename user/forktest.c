// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

#परिभाषय N 1000

शून्य
print(ध्रुव वर्ण *s)
{
  लिख(1, s, वाक्यदैर्घ्यम्(s));
}

शून्य
forktest(शून्य)
{
  पूर्णाङ्क n, pid;

  print("fork test\n");

  प्रति (n = 0; n < N; n++) {
    pid = fork();
    यदि (pid < 0)
      भङ्ग;
    यदि (pid == 0)
      निर्गच्छ(0);
  }

  यदि (n == N) {
    print("fork claimed to work N times!\n");
    निर्गच्छ(1);
  }

  प्रति (; n > 0; n--) {
    यदि (प्रतीक्षस्व(0) < 0) {
      print("wait stopped early\n");
      निर्गच्छ(1);
    }
  }

  यदि (प्रतीक्षस्व(0) != -1) {
    print("wait got too many\n");
    निर्गच्छ(1);
  }

  print("fork test OK\n");
}

पूर्णाङ्क
मुख्यम्(शून्य)
{
  forktest();
  निर्गच्छ(0);
}
