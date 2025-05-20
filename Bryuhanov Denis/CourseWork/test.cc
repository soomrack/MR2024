//#include <bits/stdc++.h>
#include <iostream>
#include <chrono> 
#include "Red-Black-Tea.h"

#include <vector>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <ctime>

std::vector<int> generateUniqueNumbers(int n) {
    std::vector<int> arr;

    for (int i = 0; i < n; i++){
        arr.push_back(i);
    }

    std::vector<int> result;
    while (arr.size() > 0)
    {
        std::mt19937 gen(time(0));
        std::uniform_int_distribution<size_t> dist(0, arr.size() - 1);
        
        int arr_index = dist(gen);
        result.push_back(arr[arr_index]);
        arr.erase(arr.begin() + arr_index);
    }
    
    return result;
}

int main()
{
  int n;
  std::cout << "Insert Tree size: ";
  std::cin >> n;
  
  Tree t;
  int key;
  int val;
  
  auto start = std::chrono::high_resolution_clock::now();

  std::vector<int> Massive = generateUniqueNumbers(n);
  
  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); 
  std::cout << "Time taken by generate Unique Numbers: " << duration.count() << " microseconds" << std::endl;


  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < n; i++)
  {
    key = i;
    val = i;
    t.insert(key, val);
  }
  end = std::chrono::high_resolution_clock::now();
  
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); 
  std::cout << "Time taken by generating Tree: " << duration.count() << " microseconds" << std::endl;


  start = std::chrono::high_resolution_clock::now();
  key = 1;
  val = -1;
  if (t.search(key, val)) {
    end = std::chrono::high_resolution_clock::now();
    std::cout<<"Search result: "<<val<<std::endl;
  }
    
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); 
  std::cout << "Time taken by searching key: " << duration.count() << " microseconds" << std::endl;
 
  
  key = n-1;
  
  start = std::chrono::high_resolution_clock::now();

  t.remove(key);

  end = std::chrono::high_resolution_clock::now();
  
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); 
  std::cout << "Time taken by removing key: " << duration.count() << " microseconds" << std::endl;
  
  return 0;
}
