#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

целое
основной(целое argc, char *argv[])
{
  sync();
  exit(0);
}
