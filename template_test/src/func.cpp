
template <typename T> void Swap(T &a, T &b)
{
    T temp = a;
    a = b;
    b = temp;
}

void bubble_sort(int arr[], int n)
{
    for (int i = 0; i < n-1; i++)
    {
        bool isSorted = true;
        for (int j = 0; j < n-1-i; j++)
        {
            if (arr[i] > arr[j+1])
            {
                isSorted = false;
                Swap(arr[j], arr[j+1]);
            }
        }
        if (isSorted) break;
    }
}

template void Swap(double &a, double &b);
