#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

整数
主要(整数 argc, char *argv[])
{
  sync();
  exit(0);
}
