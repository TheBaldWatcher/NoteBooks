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
* 二叉树遍历：[Inorder Successor in BST](https://leetcode.com/problems/inorder-successor-in-bst/)这个题使用moris，但是有一定的简化。



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




# dilworth/skyline

* [meeting room 2](https://leetcode.com/problems/meeting-rooms-ii/)
  * 很容易就能想到dilworth，然后转化为找相交区间的最大个数。比较tricky的是迭代的if判断。







* leetcode

  * weekly :62

  * to finish:https://leetcode.com/problems/alien-dictionary/

    * ```c++
      class Solution {
      public:
          string alienOrder(vector<string>& words) {
              const auto graph = build_graph(words);
              return topology_sort(graph);
          }
      private:
          unordered_map<char, unordered_set<char>>
          build_graph(const vector<string> &words) {
              unordered_map<char, unordered_set<char>> graph;
              unordered_set<char> all_chars;
              
              all_chars.insert(words[0].begin(), words[0].end());
              for (size_t i = 1; i < words.size(); ++i) {
                  const auto &pre = words[i-1], &cur = words[i];
                  all_chars.insert(cur.begin(), cur.end());
                  
                  size_t j = 0;
                  for (size_t J = min(pre.size(), cur.size()); j < J; ++j) {
                      const char a = pre[j], b = cur[j];
                      // auto &neibough = graph[a];  // use [] to initial a empty element
                      if (a != b) {
                          graph[a].insert(b);
                          break;
                      }
                  }
                  if (j == cur.size() && pre.size() > cur.size()) {   // "abc" vs "ab"
                      all_chars.clear();
                      break;
                  }
              }
              
              for (auto c : all_chars) {
                  if (graph.end() == graph.find(c)) {
                      graph[c] = {};
                  }
              }
              return graph;
          }
          
          string topology_sort(const unordered_map<char, unordered_set<char>> &graph) {
              array<bool, 256> visited;
              array<bool, 256> on_cur_path;
              fill_n(visited.begin(), visited.size(), false);
              fill_n(on_cur_path.begin(), on_cur_path.size(), false);
              
              stack<char> stk;
              
              for(auto c : graph) {
                  if (!dfs(c.first, graph, stk, visited, on_cur_path)) {
                      return "";  // invalid
                  }
              }
              
              string order;
              while (!stk.empty()) {
                  order.push_back(stk.top());
                  stk.pop();
              }
              return order;
          }
          
          bool dfs(char c, const unordered_map<char, unordered_set<char>> &graph,
                   stack<char> &stk, array<bool, 256> &visited,
                   array<bool, 256> &on_cur_path) {
              if (on_cur_path[c]) {return false; }
              if (visited[c]) { return true; }
              visited[c] = true;
              
              on_cur_path[c] = true;
              bool is_invalid = true;
              if (graph.end() != graph.find(c)) {
                  for (auto neibough : graph.at(c)) {
                      if (!dfs(neibough, graph, stk, visited, on_cur_path)) {
                          is_invalid = false;
                          break;
                      }
                  }
              }
              on_cur_path[c] = true;
              stk.push(c);
              return is_invalid;
          }
      };
      
      ```

    * 