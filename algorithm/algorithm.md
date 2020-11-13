# 杂项

* [快速求](https://en.wikipedia.org/wiki/Fast_inverse_square_root)。 $\frac {1}{\sqrt{x}}$。

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

* KMP, [Shortest Palindrome](https://leetcode.com/problems/shortest-palindrome/submissions/)

  ```rust
  pub fn shortest_palindrome(s: String) -> String {
          fn get_prefix_table(s: &[u8]) -> Vec<usize> {
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

* [Number of 1 Bits](https://leetcode.com/problems/number-of-1-bits/):MIT HAKMEM。

  * $$
    n^k \% (n-1) == 1
    $$

  * 类似的有[ Add Digits](https://leetcode.com/problems/add-digits/)。


# dilworth/skyline

* U的A划分的最少集合数==U的B链最大长度。——最少集合最大长度
* longest increasing substring:
  * lis中每个元素代表一条反链: 定义偏序关系:`i < j && nums[i] < nums[j]`。放入新元素时`i<j && nums[j] <= *lower_bound`形成反链。
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

# DP

* 四边形不等式：

  * $$
    dp[i,j] = min_{k=i}^{j-1}(dp[i, k]+dp[k+1,j]) + cost[i,j]
    $$

  * 如果cost[i,j]满足关于区间的单调关系对于a<=b<=c<=d, cost[b,c] <= cost[a,d]

  * 且cost满足四边形不等式：交叉小于包含，即a<=b<=c<=d, cost[a,c] + cost[b,d] <=cost[a,d] + cost [b,c]

  * 则dp满足四边形不等式，且最优决策点s[i, j-1] <= s[i,j] <= s[i+1,j]，即某一端点移动后，决策点不会左移

  * 复杂度：按len = j-i来枚举，每次枚举s[i,j]只会右移且最多到n，故复杂度为N^2.

# Graph

* 《挑战》P110

* Connected Component：

  * 使用dfs（复杂），或者UnionFind（简单）
  * 有向图中：Korasaju：以G^R^的拓扑序在G中dfs。对于s-v，拓扑序保证v→s，dfs保证s→v 

* topology sort: 

  * 逆后序(dfs结束时入栈)：可保证v->w时，dfs~w~ 在dfs~v~前结束
  * chop online: 找到indgree为1的点，更新neighbour。会修改node的indgree。
  * 优先级：如果两个任务之间有时间限制，如a在b完成后t时间内完成，则在a完成节点向b完成节点加一条负边-t。

* cyclic: 有向图中，dfs，并用数组onStack表示在当前路径上（多路径，如树）；marked表示是否访问过（剪枝）；二者都为真则为环。

* | Prim           | 加权有向图mst                  | 从堆中取出，判断是否是横切边，再判断是否要更新pq(insert或略过)。空间V时间ElogV |
  | -------------- | ------------------------------ | ------------------------------------------------------------ |
  | Kruskal        |                                | 空间E，时间ElogE。简单，可以字典序语义，需要并查集。         |
  | Dijkstra       | 加权无向图最短路径（正边可环） | 从一点开始，遍历邻边，再判断是否是否要更新pq（insert或略过）。空间V时间ElogV |
  |                | 无环优化                       | 拓扑序松弛。可有负边。空间V时间E+V                           |
  | Bellman-Ford   | 可有负边的最短路径、负边检测   | 从一点开始，遍历邻边，再判断是否是否要更新q。空间V时间EV     |
  | Floyd-warshall | 任一点最短路径，允许负边       | dp\[i, j]=min(dp[i, k] + dp[k, j])                           |

```c++
// Prim：加权有向图mst。堆pq、marked判断是否在mst中,依据为distTo
// distTo存储节点路径长度（初始化INT_MAX）
// edgeTo存储前驱节点(不用初始化)
pq.insert(G, startNode);
while(pq.empty()) visit(G,pq.pop());

visit(vertex v){// using vertex = int
  for (vertex w : v.adj)
    if !marked[w] && weight(v,w) < distTo[w]
      edgetTo[w] = v
      distTo[w] = weight(v,w)
      w是否在pq中? update(w, distTo[w]):insert(w, distTo[w])
}
//Korasaju，从所有边中，筛选端点不构成环(即不位于同一集合，使用并查集实现)
while (!pq.empty && mst.size < V-1) {
  edge e = pq.pop;
  vertex v = e.either, w = e.other(v);// using vertex = int
  if (uf.connectied()) continue;
  uf.union(v,w);
  mst.enque(e);
}
// Dijkstra
pq.insert(G, startNode);
while(pq.empty()) relax(G,pq.pop());

relax(vertex v) {// using vertex = int
  for (Edge e: v.adj) {
    vertex w = e.to()
    if distTo[w] > distTo[v] + e.weight 
      distTo[w] = distTo[v] + e.weight
      edgeTo[w] = e
      w是否在pq中? update(w, distTo[w]):insert(w, distTo[w])
      // 由于c++的pq实现无法支持update，所以可以选择直接insert
      // 并在前面的while的pq.pop时，检查pop的p,
      // 若distTo[p.first] < p.second则表明失效，丢弃
  }
}
// Bellman-Ford
while (true) {
  bool update = false;
  for (auto e:edges) {
    if d[e.from]!= INF && d[e.to] > d[e.from] + e.cost {
      d[e.to] = d[e.from] + e.cost;
      update = true;
    }
  }
  if (!update) break;
}
//tarjon
```




# 刷题进度

* [math](https://leetcode.com/tag/math/): 69
* [linked-list](https://leetcode.com/tag/linked-list/):92
* [tree](https://leetcode.com/tag/tree/): 98
* all
  * easy：260
  * Medium：540
  * Hard：400


$$
O\!\left( \sqrt{\text{n_rows} \cdot \text{n_cols}}\, \right)
$$


