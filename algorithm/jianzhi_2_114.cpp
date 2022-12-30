#include <bits/stdc++.h>
using namespace std;


class Solution {
private:
    const int VISITING = 1, VISITED = 2;
public:
    string alienOrder(vector<string>& words) {
        /* 拓扑排序 + DFS
        在 BFS 中我们处理环可以借助一个counter变量，而DFS则需要标记节点的状态，有三种状态：
        1. 未被遍历（未加入到状态记录字典中）
        2. 正在被遍历（此状态指的是周边的其他点，注意是有向的，还没有都被遍历完）
        3. 被遍历完了（指的是周边的点都被遍历完了，对于一个有向图某条边的最后一个节点，只要他被遍历，之后的状态就会转为被遍历完了）
        如果在DFS过程中，又遇到了某个正在遍历的点，则说明出现环
        */
        unordered_map<char, vector<char>> wordMap; // 这里记录的是邻接单词已经确认的前后顺序
        unordered_map<char, int> states;
        for (int i = 0; i < words.size(); ++i)
        {
            for (int j = 0; j < words[i].size(); ++j) wordMap[words[i][j]] = {}; // 这个处理是为了当遇到完全相同的单词时，比如["zab", "zab"]，那么它返回的顺序可以是任意的。
        }

        for (int i = 0; i < words.size()-1; ++i)
        {
            string& word1 = words[i];
            string& word2 = words[i+1];
            int minLen = min(word1.size(), word2.size());
            int j = 0;
            for (; j < minLen; ++j)
            {
                if (word1[j] != word2[j])
                {
                    wordMap[word1[j]].push_back(word2[j]);
                    break;
                }
            }
            if (j == minLen && word1.size() > word2.size()) return ""; // 前面部分相同，但是前单词的长度大于后一个，不符合要求
        }
        // 从每个入度为0的节点开始，根据DFS会遍历所有的点，注意因为我们需要按序输出结果，但是深度优先的顺序只会遍历单一路，因此最后的结果是从后往前，所以最后的结果需要reverse。
        string res = "";
        for (auto& p: wordMap)
        {
             if (!states.count(p.first)) 
             {
                if (!dfs(states, res, p.first, wordMap)) return "";
             }
        }
        reverse(res.begin(), res.end()); // 翻转输出
        return res;
    }

    bool dfs(unordered_map<char, int>& states, string& res, char c, unordered_map<char, vector<char>>& wordMap)
    {
        states[c] = VISITING; // 正在被遍历
        for (auto& c2: wordMap[c])
        {
            if (!states.count(c2))
            {
                if (!dfs(states, res, c2, wordMap)) return false;
            }
            else if (states[c2] == VISITING) return false; // 有环
        }
        states[c] = VISITED; // 节点的所有子节点都已经遍历完，本节点转为VISTED
        res += c;
        return true;
    }
};


int main()
{
    vector<string> v{"ac","ab","zc","zb"};
    Solution s;
    string res = s.alienOrder(v);
    cout << res << endl;
}