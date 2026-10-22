#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

आनयन argparse, os, inspect, re, signal, subprocess, sys, time
तः subprocess आनयन run

sys.stdout.reconfigure(पंक्ति_buffering=सत्य)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

वर्ग QEMU(object):

    def __init__(self, reset=असत्य):
        यदि reset:
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
        यत्न:
            run(["rm", "-f", "fs.img"], check=सत्य)
            run(["make", "fs.img"], check=सत्य)
        अपवाद subprocess.CalledProcessError इव e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        यत्न:
            run(["make", "kernel/kernel"], check=सत्य)
        अपवाद subprocess.CalledProcessError इव e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      यत्न:
        with open("test-xv6.out", "w") इव f:
            f.लिख(self.output)
            f.close()
      अपवाद OSError इव e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        यदि isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.लिख(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [पूर्णाङ्क(पंक्ति) प्रति पंक्ति अन्तः ps.stdout.splitlines()]
        यदि len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def पठ(self):
        यावत् सत्य:
            यत्न:
                buf = os.पठ(self.proc.stdout.fileno(), 4096)
            अपवाद BlockingIOError:
                भङ्ग
            यदि len(buf) == 0:  # qemu exited
                भङ्ग
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def पंक्तियाँ(self):
        प्रत्यागम self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=सत्य):
        found = असत्य
        प्रति पंक्ति अन्तः self.पंक्तियाँ():
            यदि any(re.match(r, पंक्ति) प्रति r अन्तः regexps):
                print(पंक्ति)
                found = सत्य
        यदि न found च exit:
            self.error(*regexps)
        प्रत्यागम found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        यदि end <= self.reported:
            प्रत्यागम
        प्रति पंक्ति अन्तः self.output[self.reported:end].splitlines():
            यदि re.match(regexp, पंक्ति):
                print(पंक्ति)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        यावत् सत्य:
            time.सुषुप्ति(1)
            timeleft = deadline - time.time()
            यदि timeleft < 0:
                self.error(*regexps)
            self.पठ()
            यदि progress:
                self.progress(progress)
            यदि self.match(*regexps, exit=असत्य):
                प्रत्यागम

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
    यदि ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    प्रत्यागम ok

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
    प्रति i अन्तः range(20):
        crash_log()
        ok = recover_log()
        यदि ok:
            print("OK")
            प्रत्यागम
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
    यदि args.q:
        opt = " -q"
        timeout = 300
    नोचेद्यदि test != "":
        opt += " " + test
    q = QEMU(सत्य)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def main():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) प्रति name,obj अन्तः inspect.getmembers(sys.modules[__name__]) 
                     यदि (inspect.isfunction(obj) च 
                         name.startswith('test'))]
    none = सत्य
    प्रति (f,n) अन्तः funcs:
        यदि re.खोज(rex, n):
            none = असत्य
            f()
    यदि none:
        test_usertests(test=args.testrex)

main()
