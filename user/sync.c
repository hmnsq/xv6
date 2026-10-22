#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

정수
main(정수 argc, char *argv[])
{
  sync();
  exit(0);
}
