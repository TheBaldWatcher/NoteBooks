* Basic: 和c++差不多
  * while、for、printf、数组
* Patterns&Actions
  * regexpr: `/regexpr/`匹配substr；`$4~/regexpr/`匹配substr，`$4 !~/regexpr/`排他匹配substr
  * Built-in Arithmeic Functions: atan2, cos, exp, log, rand, sqrt, srand
  * string
    * `index("banana", "an")` = 1
    * `match("banana", "an")`, RSTART = 1, RLENTH = 2
    * `split(s, a, fs)`
    * `gsub(r, s), gsub(r, s, t), sub(r, s), sub(r, s, t)`
    * `substr(s, p), substr(s, p, n)`
    * `sprintf, printf`
  * `number""`使数字coerce为string，`string + 0`使string coerce为数字；数字默认输出格式为`OFMT = %.2f`。
    * 字符串比较`$1 "" == $2`，数字比较`$1 + 0 == $2 + 0`
  * `for key in arr`， key**是字符串**——awk的数组类似于map、`arr[xxx]`可能会插入一个node。另外删除是用`delete arr[key]`
  * udf: 除了arrary，其他都是by-value。local variable要声明在param-list中，不然就是全局变量

