#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;
class Solution{
public:
	vector<int> twoSum(vector<int>& nums, int target){
		vector<int>::iterator iter;
		vector<int> ret;
		for (vector<int>::iterator iter = nums.begin(); iter != nums.end(); iter++)
		{
			for (vector<int>::iterator iter2 = iter + 1; iter2 != nums.end(); iter2++)
			{
				if ((*iter + *iter2) == target)
				{
					ret.push_back(*iter);
					ret.push_back(*iter2);
					return ret;
				}
			}
		}
		return ret;
	}
};

int main(){
	Solution test;
	int array[] = {1, 3, 7, 19, 36, 48, 75, 99, 103};
	vector<int> nums(array, array + sizeof(array) / sizeof(int));
	vector<int> ret = test.twoSum(nums, 118);
	for (vector<int>::iterator iter = ret.begin(); iter != ret.end(); iter++)
	{
		printf("the num:%d\n", *iter);
	}
	return 0;
}
