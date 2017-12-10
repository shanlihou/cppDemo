#include "stdafx.h"

#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;
void printVecWithNum(vector<int> &vec, int start, int end)
{
	for (int i = start; i < end; i++)
	{
		printf("%d:%d ", i - start, vec[i]);
	}
	printf("\n");
}
int oddOpr1[4][2] = {{-1, 2}, {0, 1}, {-1, 1}, {-1, 2}};
int oddOpr2[4][2] = {{0, -2}, {0, -1}, {0, -1}, {-1, 2}};
class Solution{
#define MIN(x, y) (x) < (y) ? (x) : (y)
#define MAX(x, y) (x) > (y) ? (x) : (y)
#define LEFT(x) ((*nums1)[x])
#define RIGHT(x) ((*nums2)[x])
#define LEFT_D(x) ((double)((*nums1)[x]))
#define RIGHT_D(x) ((double)((*nums2)[x]))

private:
	vector<int> (*nums1), (*nums2);
	int leftStart, leftEnd, rightStart, rightEnd;
	double getRightMedian(){
		int mid = (rightStart + rightEnd) / 2;
		if ((rightEnd - rightStart) % 2 == 0)
		{
			return (RIGHT_D(mid) + RIGHT_D(mid - 1)) / 2;
		}
		else
		{
			return RIGHT_D(mid);
		}
	}

	double getLeftMedian(){
		int mid = (leftStart + leftEnd) / 2;
		if ((leftEnd - leftStart) % 2 == 0)
		{
			return (LEFT_D(mid) + LEFT_D(mid - 1)) / 2;
		}
		else
		{
			return LEFT_D(mid);
		}
	}

	int getPosFromRight(double median, int left, int right){
		while (left < right)
		{
			double mid = RIGHT_D((left + right) / 2);
			if (mid < median)
			{
				left = (left + right) / 2 + 1;
			}
			else
			{
				right = (left + right) / 2;
			}
		}
		return right;
	}

	void addRight(int lrBorder)
	{
		if (leftEnd == lrBorder)
		{
			rightEnd ++;
		}
		else
		{
			if (LEFT(leftEnd) < RIGHT(rightEnd))
			{
				leftEnd++;
			}
			else
			{
				rightEnd++;
			}
		}
	}

