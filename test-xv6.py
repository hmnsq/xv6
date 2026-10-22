#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

استورد argparse, os, inspect, re, signal, subprocess, sys, زمان
من subprocess استورد پیاپی

sys.stdout.reconfigure(سطر_buffering=درست)

parser = argparse.آرگومان_Parser()
parser.جمع_کن_آرگومان('testrex', help="test name or regular expression")
parser.جمع_کن_آرگومان("-q", action='store_true', help="usertests quick")
آرگومانها = parser.parse_آرگومانها()

صنف QEMU(object):

    تعریف __init__(self, reset=خطأ):
        إذا reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.مجموعه_blocking(self.proc.stdout.fileno(), خطأ)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        زمان.نوم(1)

    تعریف reset_fs(self):
        حاول:
            پیاپی(["rm", "-f", "fs.img"], check=درست)
            پیاپی(["make", "fs.img"], check=درست)
        استثناء subprocess.CalledProcessError كـ e:
            چاپ(f"Command failed with exit code {e.returncode}")

    تعریف build_xv6(self):
        حاول:
            پیاپی(["make", "kernel/kernel"], check=درست)
        استثناء subprocess.CalledProcessError كـ e:
            چاپ(f"Command failed with exit code {e.returncode}")

    تعریف save_output(self):
      حاول:
        با open("test-xv6.out", "w") كـ f:
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
        ps = پیاپی(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [صحيح(سطر) لكل سطر في ps.stdout.splitlines()]
        إذا طول(kids) == 0:
            چاپ("no qemu")
            sys.exit(1)
        چاپ("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    تعریف stop(self):
        self.proc.terminate()

    تعریف اقرأ(self):
        طالما درست:
            حاول:
                buf = os.اقرأ(self.proc.stdout.fileno(), 4096)
            استثناء BlockingIOError:
                اكسر
            إذا طول(buf) == 0:  # qemu exited
                اكسر
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    تعریف سطرها(self):
        أرجع self.output.splitlines()

    تعریف error(self, *regexps):
        چاپ("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    تعریف تطابق(self, *regexps, exit=درست):
        یافت = خطأ
        لكل سطر في self.سطرها():
            إذا any(re.تطابق(r, سطر) لكل r في regexps):
                چاپ(سطر)
                یافت = درست
        إذا ليس یافت و exit:
            self.error(*regexps)
        أرجع یافت

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    تعریف پیشرفت(self, regexp):
        انتها = self.output.rfind("\n") + 1
        إذا انتها <= self.reported:
            أرجع
        لكل سطر في self.output[self.reported:انتها].splitlines():
            إذا re.تطابق(regexp, سطر):
                چاپ(سطر)
        self.reported = انتها

    تعریف monitor(self, *regexps, پیشرفت="", timeout):
        deadline = زمان.زمان() + timeout
        طالما درست:
            زمان.نوم(1)
            timeleft = deadline - زمان.زمان()
            إذا timeleft < 0:
                self.error(*regexps)
            self.اقرأ()
            إذا پیشرفت:
                self.پیشرفت(پیشرفت)
            إذا self.تطابق(*regexps, exit=خطأ):
                أرجع

تعریف crash_log():
    q = QEMU(درست)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    زمان.نوم(2)
    q.crash()
    q.stop()

تعریف recover_log():
    q = QEMU()
    زمان.نوم(2)
    q.اقرأ()
    ok = q.تطابق('^recovering', exit=خطأ)
    إذا ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    أرجع ok

تعریف forphan():
    q = QEMU(درست)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

تعریف dorphan():
    q = QEMU(درست)
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
    لكل i في دامنه(20):
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
    إذا آرگومانها.q:
        opt = " -q"
        timeout = 300
    وإلاإذا test != "":
        opt += " " + test
    q = QEMU(درست)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', پیشرفت='test', timeout=timeout)
    q.stop()

تعریف اصلی():
    چاپ(آرگومانها)
    rex = r'%s' % آرگومانها.testrex
    funcs = [(obj,name) لكل name,obj في inspect.getmembers(sys.modules[__name__]) 
                     إذا (inspect.isfunction(obj) و 
                         name.startswith('test'))]
    none = درست
    لكل (f,n) في funcs:
        إذا re.جستجو(rex, n):
            none = خطأ
            f()
    إذا none:
        test_usertests(test=آرگومانها.testrex)

اصلی()
