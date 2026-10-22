#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

استورد argparse, os, inspect, re, signal, subprocess, sys, ویلا
من subprocess استورد لگاتار

sys.stdout.reconfigure(سطر_buffering=سچ)

parser = argparse.دلیل_Parser()
parser.جوڑو_دلیل('testrex', help="test name or regular expression")
parser.جوڑو_دلیل("-q", action='store_true', help="usertests quick")
دلیلاں = parser.parse_دلیلاں()

صنف QEMU(object):

    تعریف __init__(self, reset=خطأ):
        إذا reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.سیٹ_blocking(self.proc.stdout.fileno(), خطأ)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        ویلا.نوم(1)

    تعریف reset_fs(self):
        حاول:
            لگاتار(["rm", "-f", "fs.img"], check=سچ)
            لگاتار(["make", "fs.img"], check=سچ)
        استثناء subprocess.CalledProcessError كـ e:
            چھاپو(f"Command failed with exit code {e.returncode}")

    تعریف build_xv6(self):
        حاول:
            لگاتار(["make", "kernel/kernel"], check=سچ)
        استثناء subprocess.CalledProcessError كـ e:
            چھاپو(f"Command failed with exit code {e.returncode}")

    تعریف save_output(self):
      حاول:
        نال open("test-xv6.out", "w") كـ f:
            f.اكتب(self.output)
            f.close()
      استثناء OSError كـ e:
        چھاپو("Provided a bad results path. Error:", e)     
        
    تعریف cmd(self, c):
        إذا isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.اكتب(c)
        self.proc.stdin.flush()
        
    تعریف crash(self):
        ps = لگاتار(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [صحيح(سطر) لكل سطر في ps.stdout.splitlines()]
        إذا لمبائی(kids) == 0:
            چھاپو("no qemu")
            sys.exit(1)
        چھاپو("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    تعریف stop(self):
        self.proc.terminate()

    تعریف اقرأ(self):
        طالما سچ:
            حاول:
                buf = os.اقرأ(self.proc.stdout.fileno(), 4096)
            استثناء BlockingIOError:
                اكسر
            إذا لمبائی(buf) == 0:  # qemu exited
                اكسر
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    تعریف سطریں(self):
        أرجع self.output.splitlines()

    تعریف error(self, *regexps):
        چھاپو("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    تعریف میل(self, *regexps, exit=سچ):
        لبھیا = خطأ
        لكل سطر في self.سطریں():
            إذا any(re.میل(r, سطر) لكل r في regexps):
                چھاپو(سطر)
                لبھیا = سچ
        إذا ليس لبھیا و exit:
            self.error(*regexps)
        أرجع لبھیا

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    تعریف ترقی(self, regexp):
        سرا = self.output.rfind("\n") + 1
        إذا سرا <= self.reported:
            أرجع
        لكل سطر في self.output[self.reported:سرا].splitlines():
            إذا re.میل(regexp, سطر):
                چھاپو(سطر)
        self.reported = سرا

    تعریف monitor(self, *regexps, ترقی="", timeout):
        deadline = ویلا.ویلا() + timeout
        طالما سچ:
            ویلا.نوم(1)
            timeleft = deadline - ویلا.ویلا()
            إذا timeleft < 0:
                self.error(*regexps)
            self.اقرأ()
            إذا ترقی:
                self.ترقی(ترقی)
            إذا self.میل(*regexps, exit=خطأ):
                أرجع

تعریف crash_log():
    q = QEMU(سچ)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    ویلا.نوم(2)
    q.crash()
    q.stop()

تعریف recover_log():
    q = QEMU()
    ویلا.نوم(2)
    q.اقرأ()
    ok = q.میل('^recovering', exit=خطأ)
    إذا ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    أرجع ok

تعریف forphan():
    q = QEMU(سچ)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

تعریف dorphan():
    q = QEMU(سچ)
    q.cmd("dorphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

تعریف recover_orphan():
    q = QEMU()
    q.monitor('^ireclaim', timeout=30)
    q.stop()

تعریف test_log():
    چھاپو("Test recovery of log")
    لكل i في سلسلہ(20):
        crash_log()
        ok = recover_log()
        إذا ok:
            چھاپو("OK")
            أرجع
        چھاپو("log attempt ", i+1)
    چھاپو("FAIL")
    sys.exit(1)
    
تعریف test_forphan():
    چھاپو("Test recovery of an orphaned file")
    forphan()
    recover_orphan()
    چھاپو("OK")

تعریف test_dorphan():
    چھاپو("Test recovery of an orphaned file")
    dorphan()
    recover_orphan()
    چھاپو("OK")

تعریف test_crash():
    test_log()
    test_forphan()
    test_dorphan()

تعریف test_usertests(test=""):
    timeout = 600
    opt = ""
    إذا دلیلاں.q:
        opt = " -q"
        timeout = 300
    وإلاإذا test != "":
        opt += " " + test
    q = QEMU(سچ)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', ترقی='test', timeout=timeout)
    q.stop()

تعریف مکھ():
    چھاپو(دلیلاں)
    rex = r'%s' % دلیلاں.testrex
    funcs = [(obj,name) لكل name,obj في inspect.getmembers(sys.modules[__name__]) 
                     إذا (inspect.isfunction(obj) و 
                         name.startswith('test'))]
    none = سچ
    لكل (f,n) في funcs:
        إذا re.تلاش(rex, n):
            none = خطأ
            f()
    إذا none:
        test_usertests(test=دلیلاں.testrex)

مکھ()
