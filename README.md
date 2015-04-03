# xlinq
A simply implementation to support LINQ like operations in C++. This library is tested mainly using <vector> as an example.

Supported STL Containers:
- <vector>
- <list>
- <deque>
- <map>
- <tr1/unordered_map>
- <set>
- <tr1/unordered_set>
Note: If it is compiled by C++ 11, <forward_list>, <unordered_map> and <unordered_set> can also be supported.

Limitation:
- Each query can have only one Select
- Select should always be placed after Where
- I am not guarantee all features work on others. For example, Sum() for unordered_set may be a problem because pair<TKey, TValue> may not define operator +().

Check src/main.cpp for examples. Enjoy it!
