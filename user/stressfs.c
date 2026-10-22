// Demonstrate that moving the "acquire" in iderw after the loop that
// appends to the idequeue results in a race.

// For this to work, you should also add a spin within iderw's
// idequeue traversal loop.  Adding the following demonstrated a panic
// after about 5 runs of stressfs in QEMU on a 2.1GHz CPU:
//    for (i = 0; i < 40000; i++)
//      asm volatile("");

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/fcntl.h"

पूर्णाङ्क
मुख्यम्(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क fd, i;
  वर्ण मार्गः[] = "stressfs0";
  वर्ण दत्त[512];

  प्रारूप_लिख("stressfs starting\n");
  स्मृतिं_पूरय(दत्त, 'a', परिमाण(दत्त));

  प्रति (i = 0; i < 4; i++)
    यदि (fork() > 0)
      भङ्ग;

  प्रारूप_लिख("write %d\n", i);

  मार्गः[8] += i;
  fd = उद्घाटय(मार्गः, O_CREATE | O_RDWR);
  प्रति (i = 0; i < 20; i++) {
    // printf(fd, "%d\n", i);
    लिख(fd, दत्त, परिमाण(दत्त));
  }
  close(fd);

  प्रारूप_लिख("read\n");

  fd = उद्घाटय(मार्गः, O_RDONLY);
  प्रति (i = 0; i < 20; i++)
    पठ(fd, दत्त, परिमाण(दत्त));
  close(fd);

  प्रतीक्षस्व(0);

  निर्गच्छ(0);
}
