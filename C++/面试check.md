* shared_ptr线程安全性：ptr本身安全，被指向对象不安全
* 循环引用：weak_ptr。另外control block自己的释放要等到weak_count为0，所以可能T的内存并不是立即释放的（二者放在一块儿的话）
* 《Linux多线程服务器编程》附录D关于TCP的思考题