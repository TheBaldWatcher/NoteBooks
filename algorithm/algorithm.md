# 杂项

* [快速求](https://en.wikipedia.org/wiki/Fast_inverse_square_root)。$\frac {1}{\sqrt{x}}$。

* 二分搜索

  ```C++
  int l = 0, r = static_cast<int> (arr.size()) - 1;
  while (l < r) {
    int m = l + ((r - l) >> 1);
    if (arr[m] < target) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  return arr[l] == target;
  ```

* 集合操作

  ```c++
  // k-subset of N
  int flag = (1 << k) - 1, limit = 1 << N;	// 注意 N >= 32时不成立
  while (flag < limit) {
    cout<<flag<<endl;
    int last_one = flag & -flag;
    int head = flag + last_one;
    int tail = (flag & ~head)/ last_one >> 1;
    flag = head | tail;
  }
  
  // all subset
  ```




# 树

* 二叉树重建:[pre_order+in_order](https://leetcode.com/problems/construct-binary-tree-from-inorder-and-postorder-traversal/)、[in_order+post_order](https://leetcode.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/)。