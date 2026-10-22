// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "user/user.h"

#घोषणा N 1000

व्योम
print(स्थिर अक्षर *s)
{
  ग_लिखो(1, s, म_माप(s));
}

व्योम
forktest(व्योम)
{
  पूर्णांक n, pid;

  print("fork test\n");

  क्रम (n = 0; n < N; n++) {
    pid = विभाजन();
    अगर (pid < 0)
      अवरोध;
    अगर (pid == 0)
      निकास(0);
  }

  अगर (n == N) {
    print("fork claimed to work N times!\n");
    निकास(1);
  }

  क्रम (; n > 0; n--) {
    अगर (रुको(0) < 0) {
      print("wait stopped early\n");
      निकास(1);
    }
  }

  अगर (रुको(0) != -1) {
    print("wait got too many\n");
    निकास(1);
  }

  print("fork test OK\n");
}

पूर्णांक
मुख्य(व्योम)
{
  forktest();
  निकास(0);
}
