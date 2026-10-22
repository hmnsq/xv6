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

sys.stdout.reconfigure(line_buffering=אמת)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

מחלקה QEMU(object):

    def __init__(self, reset=שקר):
        אם reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), שקר)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.نوم(1)

    def reset_fs(self):
        حاول:
            run(["rm", "-f", "fs.img"], check=אמת)
            run(["make", "fs.img"], check=אמת)
        استثناء subprocess.CalledProcessError كـ e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        حاول:
            run(["make", "kernel/kernel"], check=אמת)
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
        אם isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.اكتب(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [صحيح(line) עבור line في ps.stdout.splitlines()]
        אם len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def اقرأ(self):
        כלעוד אמת:
            حاول:
                buf = os.اقرأ(self.proc.stdout.fileno(), 4096)
            استثناء BlockingIOError:
                اكسر
            אם len(buf) == 0:  # qemu exited
                اكسر
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def lines(self):
        החזר self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=אמת):
        found = שקר
        עבור line في self.lines():
            אם any(re.match(r, line) עבור r في regexps):
                print(line)
                found = אמת
        אם לא found וגם exit:
            self.error(*regexps)
        החזר found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        אם end <= self.reported:
            החזר
        עבור line في self.output[self.reported:end].splitlines():
            אם re.match(regexp, line):
                print(line)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        כלעוד אמת:
            time.نوم(1)
            timeleft = deadline - time.time()
            אם timeleft < 0:
                self.error(*regexps)
            self.اقرأ()
            אם progress:
                self.progress(progress)
            אם self.match(*regexps, exit=שקר):
                החזר

def crash_log():
    q = QEMU(אמת)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.نوم(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.نوم(2)
    q.اقرأ()
    ok = q.match('^recovering', exit=שקר)
    אם ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    החזר ok

def forphan():
    q = QEMU(אמת)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(אמת)
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
    עבור i في range(20):
        crash_log()
        ok = recover_log()
        אם ok:
            print("OK")
            החזר
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
    אם args.q:
        opt = " -q"
        timeout = 300
    وإلاإذا test != "":
        opt += " " + test
    q = QEMU(אמת)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def main():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) עבור name,obj في inspect.getmembers(sys.modules[__name__]) 
                     אם (inspect.isfunction(obj) וגם 
                         name.startswith('test'))]
    none = אמת
    עבור (f,n) في funcs:
        אם re.search(rex, n):
            none = שקר
            f()
    אם none:
        test_usertests(test=args.testrex)

main()
