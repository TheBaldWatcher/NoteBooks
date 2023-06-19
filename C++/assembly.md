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

* mul
  * ```
    mul [address]
    mul reg
    # a * b， 都为8或者16位。一个默认放在al或者ax中。结果放在ax或者dx+ax（高+低）
    ```

## 转移指令

* offset s 标号s的地址
* jmp
  * ```
    jmp short 标号
    jmp far ptr 标号
    jmp 16位reg
    jmp word ptr 内存地址——段内转移
    jmp dword ptr 内存单元--段间转移，[IP,CS]
    jcxz label, 如果cx==0则跳转到label
    loop label, 先cx-=1, 如果cx!=0则跳转。即cx==1时break
    //
    //
    je    zf == 1
    jne   zf == 0
    jb    cf == 1
    jnb   cf == 0
    ja    cf == 0 && zf == 0
    jna   cf == 1 || zf == 1
    ```

## call和ret

* call&callf
  * call：
    * ```
      call label: pop IP-next, jmp near ptr label
      call far ptr label:         push CS, push IP-next,  jmp far ptr label
      call 16-reg:                push next-IP,           jmp 16-reg
      call word ptr [address]:    push IP-next,           jmp word ptr [address]
      call dword ptr [address]:   push CS, push IP-next,  jmp dword ptr [address]
      ```
* ret&reft
  * ret: pop IP
  * retf: pop IP, pop CS

## 标志寄存器

* ZF：zero flag。为0置1
* PF: 奇偶flag。偶数置1
* SF: sign flag。负数置1
* CF: 无符号-溢出
* OF: 有符号-溢出
* DF: movsb/movsw0：si、di递增；1：si、di递减。movsb单位1、movsw单位2。cld置零，std置1。用法mov cx,16;rep movsb

## x86_64

* 寄存器
```
rax, 函数返回值
rsp, 栈顶指针， miscellaneous
rbp, 栈帧指针
rdi, rsi, rdx, rcx, r8, r9, 参数1-6
r10-15, miscellaneous
rip, 32bit pc指针，存放下一条运行的指令
```
* 指令
```
lea, 获取内存地址
```