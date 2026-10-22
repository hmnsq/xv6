// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"

#నిర్వచించు N 1000

శూన్య
print(స్థిరరాశి అక్షరం *s)
{
  లిఖ(1, s, అక్షరశ్రేణి_పొడవు(s));
}

శూన్య
forktest(శూన్య)
{
  పూర్ణాంకం n, pid;

  print("fork test\n");

  కొరకు (n = 0; n < N; n++) {
    pid = fork();
    ఒకవేళ (pid < 0)
      ఆపు;
    ఒకవేళ (pid == 0)
      నిష్క్రమించు(0);
  }

  ఒకవేళ (n == N) {
    print("fork claimed to work N times!\n");
    నిష్క్రమించు(1);
  }

  కొరకు (; n > 0; n--) {
    ఒకవేళ (wait(0) < 0) {
      print("wait stopped early\n");
      నిష్క్రమించు(1);
    }
  }

  ఒకవేళ (wait(0) != -1) {
    print("wait got too many\n");
    నిష్క్రమించు(1);
  }

  print("fork test OK\n");
}

పూర్ణాంకం
ప్రధాన(శూన్య)
{
  forktest();
  నిష్క్రమించు(0);
}
