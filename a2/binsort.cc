
// -- BINARY INSERTION SORT NORMAL IMPLEMENTATION -- 
// C program for implementation of binary insertion sort 
#include <stdio.h> 
  
// A binary search based function to find the position 
// where item should be inserted in a[low..high] 
int binarySearch(int a[], int item, int low, int high) 
{ 
    if (high <= low) 
        return (item > a[low])?  (low + 1): low; 
  
    int mid = (low + high)/2; 
  
    if(item == a[mid]) 
        return mid+1; 
  
    if(item > a[mid]) 
        return binarySearch(a, item, mid+1, high); 
    return binarySearch(a, item, low, mid-1); 
} 
  
// Function to sort an array a[] of size 'n' 
void insertionSort(int a[], int n) 
{ 
    int i, loc, j, k, selected; 
  
    for (i = 1; i < n; ++i) 
    { 
        j = i - 1; 
        selected = a[i]; 
  
        // find location where selected sould be inseretd 
        loc = binarySearch(a, selected, 0, j); 
  
        // Move all elements after location to create space 
        while (j >= loc) 
        { 
            a[j+1] = a[j]; 
            j--; 
        } 
        a[j+1] = selected; 
    } 
} 


// -- ALTERNATIVE BINARY SORT IMPLEMENTATION 
#include <iostream>
using namespace std;
int binarySearch(int arr[], int item, int low, int high) {
   if (high <= low)
      return (item > arr[low])? (low + 1): low;
      int mid = (low + high)/2;
   if(item == arr[mid])
      return mid+1;
   if(item > arr[mid])
      return binarySearch(arr, item, mid+1, high);
      return binarySearch(arr, item, low, mid-1);
}
void BinaryInsertionSort(int arr[], int n) {
   int i, loc, j, k, selected;
   for (i = 1; i < n; ++i) {
      j = i - 1;
      selected = arr[i];
      loc = binarySearch(arr, selected, 0, j);
      while (j >= loc) {
         arr[j+1] = arr[j];
         j--;
      }
      arr[j+1] = selected;
   }
}
int main() {
   int arr[] = {12, 56, 1, 67, 45, 8, 82, 16, 63, 23};
   int n = sizeof(arr)/sizeof(arr[0]), i;
   BinaryInsertionSort(arr, n);
      cout<<"Sorted array is : \n";
   for (i = 0; i < n; i++)
      cout<<arr[i]<<"\t";
   return 0;
}