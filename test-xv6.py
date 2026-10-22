#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

ಆನಯನ argparse, os, inspect, re, signal, subprocess, sys, time
ತಃ subprocess ಆನಯನ run

sys.stdout.reconfigure(ಸಾಲು_buffering=ಸತ್ಯ)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

ವರ್ಗ QEMU(object):

    def __init__(self, reset=ಅಸತ್ಯ):
        ಯದಿ reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), ಅಸತ್ಯ)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.ಸುಷುಪ್ತಿ(1)

    def reset_fs(self):
        ಯತ್ನ:
            run(["rm", "-f", "fs.img"], check=ಸತ್ಯ)
            run(["make", "fs.img"], check=ಸತ್ಯ)
        ಅಪವಾದ subprocess.CalledProcessError ಇವ e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        ಯತ್ನ:
            run(["make", "kernel/kernel"], check=ಸತ್ಯ)
        ಅಪವಾದ subprocess.CalledProcessError ಇವ e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      ಯತ್ನ:
        with open("test-xv6.out", "w") ಇವ f:
            f.ಲಿಖ(self.output)
            f.close()
      ಅಪವಾದ OSError ಇವ e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        ಯದಿ isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.ಲಿಖ(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [ಪೂರ್ಣಾಙ್ಕ(ಸಾಲು) ಪ್ರತಿ ಸಾಲು ಅನ್ತಃ ps.stdout.splitlines()]
        ಯದಿ len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def ಪಠ(self):
        ಯಾವತ್ ಸತ್ಯ:
            ಯತ್ನ:
                buf = os.ಪಠ(self.proc.stdout.fileno(), 4096)
            ಅಪವಾದ BlockingIOError:
                ಭಙ್ಗ
            ಯದಿ len(buf) == 0:  # qemu exited
                ಭಙ್ಗ
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def ಸಾಲುಗಳು(self):
        ಪ್ರತ್ಯಾಗಮ self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=ಸತ್ಯ):
        found = ಅಸತ್ಯ
        ಪ್ರತಿ ಸಾಲು ಅನ್ತಃ self.ಸಾಲುಗಳು():
            ಯದಿ any(re.match(r, ಸಾಲು) ಪ್ರತಿ r ಅನ್ತಃ regexps):
                print(ಸಾಲು)
                found = ಸತ್ಯ
        ಯದಿ ನ found ಚ exit:
            self.error(*regexps)
        ಪ್ರತ್ಯಾಗಮ found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        ಯದಿ end <= self.reported:
            ಪ್ರತ್ಯಾಗಮ
        ಪ್ರತಿ ಸಾಲು ಅನ್ತಃ self.output[self.reported:end].splitlines():
            ಯದಿ re.match(regexp, ಸಾಲು):
                print(ಸಾಲು)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        ಯಾವತ್ ಸತ್ಯ:
            time.ಸುಷುಪ್ತಿ(1)
            timeleft = deadline - time.time()
            ಯದಿ timeleft < 0:
                self.error(*regexps)
            self.ಪಠ()
            ಯದಿ progress:
                self.progress(progress)
            ಯದಿ self.match(*regexps, exit=ಅಸತ್ಯ):
                ಪ್ರತ್ಯಾಗಮ

def crash_log():
    q = QEMU(ಸತ್ಯ)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.ಸುಷುಪ್ತಿ(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.ಸುಷುಪ್ತಿ(2)
    q.ಪಠ()
    ok = q.match('^recovering', exit=ಅಸತ್ಯ)
    ಯದಿ ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    ಪ್ರತ್ಯಾಗಮ ok

def forphan():
    q = QEMU(ಸತ್ಯ)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(ಸತ್ಯ)
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
    ಪ್ರತಿ i ಅನ್ತಃ range(20):
        crash_log()
        ok = recover_log()
        ಯದಿ ok:
            print("OK")
            ಪ್ರತ್ಯಾಗಮ
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
    ಯದಿ args.q:
        opt = " -q"
        timeout = 300
    ನೋಚೇದ್ಯದಿ test != "":
        opt += " " + test
    q = QEMU(ಸತ್ಯ)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def main():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) ಪ್ರತಿ name,obj ಅನ್ತಃ inspect.getmembers(sys.modules[__name__]) 
                     ಯದಿ (inspect.isfunction(obj) ಚ 
                         name.startswith('test'))]
    none = ಸತ್ಯ
    ಪ್ರತಿ (f,n) ಅನ್ತಃ funcs:
        ಯದಿ re.ಹುಡುಕು(rex, n):
            none = ಅಸತ್ಯ
            f()
    ಯದಿ none:
        test_usertests(test=args.testrex)

main()
