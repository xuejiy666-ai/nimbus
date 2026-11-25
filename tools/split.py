import re, os, sys

src, outdir = sys.argv[1], sys.argv[2]
K = int(sys.argv[3]) if len(sys.argv) > 3 else 220
s = open(src, encoding='utf-8').read()

def alpha(n):
    r = ''
    n += 1
    while n:
        n, k = divmod(n - 1, 26)
        r = chr(65 + k) + r
    return r

items = []
i, n = 0, len(s)
while i < n:
    while i < n and s[i] in ' \t\r\n':
        i += 1
    if i >= n:
        break
    if s[i] == '#':
        j = i
        while j < n:
            e = s.find('\n', j)
            if e < 0:
                e = n
            if s[e-1] == '\\':
                j = e + 1
                continue
            j = e
            break
        items.append(('pp', s[i:j], i))
        i = j
        continue
    start = i
    depth = 0
    isfn = False
    while i < n:
        c = s[i]
        if c in ('"', "'"):
            q = c
            i += 1
            while i < n:
                if s[i] == '\\':
                    i += 2
                    continue
                if s[i] == q:
                    i += 1
                    break
                i += 1
            continue
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        elif c == '}':
            if depth > 0:
                depth -= 1
        elif c == ';' and depth == 0:
            i += 1
            break
        elif c == '{' and depth == 0:
            prev = s[start:i].rstrip()
            if prev.endswith(')'):
                isfn = True
                d = 0
                while i < n:
                    ch = s[i]
                    if ch == '{':
                        d += 1
                    elif ch == '}':
                        d -= 1
                        if d == 0:
                            i += 1
                            break
                    elif ch in ('"', "'"):
                        q = ch
                        i += 1
                        while i < n:
                            if s[i] == '\\':
                                i += 2
                                continue
                            if s[i] == q:
                                break
                            i += 1
                    i += 1
                break
            depth += 1
        i += 1
    items.append(('fn' if isfn else 'obj', s[start:i].strip(), start))

hdr, glb, fns = [], [], []
for kind, t, pos in items:
    if kind == 'pp':
        if not t.startswith('#include'):
            hdr.append(t)
    elif t.startswith(('typedef', 'enum', 'struct', 'union')):
        hdr.append(t)
    elif kind == 'fn':
        fns.append((t, pos))
    else:
        if t.startswith('extern'):
            hdr.append(t)
        elif '(' in t and '=' not in t:
            hdr.append(re.sub(r'^static\s+', '', t))
        else:
            tt = re.sub(r'^static\s+', '', t)
            eq = tt.find('=')
            decl = (tt[:eq] if eq >= 0 else tt.rstrip(';')).rstrip()
            hdr.append('extern ' + decl + ';')
            glb.append(tt)

protos = []
for fn, _ in fns:
    sig = fn[:fn.index('{')].strip()
    protos.append(re.sub(r'^static\s+', '', sig) + ';')

bodies = [re.sub(r'^static\s+', '', fn) for fn, _ in fns]

if K > len(bodies):
    K = len(bodies)
load = [0] * K
buckets = [[] for _ in range(K)]
order = sorted(range(len(bodies)), key=lambda k: -len(bodies[k]))
for k in order:
    b = min(range(K), key=lambda x: load[x])
    buckets[b].append(k)
    load[b] += len(bodies[k]) + 2

buckets.sort(key=lambda b: min(b))
os.makedirs(outdir, exist_ok=True)
for f in os.listdir(outdir):
    if f.endswith('.c') or f == 'sc.h':
        os.remove(os.path.join(outdir, f))

includes = '''#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <zlib.h>
#include <pthread.h>
#include <dlfcn.h>
#include <jni.h>
'''
with open(os.path.join(outdir, 'sc.h'), 'w', encoding='utf-8') as f:
    f.write('#ifndef SC_H\n#define SC_H\n\n')
    f.write(includes + '\n')
    for h in hdr:
        f.write(h + '\n')
    f.write('\n')
    for pr in protos:
        f.write(pr + '\n')
    f.write('\n#endif\n')

with open(os.path.join(outdir, 'sc_globals.c'), 'w', encoding='utf-8') as f:
    for g in glb:
        f.write(g + '\n\n')

for idx, b in enumerate(buckets):
    with open(os.path.join(outdir, alpha(idx) + '.c'), 'w', encoding='utf-8') as f:
        f.write('\n\n'.join(bodies[k] for k in b) + '\n')

print('files', len(buckets), 'functions', len(bodies),
      'max', max(load), 'min', min(load), 'avg', sum(len(x) for x in bodies)//len(buckets))
