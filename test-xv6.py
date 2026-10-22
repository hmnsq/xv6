#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

ഇറക്കുമതി argparse, os, inspect, re, signal, subprocess, sys, time
തഃ subprocess ഇറക്കുമതി run

sys.stdout.reconfigure(വരി_buffering=സത്യം)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

വർഗ്ഗം QEMU(object):

    def __init__(self, reset=അസത്യം):
        എങ്കിൽ reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), അസത്യം)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.സുഷുപ്തി(1)

    def reset_fs(self):
        ശ്രമിക്കുക:
            run(["rm", "-f", "fs.img"], check=സത്യം)
            run(["make", "fs.img"], check=സത്യം)
        അപവാദം subprocess.CalledProcessError ഇവ e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        ശ്രമിക്കുക:
            run(["make", "kernel/kernel"], check=സത്യം)
        അപവാദം subprocess.CalledProcessError ഇവ e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      ശ്രമിക്കുക:
        with open("test-xv6.out", "w") ഇവ f:
            f.ലിഖ(self.output)
            f.close()
      അപവാദം OSError ഇവ e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        എങ്കിൽ isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.ലിഖ(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [പൂർണ്ണസംഖ്യ(വരി) വേണ്ടി വരി ഇൽ ps.stdout.splitlines()]
        എങ്കിൽ len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def പഠ(self):
        ആകുമ്പോൾ സത്യം:
            ശ്രമിക്കുക:
                buf = os.പഠ(self.proc.stdout.fileno(), 4096)
            അപവാദം BlockingIOError:
                നിർത്തുക
            എങ്കിൽ len(buf) == 0:  # qemu exited
                നിർത്തുക
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def വരികൾ(self):
        തിരികെ self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=സത്യം):
        found = അസത്യം
        വേണ്ടി വരി ഇൽ self.വരികൾ():
            എങ്കിൽ any(re.match(r, വരി) വേണ്ടി r ഇൽ regexps):
                print(വരി)
                found = സത്യം
        എങ്കിൽ അല്ല found ഒപ്പം exit:
            self.error(*regexps)
        തിരികെ found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        എങ്കിൽ end <= self.reported:
            തിരികെ
        വേണ്ടി വരി ഇൽ self.output[self.reported:end].splitlines():
            എങ്കിൽ re.match(regexp, വരി):
                print(വരി)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        ആകുമ്പോൾ സത്യം:
            time.സുഷുപ്തി(1)
            timeleft = deadline - time.time()
            എങ്കിൽ timeleft < 0:
                self.error(*regexps)
            self.പഠ()
            എങ്കിൽ progress:
                self.progress(progress)
            എങ്കിൽ self.match(*regexps, exit=അസത്യം):
                തിരികെ

def crash_log():
    q = QEMU(സത്യം)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.സുഷുപ്തി(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.സുഷുപ്തി(2)
    q.പഠ()
    ok = q.match('^recovering', exit=അസത്യം)
    എങ്കിൽ ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    തിരികെ ok

def forphan():
    q = QEMU(സത്യം)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(സത്യം)
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
    വേണ്ടി i ഇൽ range(20):
        crash_log()
        ok = recover_log()
        എങ്കിൽ ok:
            print("OK")
            തിരികെ
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
    എങ്കിൽ args.q:
        opt = " -q"
        timeout = 300
    അല്ലെങ്കിലെങ്കിൽ test != "":
        opt += " " + test
    q = QEMU(സത്യം)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def പ്രധാന():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) വേണ്ടി name,obj ഇൽ inspect.getmembers(sys.modules[__name__]) 
                     എങ്കിൽ (inspect.isfunction(obj) ഒപ്പം 
                         name.startswith('test'))]
    none = സത്യം
    വേണ്ടി (f,n) ഇൽ funcs:
        എങ്കിൽ re.തിരയൽ(rex, n):
            none = അസത്യം
            f()
    എങ്കിൽ none:
        test_usertests(test=args.testrex)

പ്രധാന()
