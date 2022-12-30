#include <iostream>
#include <vector>
using namespace std;

class Trie
{
public:
    vector<Trie*> next;
    vector<int> indexOf;

    Trie(): next(26, nullptr){}
    void insert(string& small, int index)
    {
        cout << "small = " << small << endl;
        Trie* cur = this;
        for (int i = 0; i < small.size(); ++i)
        {
            int pos = small[i] - 'a';
            if (cur->next[pos] == nullptr) cur->next[pos] = new Trie();
            cur = cur->next[pos];
        }
        cur->indexOf.push_back(index);
    }
};

class Solution {
public:
    vector<vector<int>> multiSearch(string big, vector<string>& smalls) {
        Trie* root = new Trie();
        for (int i = 0; i < smalls.size(); ++i) root->insert(smalls[i], i);
        vector<vector<int>> res(smalls.size());
        for (int i = 0; i < big.size(); ++i)
        {
            Trie* cur = root;
            for (int j = i; j < big.size(); ++j)
            {
                cout << "big = " << big[j] << " i = " << i << endl;
                int pos = big[j] - 'a';
                cur = cur->next[pos];
                if (cur == nullptr) break;
                if (!cur->indexOf.empty())
                {
                    cout << "not empty = " << big[j] << endl;
                    for (auto& index: cur->indexOf)
                    {
                        res[index].push_back(i);
                    }
                }
            }
        }
        return res;
    }
};

int main()
{
    Solution s;
    vector<string> small{"is","ppi","hi","sis","i","ssippi"};
    string big{"mississippi"};
    vector<vector<int>> res = s.multiSearch(big, small);
    return 0;
}