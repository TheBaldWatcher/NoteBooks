[Greg Law “Debugging Linux C++”](https://www.youtube.com/watch?v=V1t6faOKjuQ&list=PLHTh1InhhwT6V9RVdFRoCG_Pm5udDxG1c&index=38)

* ```
watch foo  				# stop when foo is modified
watch -l foo 			# watch location
rwatch foo				# stop when foo is read
watch foo thread 3 		# stop when thread 3 modifies foo
watch foo if foo > 10 	# stop when foo is > 10

thread apply 1-4 print $sp # stack pointer
thread apply all backtrace full

# dprintf location, template, expresion
dprintf  perf_msgr_client.cc:133,"Test send i = %d，ops = %d,inflight = %d\n",i,ops,inflight

catch catch
catch syscall 100

gdb reverse-stepi https://sourceware.org/gdb/onlinedocs/gdb/Reverse-Execution.html
```