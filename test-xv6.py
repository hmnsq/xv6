#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

आयात argparse, os, inspect, re, signal, subprocess, sys, time
तः subprocess आयात run

sys.stdout.reconfigure(ओळ_buffering=सत्य)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

वर्ग QEMU(object):

    def __init__(self, reset=असत्य):
        जर reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), असत्य)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.सुषुप्ति(1)

    def reset_fs(self):
        प्रयत्न:
            run(["rm", "-f", "fs.img"], check=सत्य)
            run(["make", "fs.img"], check=सत्य)
        अपवाद subprocess.CalledProcessError इव e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        प्रयत्न:
            run(["make", "kernel/kernel"], check=सत्य)
        अपवाद subprocess.CalledProcessError इव e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      प्रयत्न:
        with open("test-xv6.out", "w") इव f:
            f.लिख(self.output)
            f.बंद()
      अपवाद OSError इव e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        जर isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.लिख(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [पूर्णांक(ओळ) साठी ओळ मध्ये ps.stdout.splitlines()]
        जर len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def पठ(self):
        पर्यंत सत्य:
            प्रयत्न:
                buf = os.पठ(self.proc.stdout.fileno(), 4096)
            अपवाद BlockingIOError:
                थांबा
            जर len(buf) == 0:  # qemu exited
                थांबा
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def ओळी(self):
        परत self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=सत्य):
        found = असत्य
        साठी ओळ मध्ये self.ओळी():
            जर any(re.match(r, ओळ) साठी r मध्ये regexps):
                print(ओळ)
                found = सत्य
        जर नाही found आणि exit:
            self.error(*regexps)
        परत found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        जर end <= self.reported:
            परत
        साठी ओळ मध्ये self.output[self.reported:end].splitlines():
            जर re.match(regexp, ओळ):
                print(ओळ)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        पर्यंत सत्य:
            time.सुषुप्ति(1)
            timeleft = deadline - time.time()
            जर timeleft < 0:
                self.error(*regexps)
            self.पठ()
            जर progress:
                self.progress(progress)
            जर self.match(*regexps, exit=असत्य):
                परत

def crash_log():
    q = QEMU(सत्य)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.सुषुप्ति(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.सुषुप्ति(2)
    q.पठ()
    ok = q.match('^recovering', exit=असत्य)
    जर ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    परत ok

def forphan():
    q = QEMU(सत्य)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(सत्य)
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
    साठी i मध्ये range(20):
        crash_log()
        ok = recover_log()
        जर ok:
            print("OK")
            परत
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
    जर args.q:
        opt = " -q"
        timeout = 300
    किंवाजर test != "":
        opt += " " + test
    q = QEMU(सत्य)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def main():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) साठी name,obj मध्ये inspect.getmembers(sys.modules[__name__]) 
                     जर (inspect.isfunction(obj) आणि 
                         name.startswith('test'))]
    none = सत्य
    साठी (f,n) मध्ये funcs:
        जर re.शोध(rex, n):
            none = असत्य
            f()
    जर none:
        test_usertests(test=args.testrex)

main()
