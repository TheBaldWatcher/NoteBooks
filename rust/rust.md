Option:

* 可以用is_some来判断非None。另外链表遍历时，`Option<Box<T>>`会由于move导致问题，解决办法是加一个as_ref。参考LeetCode：rotate list/



String & Vec & Slice

* vec转String:`res.into_iter().map(|e| e.to_string()).collect::<String>()`
* iota：`(1..n as i32).collect::<Vec<i32>>()`
* 內积：`(1..n as i32).product::<i32>();`



技巧：

* 计算字符流，最后一个单词长度：`s.chars().rev().skip_while(|&e| e == ' ').take_while(|&e| e != ' ').count() as i32`

