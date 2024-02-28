#include <stdio.h>

int main()
{
    int n;
    scanf("%d", &n);
    int arr[n];
    int target, times = 0;
    for (int i = 0; i < n; i++)
    {
        int d;
        scanf("%d", &d);
        arr[i] = d;
    }
    int i = 0;
    int swap;
    while (i < n - 1)
    {
        if (arr[i] > arr[i + 1])
        {
            swap = arr[i];
            arr[i] = arr[i + 1];
            arr[i + 1] = swap;
            i = -1;
        }
        i++;
    }
    i = 0;
    while (i < n - 1)
    {
        if (arr[i])
    }
}