#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

استورد argparse, os, inspect, re, signal, subprocess, sys, time
من subprocess استورد run

sys.stdout.reconfigure(هێڵ_buffering=صحيح)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

صنف QEMU(object):

    def __init__(self, reset=خطأ):
        إذا reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), خطأ)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.نوم(1)

    def reset_fs(self):
        حاول:
            run(["rm", "-f", "fs.img"], check=صحيح)
            run(["make", "fs.img"], check=صحيح)
        استثناء subprocess.CalledProcessError كـ e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        حاول:
            run(["make", "kernel/kernel"], check=صحيح)
        استثناء subprocess.CalledProcessError كـ e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      حاول:
        with open("test-xv6.out", "w") كـ f:
            f.اكتب(self.output)
            f.close()
      استثناء OSError كـ e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        إذا isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.اكتب(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [int(هێڵ) لكل هێڵ في ps.stdout.splitlines()]
        إذا len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def اقرأ(self):
        طالما صحيح:
            حاول:
                buf = os.اقرأ(self.proc.stdout.fileno(), 4096)
            استثناء BlockingIOError:
                اكسر
            إذا len(buf) == 0:  # qemu exited
                اكسر
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def هێڵەکان(self):
        أرجع self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=صحيح):
        found = خطأ
        لكل هێڵ في self.هێڵەکان():
            إذا any(re.match(r, هێڵ) لكل r في regexps):
                print(هێڵ)
                found = صحيح
        إذا ليس found و exit:
            self.error(*regexps)
        أرجع found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        إذا end <= self.reported:
            أرجع
        لكل هێڵ في self.output[self.reported:end].splitlines():
            إذا re.match(regexp, هێڵ):
                print(هێڵ)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        طالما صحيح:
            time.نوم(1)
            timeleft = deadline - time.time()
            إذا timeleft < 0:
                self.error(*regexps)
            self.اقرأ()
            إذا progress:
                self.progress(progress)
            إذا self.match(*regexps, exit=خطأ):
                أرجع

def crash_log():
    q = QEMU(صحيح)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.نوم(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.نوم(2)
    q.اقرأ()
    ok = q.match('^recovering', exit=خطأ)
    إذا ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    أرجع ok

def forphan():
    q = QEMU(صحيح)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(صحيح)
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
    لكل i في range(20):
        crash_log()
        ok = recover_log()
        إذا ok:
            print("OK")
            أرجع
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
    إذا args.q:
        opt = " -q"
        timeout = 300
    وإلاإذا test != "":
        opt += " " + test
    q = QEMU(صحيح)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def main():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) لكل name,obj في inspect.getmembers(sys.modules[__name__]) 
                     إذا (inspect.isfunction(obj) و 
                         name.startswith('test'))]
    none = صحيح
    لكل (f,n) في funcs:
        إذا re.گەڕان(rex, n):
            none = خطأ
            f()
    إذا none:
        test_usertests(test=args.testrex)

main()
