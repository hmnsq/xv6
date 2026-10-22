#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

استورد argparse, os, inspect, re, signal, subprocess, sys, وقت
من subprocess استورد لاڳيتو

sys.stdout.reconfigure(سٽ_buffering=سچ)

parser = argparse.دليل_Parser()
parser.جوڙيو_دليل('testrex', help="test name or regular expression")
parser.جوڙيو_دليل("-q", action='store_true', help="usertests quick")
دليلون = parser.parse_دليلون()

صنف QEMU(object):

    وصف __init__(self, reset=خطأ):
        إذا reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.سيٽ_blocking(self.proc.stdout.fileno(), خطأ)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        وقت.نوم(1)

    وصف reset_fs(self):
        حاول:
            لاڳيتو(["rm", "-f", "fs.img"], check=سچ)
            لاڳيتو(["make", "fs.img"], check=سچ)
        استثناء subprocess.CalledProcessError كـ e:
            ڇاپيو(f"Command failed with exit code {e.returncode}")

    وصف build_xv6(self):
        حاول:
            لاڳيتو(["make", "kernel/kernel"], check=سچ)
        استثناء subprocess.CalledProcessError كـ e:
            ڇاپيو(f"Command failed with exit code {e.returncode}")

    وصف save_output(self):
      حاول:
        سان open("test-xv6.out", "w") كـ f:
            f.اكتب(self.output)
            f.close()
      استثناء OSError كـ e:
        ڇاپيو("Provided a bad results path. Error:", e)     
        
    وصف cmd(self, c):
        إذا isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.اكتب(c)
        self.proc.stdin.flush()
        
    وصف crash(self):
        ps = لاڳيتو(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [صحيح(سٽ) لكل سٽ في ps.stdout.splitlines()]
        إذا ڊيگهه(kids) == 0:
            ڇاپيو("no qemu")
            sys.exit(1)
        ڇاپيو("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    وصف stop(self):
        self.proc.terminate()

    وصف اقرأ(self):
        طالما سچ:
            حاول:
                buf = os.اقرأ(self.proc.stdout.fileno(), 4096)
            استثناء BlockingIOError:
                اكسر
            إذا ڊيگهه(buf) == 0:  # qemu exited
                اكسر
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    وصف سٽون(self):
        أرجع self.output.splitlines()

    وصف error(self, *regexps):
        ڇاپيو("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    وصف ميلاپ(self, *regexps, exit=سچ):
        مليو = خطأ
        لكل سٽ في self.سٽون():
            إذا any(re.ميلاپ(r, سٽ) لكل r في regexps):
                ڇاپيو(سٽ)
                مليو = سچ
        إذا ليس مليو و exit:
            self.error(*regexps)
        أرجع مليو

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    وصف ترقي(self, regexp):
        پڇاڙي = self.output.rfind("\n") + 1
        إذا پڇاڙي <= self.reported:
            أرجع
        لكل سٽ في self.output[self.reported:پڇاڙي].splitlines():
            إذا re.ميلاپ(regexp, سٽ):
                ڇاپيو(سٽ)
        self.reported = پڇاڙي

    وصف monitor(self, *regexps, ترقي="", timeout):
        deadline = وقت.وقت() + timeout
        طالما سچ:
            وقت.نوم(1)
            timeleft = deadline - وقت.وقت()
            إذا timeleft < 0:
                self.error(*regexps)
            self.اقرأ()
            إذا ترقي:
                self.ترقي(ترقي)
            إذا self.ميلاپ(*regexps, exit=خطأ):
                أرجع

وصف crash_log():
    q = QEMU(سچ)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    وقت.نوم(2)
    q.crash()
    q.stop()

وصف recover_log():
    q = QEMU()
    وقت.نوم(2)
    q.اقرأ()
    ok = q.ميلاپ('^recovering', exit=خطأ)
    إذا ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    أرجع ok

وصف forphan():
    q = QEMU(سچ)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

وصف dorphan():
    q = QEMU(سچ)
    q.cmd("dorphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

وصف recover_orphan():
    q = QEMU()
    q.monitor('^ireclaim', timeout=30)
    q.stop()

وصف test_log():
    ڇاپيو("Test recovery of log")
    لكل i في سلسلو(20):
        crash_log()
        ok = recover_log()
        إذا ok:
            ڇاپيو("OK")
            أرجع
        ڇاپيو("log attempt ", i+1)
    ڇاپيو("FAIL")
    sys.exit(1)
    
وصف test_forphan():
    ڇاپيو("Test recovery of an orphaned file")
    forphan()
    recover_orphan()
    ڇاپيو("OK")

وصف test_dorphan():
    ڇاپيو("Test recovery of an orphaned file")
    dorphan()
    recover_orphan()
    ڇاپيو("OK")

وصف test_crash():
    test_log()
    test_forphan()
    test_dorphan()

وصف test_usertests(test=""):
    timeout = 600
    opt = ""
    إذا دليلون.q:
        opt = " -q"
        timeout = 300
    وإلاإذا test != "":
        opt += " " + test
    q = QEMU(سچ)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', ترقي='test', timeout=timeout)
    q.stop()

وصف مکيه():
    ڇاپيو(دليلون)
    rex = r'%s' % دليلون.testrex
    funcs = [(obj,name) لكل name,obj في inspect.getmembers(sys.modules[__name__]) 
                     إذا (inspect.isfunction(obj) و 
                         name.startswith('test'))]
    none = سچ
    لكل (f,n) في funcs:
        إذا re.ڳولا(rex, n):
            none = خطأ
            f()
    إذا none:
        test_usertests(test=دليلون.testrex)

مکيه()
