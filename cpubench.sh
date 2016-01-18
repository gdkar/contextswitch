#!/bin/bash
fgrep 'model name' /proc/cpuinfo | sort -u | xargs
pgrep vmware >/dev/null && echo Running under VMware
ncpus=`sort -u /sys/devices/system/cpu/cpu*/topology/physical_package_id | wc -l`
corepercpu=`sort -u /sys/devices/system/cpu/cpu*/topology/core_id | wc -l`
threadpercore=`sed \
"s/2/10/g;\
s/3/11/g;\
s/4/100/g;\
s/5/101/g;\
s/6/110/g;\
s/7/111/g;\
s/8/1000/g;\
s/9/1001/g;\
s/a/1010/g;\
s/b/1011/g;\
s/c/1100/g;\
s/d/1101/g;\
s/e/1110/g;\
s/f/1111/g;\
s/[^1]//g" /sys/devices/system/cpu/cpu*/topology/thread_siblings \
  | while read nthreads; do echo ${#nthreads}; done | sort -u`
total=$((ncpus * corepercpu * threadpercore))
echo "$ncpus physical CPUs, $corepercpu cores/CPU,\
 $threadpercore hardware threads/core = $total hw threads total"

runbench() {
  $* ./timespin
  $* ./timefncall
  LD_LIBRARY_PATH=$./:$LD_LIBRARY_PATH $* ./timelibcall
  $* ./timesyscall
  $* ./timectxsw
  $* ./timectxswsem
  $* ./timectxswpipe
  $* ./timetctxsw
  $* ./timetctxsw2
  $* ./timetctxswsem
  $* ./timetctxswcond
  $* ./timetctxswpipe
}

echo '-- No CPU affinity --'
runbench

echo "-- With CPU affinity 0 ( for parent ) --"
lastcpu=`awk </dev/null -v ncpus=$ncpus 'BEGIN {
  n = 1;
  for(i = 1; i < ncpus; i++) n *= 2;
  printf("0x%x\n", n);
}'`
runbench taskset  0x2

echo '-- With CPU affinity to CPU 0 for all --'
runbench taskset -a -c 0
