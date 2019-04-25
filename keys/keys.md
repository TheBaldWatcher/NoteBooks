# bash

<font color = red>红色表示以后用到再看：Linux命令行与shell脚本编程大全</font>：

* `!`相关：
  * `!$`是上一个命令的最后一个字符串，类似功能的有`esc + .`
  * `!!`是上一条指令。另外`^old^new`可以把上一条指令的old全替换为new
  * `!ls`是上一条ls指令，其他类推。在zsh支持下，只用敲tab
* `du -sh`，`df -ih`
* 网络问题：
  * <font color=red>[用mtr诊断网络问题]</font> https://cloud.tencent.com/developer/article/1332118
  * <font color=red>用curl向服务器发送命令</font>、查看本机在外网的地址：`curl ifcongig.me`
  * <font color = red> 这里有一些网络相关的东西</font>https://coolshell.cn/articles/8883.html
* <font color = red>定时任务、cron</font>https://blog.csdn.net/kinglyjn/article/details/53519470
* <font color=red>进程信息ps</font>https://www.cnblogs.com/wxgblogs/p/6591980.html
* man ascii显示码表
* 远端调试：
  * `ssh user@server bash < /path/to/local/script.sh`运行脚本，可以免去脚本的拷贝
  * `vim scp://username@host//path/to/somefile`vim一个远程该文件
* <font color =red> xargs </font>
* 通过 <(some command) 可以把某命令当成一个文件。示例：比较一个本地文件和远程文件 /etc/hosts： diff /etc/hosts <(ssh somehost cat /etc/hosts)
* 编码转换： 
  - UNIX2dos/dos2UNIX -n(ew) k(eep)
  - iconv -f from -t to filename [-o newfile]



# vim

* <font color = red>添加IDE特性</font>https://coolshell.cn/articles/894.html
* ![vim_cheat_sheet_for_programmers_print](/Users/ashtower/Desktop/sdk_workspace/to_delete/vim_cheat_sheet_for_programmers_print.png)
* `#,%,*`:`%`匹配括号进行移动；`*`选中当前单词，之后`*`切换到下一个单词，`#`切换到上一个单词