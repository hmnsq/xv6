#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

استورد argparse, os, inspect, re, signal, subprocess, sys, وخت
من subprocess استورد پرلپسې

sys.stdout.reconfigure(کرښه_buffering=ریښتیا)

parser = argparse.دلیل_Parser()
parser.جمع_کړه_دلیل('testrex', help="test name or regular expression")
parser.جمع_کړه_دلیل("-q", action='store_true', help="usertests quick")
دلیلونه = parser.parse_دلیلونه()

صنف QEMU(object):

    تعریف __init__(self, reset=خطأ):
        إذا reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.سېټ_blocking(self.proc.stdout.fileno(), خطأ)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        وخت.نوم(1)

    تعریف reset_fs(self):
        حاول:
            پرلپسې(["rm", "-f", "fs.img"], check=ریښتیا)
            پرلپسې(["make", "fs.img"], check=ریښتیا)
        استثناء subprocess.CalledProcessError كـ e:
            چاپ(f"Command failed with exit code {e.returncode}")

    تعریف build_xv6(self):
        حاول:
            پرلپسې(["make", "kernel/kernel"], check=ریښتیا)
        استثناء subprocess.CalledProcessError كـ e:
            چاپ(f"Command failed with exit code {e.returncode}")

    تعریف save_output(self):
      حاول:
        سره open("test-xv6.out", "w") كـ f:
            f.اكتب(self.output)
            f.close()
      استثناء OSError كـ e:
        چاپ("Provided a bad results path. Error:", e)     
        
    تعریف cmd(self, c):
        إذا isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.اكتب(c)
        self.proc.stdin.flush()
        
    تعریف crash(self):
        ps = پرلپسې(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [صحيح(کرښه) لكل کرښه في ps.stdout.splitlines()]
        إذا اوږدوالی(kids) == 0:
            چاپ("no qemu")
            sys.exit(1)
        چاپ("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    تعریف stop(self):
        self.proc.terminate()

    تعریف اقرأ(self):
        طالما ریښتیا:
            حاول:
                buf = os.اقرأ(self.proc.stdout.fileno(), 4096)
            استثناء BlockingIOError:
                اكسر
            إذا اوږدوالی(buf) == 0:  # qemu exited
                اكسر
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    تعریف کرښې(self):
        أرجع self.output.splitlines()

    تعریف error(self, *regexps):
        چاپ("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    تعریف سمون(self, *regexps, exit=ریښتیا):
        وموندل = خطأ
        لكل کرښه في self.کرښې():
            إذا any(re.سمون(r, کرښه) لكل r في regexps):
                چاپ(کرښه)
                وموندل = ریښتیا
        إذا ليس وموندل و exit:
            self.error(*regexps)
        أرجع وموندل

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    تعریف پرمختګ(self, regexp):
        پای_ټکی = self.output.rfind("\n") + 1
        إذا پای_ټکی <= self.reported:
            أرجع
        لكل کرښه في self.output[self.reported:پای_ټکی].splitlines():
            إذا re.سمون(regexp, کرښه):
                چاپ(کرښه)
        self.reported = پای_ټکی

    تعریف monitor(self, *regexps, پرمختګ="", timeout):
        deadline = وخت.وخت() + timeout
        طالما ریښتیا:
            وخت.نوم(1)
            timeleft = deadline - وخت.وخت()
            إذا timeleft < 0:
                self.error(*regexps)
            self.اقرأ()
            إذا پرمختګ:
                self.پرمختګ(پرمختګ)
            إذا self.سمون(*regexps, exit=خطأ):
                أرجع

تعریف crash_log():
    q = QEMU(ریښتیا)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    وخت.نوم(2)
    q.crash()
    q.stop()

تعریف recover_log():
    q = QEMU()
    وخت.نوم(2)
    q.اقرأ()
    ok = q.سمون('^recovering', exit=خطأ)
    إذا ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    أرجع ok

تعریف forphan():
    q = QEMU(ریښتیا)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

تعریف dorphan():
    q = QEMU(ریښتیا)
    q.cmd("dorphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

تعریف recover_orphan():
    q = QEMU()
    q.monitor('^ireclaim', timeout=30)
    q.stop()

تعریف test_log():
    چاپ("Test recovery of log")
    لكل i في لړ(20):
        crash_log()
        ok = recover_log()
        إذا ok:
            چاپ("OK")
            أرجع
        چاپ("log attempt ", i+1)
    چاپ("FAIL")
    sys.exit(1)
    
تعریف test_forphan():
    چاپ("Test recovery of an orphaned file")
    forphan()
    recover_orphan()
    چاپ("OK")

تعریف test_dorphan():
    چاپ("Test recovery of an orphaned file")
    dorphan()
    recover_orphan()
    چاپ("OK")

تعریف test_crash():
    test_log()
    test_forphan()
    test_dorphan()

تعریف test_usertests(test=""):
    timeout = 600
    opt = ""
    إذا دلیلونه.q:
        opt = " -q"
        timeout = 300
    وإلاإذا test != "":
        opt += " " + test
    q = QEMU(ریښتیا)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', پرمختګ='test', timeout=timeout)
    q.stop()

تعریف اصلي():
    چاپ(دلیلونه)
    rex = r'%s' % دلیلونه.testrex
    funcs = [(obj,name) لكل name,obj في inspect.getmembers(sys.modules[__name__]) 
                     إذا (inspect.isfunction(obj) و 
                         name.startswith('test'))]
    none = ریښتیا
    لكل (f,n) في funcs:
        إذا re.پلټنه(rex, n):
            none = خطأ
            f()
    إذا none:
        test_usertests(test=دلیلونه.testrex)

اصلي()
