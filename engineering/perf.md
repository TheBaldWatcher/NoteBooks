# perf Examples

* [网页](https://www.brendangregg.com/perf.html)。

```bash
# Counting events
perf stat -p PID sleep 5

# Profiling
# a/p 所有，或者某个pid
# -g 收集调用栈，可改为--call-graph dwarf，但是文件大小会爆炸, X40。
# F 采样频率/s
perf record -F 99 -p PID -g -- sleep 10

# 以下不太清楚咋在实际中运用: Static/Dynamic Tracing
perf probe -x /lib64/libc.so.6 malloc

# 采样命令
perf record -a -g --call-graph dwarf -p 101018 -F 99 -- sleep 60
sudo perf report -i perf.data > perf.txt
```

* 6.1  
  * insn per cycle  ——IPC
    * I'd generally interpret high IPC values (eg, over 1.0) as goo
  * stalled cycles per insn —— SCPI
    * This makes it easy to interpret: stalls are latency, reduce stalls.
* 6.3 
  * Skew and PEBS 。event下的采样有可能不精确。解决办法是加:p  （PEBS/IBS）



## gperf

* 1、sudo yum install graphviz-gd
* 2、pprof --gif recommend_server_trpc cpu.prof >cpu_profiling.gif 