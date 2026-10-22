#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

ಆಮದು argparse, os, inspect, re, signal, subprocess, sys, ಸಮಯ
ತಃ subprocess ಆಮದು ಸತತ

sys.stdout.reconfigure(ಸಾಲು_buffering=ನಿಜ)

parser = argparse.ವಾದ_Parser()
parser.ಕೂಡಿಸು_ವಾದ('testrex', help="test name or regular expression")
parser.ಕೂಡಿಸು_ವಾದ("-q", action='store_true', help="usertests quick")
ವಾದಗಳು = parser.parse_ವಾದಗಳು()

ವರ್ಗ QEMU(object):

    ವ್ಯಾಖ್ಯಾನ __init__(self, reset=ಸುಳ್ಳು):
        ಆದರೆ reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.ಗಣ_blocking(self.proc.stdout.fileno(), ಸುಳ್ಳು)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        ಸಮಯ.ಸುಷುಪ್ತಿ(1)

    ವ್ಯಾಖ್ಯಾನ reset_fs(self):
        ಪ್ರಯತ್ನಿಸು:
            ಸತತ(["rm", "-f", "fs.img"], check=ನಿಜ)
            ಸತತ(["make", "fs.img"], check=ನಿಜ)
        ಅಪವಾದ subprocess.CalledProcessError ಇವ e:
            ಮುದ್ರಿಸು(f"Command failed with exit code {e.returncode}")

    ವ್ಯಾಖ್ಯಾನ build_xv6(self):
        ಪ್ರಯತ್ನಿಸು:
            ಸತತ(["make", "kernel/kernel"], check=ನಿಜ)
        ಅಪವಾದ subprocess.CalledProcessError ಇವ e:
            ಮುದ್ರಿಸು(f"Command failed with exit code {e.returncode}")

    ವ್ಯಾಖ್ಯಾನ save_output(self):
      ಪ್ರಯತ್ನಿಸು:
        ಜೊತೆ open("test-xv6.out", "w") ಇವ f:
            f.ಲಿಖ(self.output)
            f.close()
      ಅಪವಾದ OSError ಇವ e:
        ಮುದ್ರಿಸು("Provided a bad results path. Error:", e)     
        
    ವ್ಯಾಖ್ಯಾನ cmd(self, c):
        ಆದರೆ isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.ಲಿಖ(c)
        self.proc.stdin.flush()
        
    ವ್ಯಾಖ್ಯಾನ crash(self):
        ps = ಸತತ(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [ಪೂರ್ಣಾಂಕ(ಸಾಲು) ಗಾಗಿ ಸಾಲು ಒಳಗೆ ps.stdout.splitlines()]
        ಆದರೆ ಉದ್ದ(kids) == 0:
            ಮುದ್ರಿಸು("no qemu")
            sys.exit(1)
        ಮುದ್ರಿಸು("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    ವ್ಯಾಖ್ಯಾನ stop(self):
        self.proc.terminate()

    ವ್ಯಾಖ್ಯಾನ ಪಠ(self):
        ಆಗ ನಿಜ:
            ಪ್ರಯತ್ನಿಸು:
                buf = os.ಪಠ(self.proc.stdout.fileno(), 4096)
            ಅಪವಾದ BlockingIOError:
                ನಿಲ್ಲಿಸು
            ಆದರೆ ಉದ್ದ(buf) == 0:  # qemu exited
                ನಿಲ್ಲಿಸು
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    ವ್ಯಾಖ್ಯಾನ ಸಾಲುಗಳು(self):
        ಹಿಂತಿರುಗಿ self.output.splitlines()

    ವ್ಯಾಖ್ಯಾನ error(self, *regexps):
        ಮುದ್ರಿಸು("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    ವ್ಯಾಖ್ಯಾನ ಹೊಂದಿಕೆ(self, *regexps, exit=ನಿಜ):
        ಸಿಕ್ಕಿತು = ಸುಳ್ಳು
        ಗಾಗಿ ಸಾಲು ಒಳಗೆ self.ಸಾಲುಗಳು():
            ಆದರೆ any(re.ಹೊಂದಿಕೆ(r, ಸಾಲು) ಗಾಗಿ r ಒಳಗೆ regexps):
                ಮುದ್ರಿಸು(ಸಾಲು)
                ಸಿಕ್ಕಿತು = ನಿಜ
        ಆದರೆ ಅಲ್ಲ ಸಿಕ್ಕಿತು ಮತ್ತು exit:
            self.error(*regexps)
        ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    ವ್ಯಾಖ್ಯಾನ ಪ್ರಗತಿ(self, regexp):
        ಕೊನೆ = self.output.rfind("\n") + 1
        ಆದರೆ ಕೊನೆ <= self.reported:
            ಹಿಂತಿರುಗಿ
        ಗಾಗಿ ಸಾಲು ಒಳಗೆ self.output[self.reported:ಕೊನೆ].splitlines():
            ಆದರೆ re.ಹೊಂದಿಕೆ(regexp, ಸಾಲು):
                ಮುದ್ರಿಸು(ಸಾಲು)
        self.reported = ಕೊನೆ

    ವ್ಯಾಖ್ಯಾನ monitor(self, *regexps, ಪ್ರಗತಿ="", timeout):
        deadline = ಸಮಯ.ಸಮಯ() + timeout
        ಆಗ ನಿಜ:
            ಸಮಯ.ಸುಷುಪ್ತಿ(1)
            timeleft = deadline - ಸಮಯ.ಸಮಯ()
            ಆದರೆ timeleft < 0:
                self.error(*regexps)
            self.ಪಠ()
            ಆದರೆ ಪ್ರಗತಿ:
                self.ಪ್ರಗತಿ(ಪ್ರಗತಿ)
            ಆದರೆ self.ಹೊಂದಿಕೆ(*regexps, exit=ಸುಳ್ಳು):
                ಹಿಂತಿರುಗಿ

ವ್ಯಾಖ್ಯಾನ crash_log():
    q = QEMU(ನಿಜ)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    ಸಮಯ.ಸುಷುಪ್ತಿ(2)
    q.crash()
    q.stop()

ವ್ಯಾಖ್ಯಾನ recover_log():
    q = QEMU()
    ಸಮಯ.ಸುಷುಪ್ತಿ(2)
    q.ಪಠ()
    ok = q.ಹೊಂದಿಕೆ('^recovering', exit=ಸುಳ್ಳು)
    ಆದರೆ ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    ಹಿಂತಿರುಗಿ ok

ವ್ಯಾಖ್ಯಾನ forphan():
    q = QEMU(ನಿಜ)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

ವ್ಯಾಖ್ಯಾನ dorphan():
    q = QEMU(ನಿಜ)
    q.cmd("dorphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

ವ್ಯಾಖ್ಯಾನ recover_orphan():
    q = QEMU()
    q.monitor('^ireclaim', timeout=30)
    q.stop()

ವ್ಯಾಖ್ಯಾನ test_log():
    ಮುದ್ರಿಸು("Test recovery of log")
    ಗಾಗಿ i ಒಳಗೆ ಶ್ರೇಣಿ(20):
        crash_log()
        ok = recover_log()
        ಆದರೆ ok:
            ಮುದ್ರಿಸು("OK")
            ಹಿಂತಿರುಗಿ
        ಮುದ್ರಿಸು("log attempt ", i+1)
    ಮುದ್ರಿಸು("FAIL")
    sys.exit(1)
    
ವ್ಯಾಖ್ಯಾನ test_forphan():
    ಮುದ್ರಿಸು("Test recovery of an orphaned file")
    forphan()
    recover_orphan()
    ಮುದ್ರಿಸು("OK")

ವ್ಯಾಖ್ಯಾನ test_dorphan():
    ಮುದ್ರಿಸು("Test recovery of an orphaned file")
    dorphan()
    recover_orphan()
    ಮುದ್ರಿಸು("OK")

ವ್ಯಾಖ್ಯಾನ test_crash():
    test_log()
    test_forphan()
    test_dorphan()

ವ್ಯಾಖ್ಯಾನ test_usertests(test=""):
    timeout = 600
    opt = ""
    ಆದರೆ ವಾದಗಳು.q:
        opt = " -q"
        timeout = 300
    ಅಥವಾಆದರೆ test != "":
        opt += " " + test
    q = QEMU(ನಿಜ)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', ಪ್ರಗತಿ='test', timeout=timeout)
    q.stop()

ವ್ಯಾಖ್ಯಾನ ಮುಖ್ಯ():
    ಮುದ್ರಿಸು(ವಾದಗಳು)
    rex = r'%s' % ವಾದಗಳು.testrex
    funcs = [(obj,name) ಗಾಗಿ name,obj ಒಳಗೆ inspect.getmembers(sys.modules[__name__]) 
                     ಆದರೆ (inspect.isfunction(obj) ಮತ್ತು 
                         name.startswith('test'))]
    none = ನಿಜ
    ಗಾಗಿ (f,n) ಒಳಗೆ funcs:
        ಆದರೆ re.ಹುಡುಕು(rex, n):
            none = ಸುಳ್ಳು
            f()
    ಆದರೆ none:
        test_usertests(test=ವಾದಗಳು.testrex)

ಮುಖ್ಯ()
