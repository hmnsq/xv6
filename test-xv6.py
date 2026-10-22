#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

आनयन argparse, os, inspect, re, signal, subprocess, sys, time
तः subprocess आनयन सततम्

sys.stdout.reconfigure(पङ्क्तिः_buffering=सत्य)

parser = argparse.तर्कः_Parser()
parser.सङ्कलय_तर्कः('testrex', help="test name or regular expression")
parser.सङ्कलय_तर्कः("-q", action='store_true', help="usertests quick")
तर्काः = parser.parse_तर्काः()

वर्ग QEMU(object):

    परिभाषा __init__(self, reset=असत्य):
        यदि reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.समुच्चयः_blocking(self.proc.stdout.fileno(), असत्य)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.सुषुप्ति(1)

    परिभाषा reset_fs(self):
        यत्न:
            सततम्(["rm", "-f", "fs.img"], check=सत्य)
            सततम्(["make", "fs.img"], check=सत्य)
        अपवाद subprocess.CalledProcessError इव e:
            मुद्रय(f"Command failed with exit code {e.returncode}")

    परिभाषा build_xv6(self):
        यत्न:
            सततम्(["make", "kernel/kernel"], check=सत्य)
        अपवाद subprocess.CalledProcessError इव e:
            मुद्रय(f"Command failed with exit code {e.returncode}")

    परिभाषा save_output(self):
      यत्न:
        सह open("test-xv6.out", "w") इव f:
            f.लिख(self.output)
            f.close()
      अपवाद OSError इव e:
        मुद्रय("Provided a bad results path. Error:", e)     
        
    परिभाषा cmd(self, c):
        यदि isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.लिख(c)
        self.proc.stdin.flush()
        
    परिभाषा crash(self):
        ps = सततम्(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [पूर्णाङ्क(पङ्क्तिः) प्रति पङ्क्तिः अन्तः ps.stdout.splitlines()]
        यदि दीर्घता(kids) == 0:
            मुद्रय("no qemu")
            sys.exit(1)
        मुद्रय("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    परिभाषा तिष्ठ(self):
        self.proc.terminate()

    परिभाषा पठ(self):
        यावत् सत्य:
            यत्न:
                buf = os.पठ(self.proc.stdout.fileno(), 4096)
            अपवाद BlockingIOError:
                भङ्ग
            यदि दीर्घता(buf) == 0:  # qemu exited
                भङ्ग
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    परिभाषा पङ्क्तयः(self):
        प्रत्यागम self.output.splitlines()

    परिभाषा error(self, *regexps):
        मुद्रय("FAIL: match failed", regexps)
        self.save_output()
        self.तिष्ठ()
        sys.exit(1)

    परिभाषा मेलनम्(self, *regexps, exit=सत्य):
        प्राप्तम् = असत्य
        प्रति पङ्क्तिः अन्तः self.पङ्क्तयः():
            यदि any(re.मेलनम्(r, पङ्क्तिः) प्रति r अन्तः regexps):
                मुद्रय(पङ्क्तिः)
                प्राप्तम् = सत्य
        यदि न प्राप्तम् च exit:
            self.error(*regexps)
        प्रत्यागम प्राप्तम्

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    परिभाषा प्रगतिः(self, regexp):
        अन्तः_2 = self.output.rfind("\n") + 1
        यदि अन्तः_2 <= self.reported:
            प्रत्यागम
        प्रति पङ्क्तिः अन्तः self.output[self.reported:अन्तः_2].splitlines():
            यदि re.मेलनम्(regexp, पङ्क्तिः):
                मुद्रय(पङ्क्तिः)
        self.reported = अन्तः_2

    परिभाषा monitor(self, *regexps, प्रगतिः="", timeout):
        deadline = time.time() + timeout
        यावत् सत्य:
            time.सुषुप्ति(1)
            timeleft = deadline - time.time()
            यदि timeleft < 0:
                self.error(*regexps)
            self.पठ()
            यदि प्रगतिः:
                self.प्रगतिः(प्रगतिः)
            यदि self.मेलनम्(*regexps, exit=असत्य):
                प्रत्यागम

परिभाषा crash_log():
    q = QEMU(सत्य)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.सुषुप्ति(2)
    q.crash()
    q.तिष्ठ()

परिभाषा recover_log():
    q = QEMU()
    time.सुषुप्ति(2)
    q.पठ()
    ok = q.मेलनम्('^recovering', exit=असत्य)
    यदि ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.तिष्ठ()
    प्रत्यागम ok

परिभाषा forphan():
    q = QEMU(सत्य)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.तिष्ठ()

परिभाषा dorphan():
    q = QEMU(सत्य)
    q.cmd("dorphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.तिष्ठ()

परिभाषा recover_orphan():
    q = QEMU()
    q.monitor('^ireclaim', timeout=30)
    q.तिष्ठ()

परिभाषा test_log():
    मुद्रय("Test recovery of log")
    प्रति i अन्तः परिसरः(20):
        crash_log()
        ok = recover_log()
        यदि ok:
            मुद्रय("OK")
            प्रत्यागम
        मुद्रय("log attempt ", i+1)
    मुद्रय("FAIL")
    sys.exit(1)
    
परिभाषा test_forphan():
    मुद्रय("Test recovery of an orphaned file")
    forphan()
    recover_orphan()
    मुद्रय("OK")

परिभाषा test_dorphan():
    मुद्रय("Test recovery of an orphaned file")
    dorphan()
    recover_orphan()
    मुद्रय("OK")

परिभाषा test_crash():
    test_log()
    test_forphan()
    test_dorphan()

परिभाषा test_usertests(test=""):
    timeout = 600
    opt = ""
    यदि तर्काः.q:
        opt = " -q"
        timeout = 300
    नोचेद्यदि test != "":
        opt += " " + test
    q = QEMU(सत्य)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', प्रगतिः='test', timeout=timeout)
    q.तिष्ठ()

परिभाषा मुख्यम्():
    मुद्रय(तर्काः)
    rex = r'%s' % तर्काः.testrex
    funcs = [(obj,name) प्रति name,obj अन्तः inspect.getmembers(sys.modules[__name__]) 
                     यदि (inspect.isfunction(obj) च 
                         name.startswith('test'))]
    none = सत्य
    प्रति (f,n) अन्तः funcs:
        यदि re.अन्वेषणम्(rex, n):
            none = असत्य
            f()
    यदि none:
        test_usertests(test=तर्काः.testrex)

मुख्यम्()
