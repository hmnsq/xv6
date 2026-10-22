#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

واردکن argparse, os, inspect, re, signal, subprocess, sys, time
از subprocess واردکن run

sys.stdout.reconfigure(سطر_buffering=درست)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

رده QEMU(object):

    def __init__(self, reset=نادرست):
        اگر reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), نادرست)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.نوم(1)

    def reset_fs(self):
        تلاش:
            run(["rm", "-f", "fs.img"], check=درست)
            run(["make", "fs.img"], check=درست)
        استثنا subprocess.CalledProcessError بهعنوان e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        تلاش:
            run(["make", "kernel/kernel"], check=درست)
        استثنا subprocess.CalledProcessError بهعنوان e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      تلاش:
        with open("test-xv6.out", "w") بهعنوان f:
            f.اكتب(self.output)
            f.close()
      استثنا OSError بهعنوان e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        اگر isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.اكتب(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [صحیح(سطر) برای سطر در ps.stdout.splitlines()]
        اگر len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def اقرأ(self):
        وقتی درست:
            تلاش:
                buf = os.اقرأ(self.proc.stdout.fileno(), 4096)
            استثنا BlockingIOError:
                بشکن
            اگر len(buf) == 0:  # qemu exited
                بشکن
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def سطرها(self):
        بازگشت self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=درست):
        found = نادرست
        برای سطر در self.سطرها():
            اگر any(re.match(r, سطر) برای r در regexps):
                print(سطر)
                found = درست
        اگر نه found و exit:
            self.error(*regexps)
        بازگشت found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        اگر end <= self.reported:
            بازگشت
        برای سطر در self.output[self.reported:end].splitlines():
            اگر re.match(regexp, سطر):
                print(سطر)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        وقتی درست:
            time.نوم(1)
            timeleft = deadline - time.time()
            اگر timeleft < 0:
                self.error(*regexps)
            self.اقرأ()
            اگر progress:
                self.progress(progress)
            اگر self.match(*regexps, exit=نادرست):
                بازگشت

def crash_log():
    q = QEMU(درست)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.نوم(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.نوم(2)
    q.اقرأ()
    ok = q.match('^recovering', exit=نادرست)
    اگر ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    بازگشت ok

def forphan():
    q = QEMU(درست)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(درست)
    q.cmd("dorphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def recover_orphan():
    q = QEMU()
    q.monitor('^ireclaim', timeout=30)
    q.stop()

def test_log():
    print("Test recovery of log")
    برای i در range(20):
        crash_log()
        ok = recover_log()
        اگر ok:
            print("OK")
            بازگشت
        print("log attempt ", i+1)
    print("FAIL")
    sys.exit(1)
    
def test_forphan():
    print("Test recovery of an orphaned file")
    forphan()
    recover_orphan()
    print("OK")

def test_dorphan():
    print("Test recovery of an orphaned file")
    dorphan()
    recover_orphan()
    print("OK")

def test_crash():
    test_log()
    test_forphan()
    test_dorphan()

def test_usertests(test=""):
    timeout = 600
    opt = ""
    اگر args.q:
        opt = " -q"
        timeout = 300
    وگرنهاگر test != "":
        opt += " " + test
    q = QEMU(درست)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def اصلی():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) برای name,obj در inspect.getmembers(sys.modules[__name__]) 
                     اگر (inspect.isfunction(obj) و 
                         name.startswith('test'))]
    none = درست
    برای (f,n) در funcs:
        اگر re.جستجو(rex, n):
            none = نادرست
            f()
    اگر none:
        test_usertests(test=args.testrex)

اصلی()
