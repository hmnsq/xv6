#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

import argparse, os, inspect, re, signal, subprocess, sys, time
ta_h subprocess import run

sys.stdout.reconfigure(line_buffering=True)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

class QEMU(object):

    def __init__(self, reset=False):
        གལ་ཏེ reset:
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
        time.su_supti(1)

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
        གལ་ཏེ isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.write(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [ཧྲིལ(line) ཆེད་དུ line anta_h ps.stdout.splitlines()]
        གལ་ཏེ len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def read(self):
        སྐབས True:
            try:
                buf = os.read(self.proc.stdout.fileno(), 4096)
            except BlockingIOError:
                bha_nga
            གལ་ཏེ len(buf) == 0:  # qemu exited
                bha_nga
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def lines(self):
        ཕྱིར self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=True):
        found = False
        ཆེད་དུ line anta_h self.lines():
            གལ་ཏེ any(re.match(r, line) ཆེད་དུ r anta_h regexps):
                print(line)
                found = True
        གལ་ཏེ na found ca exit:
            self.error(*regexps)
        ཕྱིར found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        གལ་ཏེ end <= self.reported:
            ཕྱིར
        ཆེད་དུ line anta_h self.output[self.reported:end].splitlines():
            གལ་ཏེ re.match(regexp, line):
                print(line)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        སྐབས True:
            time.su_supti(1)
            timeleft = deadline - time.time()
            གལ་ཏེ timeleft < 0:
                self.error(*regexps)
            self.read()
            གལ་ཏེ progress:
                self.progress(progress)
            གལ་ཏེ self.match(*regexps, exit=False):
                ཕྱིར

def crash_log():
    q = QEMU(True)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.su_supti(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.su_supti(2)
    q.read()
    ok = q.match('^recovering', exit=False)
    གལ་ཏེ ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    ཕྱིར ok

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
    ཆེད་དུ i anta_h range(20):
        crash_log()
        ok = recover_log()
        གལ་ཏེ ok:
            print("OK")
            ཕྱིར
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
    གལ་ཏེ args.q:
        opt = " -q"
        timeout = 300
    elif test != "":
        opt += " " + test
    q = QEMU(True)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def གཙོ():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) ཆེད་དུ name,obj anta_h inspect.getmembers(sys.modules[__name__]) 
                     གལ་ཏེ (inspect.isfunction(obj) ca 
                         name.startswith('test'))]
    none = True
    ཆེད་དུ (f,n) anta_h funcs:
        གལ་ཏེ re.search(rex, n):
            none = False
            f()
    གལ་ཏེ none:
        test_usertests(test=args.testrex)

གཙོ()
