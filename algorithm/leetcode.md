# 总览

* Pair hash可以参考353. 更一般的情况是hash combine任意个字段
* 406: 挺有意思的，sort+bit
* [81 Search in Rotated Sorted Array II](https://leetcode.com/problems/search-in-rotated-sorted-array-ii/description/)  和 [154. Find Minimum in Rotated Sorted Array II](https://leetcode.com/problems/find-minimum-in-rotated-sorted-array-ii/description)  可以对比起来看。前者注意力在target上，所以binary search时, nums[l], nums[m], nums[r]注重找单调区间；后者注意力在minimum上，所以注重不单调区间

* TODO

  * https://leetcode.com/problems/word-ladder-ii/submissions/。这个问题可能需要计算下最短路径。回去翻一下相关代码，整理下dijkstra、Floyd、Bellman-Ford之类的。顺道找下以前的笔记

* 代码片段

  ```c++
  class UnionFind {
     public:
      explicit UnionFind(int nodes) {
        groups_ = nodes;
        rank_.resize(nodes, 1);
        parents_.resize(nodes);
        iota(parents_.begin(), parents_.end(), 0);
      }
  
      void Union(int node_a, int node_b) {
        auto parent_a = Find(node_a);
        auto parent_b = Find(node_b);
        if (parent_a == parent_b) {
          return;
        }
  
        --groups_;
        if (rank_[parent_a] < rank_[parent_b]) {
          parents_[parent_a] = parent_b;
        } else {
          parents_[parent_b] = parent_a;
          if (rank_[parent_a] == rank_[parent_b]) {
            ++rank_[parent_a];
          }
        }
      }
   int Groups() const { return groups_; }
  
     private:
      int Find(int node) { return parents_[node] == node ? node : Find(parents_[node]); }
  
      int groups_ = 0;
      vector<int> parents_;
      vector<int> rank_;
  };
  ```

* 

* TODO
  * BIT 实现
  * topology排序实现

### 刷题进度：

all：

* M：340-356(finish)

* H：300-

* LeetCode 101
  * 455,135,435,605,763,122,406,665
  * 167,88,142,76,633,680,524,340
    * 1216,2330
  * 69,34,81
    * 82,1836, 154, 540,4 
  * 215,347,451,75
  * 695, 547, 417, 46, 47, 77, 79, 51,934,,126,130,257,40, 37,310
  * DP:70, 198, 413, 64,542，221,279,,91,139,300，1143,416,474,322
  * Chapter 7.7: 72,650,10, 53,343, 587, 712， 646,376,494
  * chapter 8: 241,932,312
  * Chapter 9:204, 504,172,415,326,384, 528,382，168,67,238,462,169,229,470,202
  * Bit: 461,190,136,342,318,268,693,476,260
  * stl:
    * array 48,241,769
    * Stk: 232,155,20,739
    * Pq:23,218,239
    * hash: 1, 149,332,
    * Trie:303,304,560,566,225,503,217,697,594,287,313,870,397,307
  * string: 242,647,696,227,28,409,3,772,5
  * List: 206, 21,24,160,234,83,328,19
  
  
  
  

* TODO
  * morris实现
  * reservoir sampleing
  * * 



Rust

* M:

linked list：

* trick

  * ```
    auto a = [](){
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);
    }();
    ```

  * 

353. Design Snake Game 

     ```c++
     // https://youngforest.github.io/2020/05/27/best-implement-to-use-pair-as-key-to-std-unordered-map-in-C/
     template <typename T>
     inline void hash_combine(std::size_t& seed, T const& val) {
       seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
     }
     
     template <typename... Types>
     inline std::size_t hash_val(const Types&... args) {
       std::size_t seed = 0;
       (..., hash_combine(seed, args));
       return seed;
     }
     
     class SnakeGame {
      public:
       SnakeGame(int width, int height, vector<vector<int>>& food)
           : food_(food), max_width_(width), max_height_(height) {}
     
       int move(string const& direction) {
         if (direction.size() != 1) {
           return -1;
         }
         auto next_head = body_.front();
         switch (direction[0]) {
           case 'R': {
             next_head.x += 1;
             break;
           }
           case 'L': {
             next_head.x -= 1;
             break;
           }
           case 'D': {
             next_head.y += 1;
             break;
           }
           case 'U': {
             next_head.y -= 1;
             break;
           }
           default: {
             cout << "bad direction" << direction;
             break;
           }
         }
     
         if (HitBoundryOrBody(next_head)) {
           return -1;
         }
     
         UpdateBody(next_head);
         return body_.size() - 1;
       }
     
      private:
       struct Coordinate {
         int x = 0;
         int y = 0;
     
         bool operator==(Coordinate const& other) const { return x == other.x && y == other.y; }
       };
     
       void UpdateBody(Coordinate const& new_head) {
         constexpr size_t kX = 1, kY = 0;
         if (auto [x, y] = new_head;
             cur_food_ < food_.size() && x == food_[cur_food_][kX] && y == food_[cur_food_][kY]) {
           ++cur_food_;
           body_.push_front(new_head);
           body_set_.insert(new_head);
         } else {
           body_set_.erase(body_.back());
           body_.pop_back();
           // care: edge case
           body_.push_front(new_head);
           body_set_.insert(new_head);
         }
       }
     
       bool HitBoundryOrBody(Coordinate const& new_head) {
         auto [x, y] = new_head;
         // care: edge case
         if (new_head == body_.back()) {
           return false;
         }
         return x < 0 || x >= max_width_ || y < 0 || y >= max_height_ ||
                body_set_.end() != body_set_.find(new_head);
       }
     
       struct pair_hash {
         std::size_t operator()(const Coordinate& p) const { return hash_val(p.x, p.y); }
       };
     
       deque<Coordinate> body_{{0, 0}};
       unordered_set<Coordinate, pair_hash> body_set_{{0, 0}};
       vector<vector<int>> const& food_;
       size_t cur_food_ = 0;
       int max_width_ = 0;
       int max_height_ = 0;
     };
     ```
     
354. Russian Doll Envelopes

     ```c++
     class Solution {
      public:
       int maxEnvelopes(vector<vector<int>>& envelopes) {
         // dilworth
         // 1. sort by doll[0], we can guarantee doll_a[0] <= doll_b[0]
         // 2. now, for a anti-chain, we need to find:
         //   a. if d_a[0] == d_b[0], any d_a[1] and d_b[1] is okay
         //   b. if d_a[0]  < d_b[0], d_a[1] >= d_b[1]
         // 3. for 2.b, this can be done by sort by doll[1] descendingly, and binary search
         sort(envelopes.begin(), envelopes.end(),
              [](auto const& a, auto const& b) { return a[0] < b[0] || a[0] == b[0] && a[1] > b[1]; });
     
         vector<int> lis;  // longest increasing sequence
         for (auto const& i : envelopes) {
           // optimize
           if (!lis.empty() && i[1] > lis.back()) {
             lis.push_back(i[1]);
             continue;
           }
     
           if (auto it = lower_bound(lis.begin(), lis.end(), i[1]); it != lis.end()) {
             *it = i[1];
           } else {
             lis.push_back(i[1]);
           }
         }
         return lis.size();
       }
     };
     ```

     

355. 

