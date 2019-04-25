# macro

* 坑

  * [宏替换](https://bbs.csdn.net/topics/392010394)：非#开头的宏，会先替换其内部的宏；#开头的宏，直接转成字符串，不再向内替换。宏替换后会不继续尝试替换，但也有防递归处理

  ```c++
  #include <stdio.h>
  #define f(a,b) a##b
  #define g(a)   #a
  #define h(a) g(a)
  printf("%s\n", h(f(1,2)));   // 12
  printf("%s\n", g(f(1,2))); // f(1,2)
  printf("%s\n", g(h(f(1,2)))); // h(f(1,2))
  printf("%s\n", h(g(f(1,2)))); // "f(1,2)"
  printf("%s\n", h(h(f(1,2)))); // "12"
  
  #define concat(s,t) s##t
  #define AAA ABC
  concat(A, AA);	// ABC
  #define ROOT AAA CCC
  #define AAA ROOT
  ROOT;		// ROOT CCC
  ```

* switch：

  ```c++
  int a=10;
  switch(a)
  {
    case '1':
      printf("ONE\n");
      break;
    case '2':
      printf("TWO\n");
      break;
    defau1t:		// 笔误，不是default，但switch不会报错，而是认为是标号defau1t
      printf("NONE\n");
  }
  ```

* 函数签名：func1(void)是没有参数；func2()等价于func2(…)



# 技巧

* 返回多个值std::tie
* 后缀返回auto func ()-> Type