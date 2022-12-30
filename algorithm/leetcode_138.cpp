#include <bits/stdc++.h>
using namespace std;

// Definition for a Node.
class Node {
public:
    int val;
    Node* next;
    Node* random;
    
    Node(int _val) {
        val = _val;
        next = NULL;
        random = NULL;
    }
};


class Solution {
public:
    Node* copyRandomList(Node* head) {
        unordered_map<Node*, Node*> myMap;
        Node* dummyHead = head;
        while (dummyHead)
        {
            myMap[dummyHead] = new Node(dummyHead->val);
            dummyHead = dummyHead->next;
        }
        dummyHead = head;
        while (dummyHead)
        {
            myMap[dummyHead]->next = myMap[dummyHead->next];
            myMap[dummyHead]->random = myMap[dummyHead->random];
            dummyHead = dummyHead->next;
        }
        return myMap[head];
    }
};

int main()
{
    Node * node7 = new Node(7);
    Node * node13 = new Node(13);
    Node * node11 = new Node(11);
    Node * node10 = new Node(10);
    Node * node1 = new Node(1);
    node7->next = node13;
    node7->random = NULL;
    node13->next = node11;
    node13->random = node7;
    node11->next = node10;
    node11->random = node1;
    node10->next = node1;
    node10->random = node11;
    node1->next = NULL;
    node1->random = node7;
    Solution s;
    Node* res = s.copyRandomList(node7);
    cout << "23" << endl;
    cout << res->val << endl;
}