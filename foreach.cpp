#include<vector>
#include<iostream>
using namespace std;
int main(){

    vector<vector<bool>> dp;
    int n = 5;
    dp.assign(n, vector<bool>(n, true));

    for (const auto& v : dp){
        for (const auto& r : v){
            cout << r << " ";
        }
        cout << endl;
    }

    cout << "another test" << endl;

    bool arr[n][n];
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            arr[i][j] = true;
        }
    }

    for (auto& v : arr){
        for (auto& r: v){
            cout << r << " ";
        }
        cout << endl;
    }

}