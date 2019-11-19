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



# String

* KMP, sunday

  ```rust
  pub fn shortest_palindrome(s: String) -> String {
          fn get_prefix_table(s: &[u8]) -> Vec<usize> {
              // the values in table is 1-based index, in order to use `size_t`
              // so `1` correspond to `s[0]`.
              // Or, `l` correspond to `s[l-1]`, vice versa `s[l]` correspond to `l+1`
              let mut table = vec![0; s.len()];
              let mut left = 0;
              for i in 1..s.len() {
                  while left > 0 && s[left] != s[i] { left = table[left - 1]; }
                  if s[left] == s[i] { left += 1; }
                  table[i] = left;
              }
              table
          }
  
          let table = get_prefix_table(&s.as_bytes());
          let mut left = 0;
          let s_vec = s.as_bytes();
          for i in (0..s_vec.len()).rev() {
              while left > 0 && s_vec[left] != s_vec[i] { left = table[left - 1]; }
  
              if s_vec[left] == s_vec[i] {  left += 1; }
          }
    		// do something
  }
  ```

  


# 树

* 二叉树重建:[pre_order+in_order](https://leetcode.com/problems/construct-binary-tree-from-inorder-and-postorder-traversal/)、[in_order+post_order](https://leetcode.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/)。



# bit

* 大规模bit mask[217. Contains Duplicate](https://leetcode.com/problems/contains-duplicate/)。

  ```c++
  // log(8 * n) = 5 or 6    ln8+ lnN = 3 + lnN = 5 or 6
  constexpr size_t kOffset = log2(sizeof(size_t));
  constexpr size_t kMask = (1<<kOffset) - 1;
  size_t num = static_cast<size_t>(i);
  size_t key = num >> kOffset;
  size_t val = 1 << (num & kMask);
  hash_map[key_val.first] |= key_val.second;	// set
  hash_map[key_val.first] & key_val.second;	// test
  ```

  