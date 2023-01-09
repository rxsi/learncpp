#include <bits/stdc++.h>
using namespace std;

 // This is the interface that allows for creating nested lists.
 // You should not implement it, or speculate about its implementation
class NestedInteger {
public:
    // Return true if this NestedInteger holds a single integer, rather than a nested list.
    bool isInteger() const;
    // Return the single integer that this NestedInteger holds, if it holds a single integer
    // The result is undefined if this NestedInteger holds a nested list
    int getInteger() const;
    // Return the nested list that this NestedInteger holds, if it holds a nested list
    // The result is undefined if this NestedInteger holds a single integer
    const vector<NestedInteger> &getList() const;
};




class NestedIterator {

private:
    int index = 0;
    int count = 0;
    vector<NestedInteger> _nestedList;
    unordered_map<int, NestedIterator*> myMap;

public:
    NestedIterator(vector<NestedInteger> &nestedList) {
        _nestedList.assign(nestedList.begin(), nestedList.end());
        cout << "init" << endl;
    }
    
    int next() {
        NestedInteger nest = _nestedList[index];
        while (!nest.isInteger())
        {
            cout << "in while: " << index << endl;
            if (myMap.find(index) == myMap.end()) myMap[index] = new NestedIterator(nest.getList());
            if (myMap[index]->hasNext()) return myMap[index]->next();
            else nest = _nestedList[index + 1];
        }
        cout << "is_integer" << endl;
        index++;
        return nest.getInteger();
    }
    
    bool hasNext() {
        if (index < _nestedList.size()) return true;
        NestedInteger nest = _nestedList[index];
        if (nest.isInteger()) return false;
        return myMap[index]->hasNext();
    }
};


int main()
{
    // nestedList = [[1,1],2,[1,1]]
}