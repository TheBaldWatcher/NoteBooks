# 基础

## 缩写

* cs  code segemnt
* cp code pointer
* ds data segment
* cx 循环
* define xx
  * dw define word
  * db define binary
  * dd define doubel word
  * dup: `db 3 dup (0,1,2)` 相当于`db 0,1,2,0,1,2,0,1,2`
* sp stack pointer
* ss stack segment

## 记法

* 只有bx, si, di, bp可以进行寻址

* bx默认段地址为ds，bp默认段地址ss

* 取址

  * ```
    [bi].10.[si]
    [bi].10
    10[bi]
    [bi+si]
    [bi][si]
    cs:[bi]
    ```

* 操作符 word/byte ptr 指明内存大小是字还是字节

* div

  * ```
    # a / b
    # 如果b为8位，a则为16位
    ax:a -> al: 商, ah: 余
    # 如果b为16位，a则为32位
    dx+ax:a(高+低) -> ax：商，dx：余
    ```

## 转移指令

* offset s 标号s的地址