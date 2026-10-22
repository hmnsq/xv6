#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

import argparse, os, inspect, re, signal, subprocess, sys, time
from subprocess import run

sys.stdout.reconfigure(line_buffering=True)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

class QEMU(object):

    def __init__(self, reset=False):
        nếu reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), False)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.sleep(1)

    def reset_fs(self):
        try:
            run(["rm", "-f", "fs.img"], check=True)
            run(["make", "fs.img"], check=True)
        except subprocess.CalledProcessError as e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        try:
            run(["make", "kernel/kernel"], check=True)
        except subprocess.CalledProcessError as e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      try:
        with open("test-xv6.out", "w") as f:
            f.write(self.output)
            f.close()
      except OSError as e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        nếu isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.write(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [số_nguyên(line) cho line in ps.stdout.splitlines()]
        nếu len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def read(self):
        trong_khi True:
            try:
                buf = os.read(self.proc.stdout.fileno(), 4096)
            except BlockingIOError:
                break
            nếu len(buf) == 0:  # qemu exited
                break
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def lines(self):
        trả_về self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=True):
        found = False
        cho line in self.lines():
            nếu any(re.match(r, line) cho r in regexps):
                print(line)
                found = True
        nếu not found and exit:
            self.error(*regexps)
        trả_về found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        nếu end <= self.reported:
            trả_về
        cho line in self.output[self.reported:end].splitlines():
            nếu re.match(regexp, line):
                print(line)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        trong_khi True:
            time.sleep(1)
            timeleft = deadline - time.time()
            nếu timeleft < 0:
                self.error(*regexps)
            self.read()
            nếu progress:
                self.progress(progress)
            nếu self.match(*regexps, exit=False):
                trả_về

def crash_log():
    q = QEMU(True)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.sleep(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.sleep(2)
    q.read()
    ok = q.match('^recovering', exit=False)
    nếu ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    trả_về ok

def forphan():
    q = QEMU(True)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(True)
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
    cho i in range(20):
        crash_log()
        ok = recover_log()
        nếu ok:
            print("OK")
            trả_về
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
    nếu args.q:
        opt = " -q"
        timeout = 300
    elif test != "":
        opt += " " + test
    q = QEMU(True)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def main():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) cho name,obj in inspect.getmembers(sys.modules[__name__]) 
                     nếu (inspect.isfunction(obj) and 
                         name.startswith('test'))]
    none = True
    cho (f,n) in funcs:
        nếu re.search(rex, n):
            none = False
            f()
    nếu none:
        test_usertests(test=args.testrex)

main()
