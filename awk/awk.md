* Basic: 和c++差不多
  * while、for、printf、数组
* Patterns&Actions
  * regexpr: `/regexpr/`匹配substr；`$4~/regexpr/`匹配substr，`$4 !~/regexpr/`排他匹配substr
    * `pat1, pat2` 范围匹配：从pat1出现，直至pat2(若无，则一直匹配到末尾)
  * Built-in Arithmeic Functions: atan2, cos, exp, log, rand, sqrt, srand
  * Build-in Variable: `ARGC,ARGV,FILENAME,FNR,FS,NF,NR,OFMT,OFS,ORS,RLENGTH,RS,RSTART,SUBSEP`，Page36
  * string
    * `index("banana", "an")` = 2
    * `match("banana", "an")`, RSTART = 1, RLENTH = 2
    * `split(s, a, fs)`
    * `gsub(r, s), gsub(r, s, t), sub(r, s), sub(r, s, t)`：`&`表示被r匹配到的字符串
    * `substr(s, p), substr(s, p, n)`。
    * `sprintf, printf`
    * `substr($0, index($0, " "))`：删掉空格前的所有字符
  * `number""`使数字coerce为string，`string + 0`使string coerce为数字；数字默认输出格式为`OFMT = %.2f`。
    * 字符串比较`$1 "" == $2`，数字比较`$1 + 0 == $2 + 0`
  * `for key in arr`， key**是字符串**——awk的数组类似于map、`arr[xxx]`可能会插入一个node。另外删除是用`delete arr[key]`
  * udf: 除了arrary，其他都是by-value。local variable要声明在param-list中，不然就是全局变量
  * getline: 1-正常；0-eof；-1-error，Page62
* Data Processing
  * multi-line: `BEGIN {RS = ""; ORS = "\n\n"}`
* Reports And Database
  * 传递变量：两次遍历（第一次求总，第二次求百分比）:`awk -f prep3 pass=1 countries pass=2 countries`
  
  * 一个表头参考：Page 94
  
  * awk文件中`| " sort "`用双引号表示unix命令，shell文件中`$ n`表示替换shell变量。
  
    * in shell: 
  
      * ```shell
        $1 ~ /`$1`/ {}
        ```
  
      * 
  
  * 邮件格式化：替换`#1`为
  
  * **A Relational Database System**







`awk '/'$1' ranking\/ranking\.cc:503/,/^$/ { print $0 }' $2 > $1`