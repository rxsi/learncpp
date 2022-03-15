#include <vector>
using namespace std;

class Solution {
public:
    int peakIndexInMountainArray(vector<int>& arr) {
        int left = 0;
        int n = arr.size();
        int right = n - 1;

        while (left < right)
        {
            int mid = left + (right - left) / 2;
            int num = arr[mid];
            if (num < arr[mid+1])
            {
                left = mid + 1;
            } 
            else if (num < arr[mid-1])
            {
                right = mid - 1;
            }
            else
            {
                return mid;
            }
        }
        return left;
    }
};


int main()
{
    Solution s;
    vector<int> vec{3,5,3,2,0};
    s.peakIndexInMountainArray(vec);
}