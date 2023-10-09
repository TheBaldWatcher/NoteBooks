[TOC]



# I. Performance Engineering

## 2.Couputer Architecture

### 2.2 Assembly Lauguage

* Syntax: AT&T相比Intel:
  * The *last* operand is used to specify the destination.
  * 操作符有suffix：
    * b=byte(8b)
    * w=word(16b)
    * l=long(32b int/64b float)
    * q=quad(64b)
    * s=single(32b float)
    * t=ten-bytes(80b float)

### 2.6 Machine Code Layout

* 代码需要对齐，但是还得考虑到instruction cache——hot code和hot code放一起，cold code和cold code放一起。 check out Facebook’s [Binary Optimization and Layout Tool](https://engineering.fb.com/2018/06/19/data-infrastructure/accelerate-large-scale-applications-with-bolt/), which was recently [merged](https://github.com/llvm/llvm-project/commit/4c106cfdf7cf7eec861ad3983a3dd9a9e8f3a8ae) into LLVM.
* branch predication
  * likely只是改变了code layout，硬件层面有自己的统计器。likely的收益似乎只有不到1cycle
  * 如果数据太少，硬件层面甚至能把顺序记录下来？
  * branchless即同时计算两个分支，好处是开销恒定，在不确定度很高时（如25%-75%）比分支预测好，当然还得看分支计算量进行调整
    * 从controlle hazard变成了data hazard。虽然pipeline还是会被stall，但是开销更小，有专门的指令，比如cmov
  * 
* 