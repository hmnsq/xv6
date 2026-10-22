#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

درآمد argparse, os, inspect, re, signal, subprocess, sys, وقت
سے subprocess درآمد تسلسل

sys.stdout.reconfigure(سطر_buffering=سچ)

parser = argparse.دلیل_Parser()
parser.جوڑو_دلیل('testrex', help="test name or regular expression")
parser.جوڑو_دلیل("-q", action='store_true', help="usertests quick")
دلائل = parser.parse_دلائل()

جماعت QEMU(object):

    تعریف __init__(self, reset=جھوٹ):
        اگر reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.سیٹ_blocking(self.proc.stdout.fileno(), جھوٹ)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        وقت.نوم(1)

    تعریف reset_fs(self):
        کوشش:
            تسلسل(["rm", "-f", "fs.img"], check=سچ)
            تسلسل(["make", "fs.img"], check=سچ)
        استثنا subprocess.CalledProcessError بطور e:
            چھاپو(f"Command failed with exit code {e.returncode}")

    تعریف build_xv6(self):
        کوشش:
            تسلسل(["make", "kernel/kernel"], check=سچ)
        استثنا subprocess.CalledProcessError بطور e:
            چھاپو(f"Command failed with exit code {e.returncode}")

    تعریف save_output(self):
      کوشش:
        ساتھ open("test-xv6.out", "w") بطور f:
            f.اكتب(self.output)
            f.close()
      استثنا OSError بطور e:
        چھاپو("Provided a bad results path. Error:", e)     
        
    تعریف cmd(self, c):
        اگر isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.اكتب(c)
        self.proc.stdin.flush()
        
    تعریف crash(self):
        ps = تسلسل(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [صحیح(سطر) برائے سطر میں ps.stdout.splitlines()]
        اگر لمبائی(kids) == 0:
            چھاپو("no qemu")
            sys.exit(1)
        چھاپو("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    تعریف stop(self):
        self.proc.terminate()

    تعریف اقرأ(self):
        جب سچ:
            کوشش:
                buf = os.اقرأ(self.proc.stdout.fileno(), 4096)
            استثنا BlockingIOError:
                توڑو
            اگر لمبائی(buf) == 0:  # qemu exited
                توڑو
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    تعریف سطریں(self):
        واپس self.output.splitlines()

    تعریف error(self, *regexps):
        چھاپو("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    تعریف مطابقت(self, *regexps, exit=سچ):
        ملا = جھوٹ
        برائے سطر میں self.سطریں():
            اگر any(re.مطابقت(r, سطر) برائے r میں regexps):
                چھاپو(سطر)
                ملا = سچ
        اگر نہیں ملا اور exit:
            self.error(*regexps)
        واپس ملا

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    تعریف پیش_رفت(self, regexp):
        انتہا = self.output.rfind("\n") + 1
        اگر انتہا <= self.reported:
            واپس
        برائے سطر میں self.output[self.reported:انتہا].splitlines():
            اگر re.مطابقت(regexp, سطر):
                چھاپو(سطر)
        self.reported = انتہا

    تعریف monitor(self, *regexps, پیش_رفت="", timeout):
        deadline = وقت.وقت() + timeout
        جب سچ:
            وقت.نوم(1)
            timeleft = deadline - وقت.وقت()
            اگر timeleft < 0:
                self.error(*regexps)
            self.اقرأ()
            اگر پیش_رفت:
                self.پیش_رفت(پیش_رفت)
            اگر self.مطابقت(*regexps, exit=جھوٹ):
                واپس

تعریف crash_log():
    q = QEMU(سچ)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    وقت.نوم(2)
    q.crash()
    q.stop()

تعریف recover_log():
    q = QEMU()
    وقت.نوم(2)
    q.اقرأ()
    ok = q.مطابقت('^recovering', exit=جھوٹ)
    اگر ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    واپس ok

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
    برائے i میں سلسلہ(20):
        crash_log()
        ok = recover_log()
        اگر ok:
            چھاپو("OK")
            واپس
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
    اگر دلائل.q:
        opt = " -q"
        timeout = 300
    وگرنہ_اگر test != "":
        opt += " " + test
    q = QEMU(سچ)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', پیش_رفت='test', timeout=timeout)
    q.stop()

تعریف مین():
    چھاپو(دلائل)
    rex = r'%s' % دلائل.testrex
    funcs = [(obj,name) برائے name,obj میں inspect.getmembers(sys.modules[__name__]) 
                     اگر (inspect.isfunction(obj) اور 
                         name.startswith('test'))]
    none = سچ
    برائے (f,n) میں funcs:
        اگر re.تلاش(rex, n):
            none = جھوٹ
            f()
    اگر none:
        test_usertests(test=دلائل.testrex)

مین()