	void minusLeft(int lrBorder, int rrBorder)
	{
		if (leftStart == lrBorder)
		{
			rightStart++;
			return;
		}else if (rightStart == rrBorder)
		{
			leftStart++;
			return;
		}
		
		if (LEFT(leftStart) < RIGHT(rightStart))
		{
			leftStart++;
		}
		else
		{
			rightStart++;
		}
	}
	bool addLeft(int llBorder, int rlBorder)
	{
		if (leftStart == llBorder)
		{
			if (rightStart == rlBorder)
			{
				return false;
			}
			rightStart--;
		}
		else if (rightStart == rlBorder)
		{
			leftStart--;
			return true;
		}
		else
		{
			if (LEFT(leftStart - 1) > RIGHT(rightStart - 1))
			{
				leftStart--;
			}
			else
			{
				rightStart--;
			}
		}
		return true;
	}
	double recFind(){
		//printf("new:\n");
		int llBorder = leftStart, lrBorder = leftEnd, rlBorder = rightStart, rrBorder = rightEnd;
		if (leftStart >= leftEnd)
		{
			return getRightMedian();
		}
		else if (rightStart >= rightEnd)
		{
			return getLeftMedian();
		}

		int leftMidPos = (leftStart + leftEnd) / 2;
		//printf("leftStart:%d + leftEnd:%d\n", leftStart, leftEnd);

		int odd = (leftStart - leftEnd) & 1;
		if (!odd)
		{
			leftMidPos--;
		}
		int leftMid = (*nums1)[leftMidPos];

		int leftAtRightPos = getPosFromRight(leftMid, rightStart, rightEnd);
		int div1 = leftAtRightPos - rightStart;
		int div2 = rightEnd - leftAtRightPos;
		//printf("div1:%d, div2:%d\n", div1, div2);
		if (div1 < div2)
		{
			if (div2 - div1 == 1)
			{
				if (odd)
				{
					if (leftEnd - leftStart == 1)
					{
						return ((double)leftMid + RIGHT_D(leftAtRightPos)) / 2;
					}
					int min = MIN(LEFT(leftMidPos + 1), RIGHT(leftAtRightPos));
					return ((double)leftMid + (double)min) / 2;
				}
			}
			int tmpDiv = div2 - div1;
			int oddDiv = tmpDiv & 1;
			tmpDiv /= 2;
			int oddIndex = (odd << 1) + oddDiv;
			tmpDiv += oddOpr1[oddIndex][0];
			leftStart = leftMidPos + 1;
			rightStart = leftAtRightPos;
			rightEnd = rightStart + tmpDiv;
			if (leftEnd < leftStart + tmpDiv)
			{
				rightEnd += leftStart + tmpDiv - leftEnd;
				rightEnd += oddOpr1[oddIndex][1];
			}
			else
			{
				int tmpEnd = leftEnd;
				leftEnd = leftStart + tmpDiv;
				for (int i = 0; i < oddOpr1[oddIndex][1]; i++)
				{
					addRight(lrBorder);
				}
			}
		}
		else if (div1 > div2)
		{
			int div = div1 - div2;
			rightEnd = leftAtRightPos;
			leftEnd = leftMidPos + 1;
			if (odd)
			{
				div++;
			}

			int rDiv = div / 2;
			int lDiv = div - rDiv;

			if (leftEnd - lDiv < llBorder)
			{
				leftStart = llBorder;
				rightStart = rightEnd - rDiv + leftEnd - lDiv - llBorder; 
			}
			else
			{
				leftStart = leftEnd - lDiv;
				rightStart = rightEnd - rDiv;
			}
			
			//printf("leftStart:%d leftEnd:%d, rightStart:%d, rightEnd:%d\n", leftStart, leftEnd, rightStart, rightEnd);
			if (addLeft(llBorder,rlBorder))
				minusLeft(lrBorder, rrBorder);

			if ((leftEnd - leftStart) == 1)
			{
				if (div == 1)
				{
					return LEFT_D(leftStart);
				}
				else if (div == 2)
				{
					return (LEFT_D(leftStart) + RIGHT_D(rightStart)) / 2;
				}
				else
				{
					leftEnd--;
					rightStart++;
				}
			}
		}
		else
		{
			if (odd)
			{
				return LEFT_D(leftMidPos);
			}
			else
			{
				int min = MIN(LEFT(leftMidPos + 1), RIGHT(leftAtRightPos));
				return ((double)leftMid + (double)min) / 2;
			}
		}
		//printf("leftMidPos:%d, leftMid:%d, leftAtRightPos:%d, rightValue:%d\n", leftMidPos, leftMid, leftAtRightPos, RIGHT(leftAtRightPos));
		//printf("leftStart:%d leftEnd:%d, rightStart:%d, rightEnd:%d\n", leftStart, leftEnd, rightStart, rightEnd);
		//printVecWithNum(*nums1, leftStart, leftEnd);
		//printVecWithNum(*nums2, rightStart, rightEnd);
		return recFind();
	}
public:
	double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2){
		this->nums1 = &nums1;
		this->nums2 = &nums2;
		leftStart = 0;
		leftEnd = (int)(*this->nums1).size();
		rightStart = 0;
		rightEnd = (int)(*this->nums2).size();
		return recFind();
	}
};
void printVec(vector<int> &vec)
{
	int len = vec.size();
	for (int i = 0; i < len; i++)
	{
		printf("%d:%d ", i, vec[i]);
	}
	printf("\n");
}

#ifndef LEET_ORI
extern "C"
{
	__declspec(dllexport) double calc(int *left, int len1, int *right, int len2)
	{
		printf("\n\n\n***********************\nstart a new test!\n");
		Solution solut;
		vector<int> leftVec(left, left + len1);
		//printf("left size:%d\n", leftVec.size());

		vector<int> rightVec(right, right + len2);
		//printf("right size:%d\n", rightVec.size());
		//printVec(leftVec);
		//printVec(rightVec);
		double ret = solut.findMedianSortedArrays(leftVec, rightVec);
		printf("result is :%f\n", ret);
		return ret;
	}
};
#else
int main(){
	Solution solut;
	int len1 = sizeof(test1) / sizeof(int);
	int len2 = sizeof(test2) / sizeof(int);
	printf("\n\nlen1 is:%d\n", len1);
	printf("len2 is:%d\n", len2);

	vector<int> leftVec(test1, test1 + len1);
	printf("left size:%d\n", leftVec.size());

	vector<int> rightVec(test2, test2 + len2);
	printf("right size:%d\n", rightVec.size());
	printVec(leftVec);
	printVec(rightVec);
	printf("ret is:%lf\n", solut.findMedianSortedArrays(leftVec, rightVec));
	getchar();
	return 0;
}

#endif