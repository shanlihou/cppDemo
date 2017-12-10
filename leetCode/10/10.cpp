#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;
class Solution{
private:
	string::const_iterator leftEnd;
	string::const_iterator rightEnd;
	static const int DOT_STAR = 256 + '.';
	int nextLeft(string::const_iterator iter){
		if ((iter + 1 != leftEnd) && (*(iter + 1) == '*'))
			return *iter + 256;
		else
			return *iter;
	}
	int nextRight(string::const_iterator iter){
		if ((iter + 1 != rightEnd) && (*(iter + 1) == '*'))
			return *iter + 256;
		else
			return *iter;
	}

	string::const_iterator leftGo(string::const_iterator iter){
		if ((iter + 1 != leftEnd) && (*(iter + 1) == '*'))
			return iter + 2;
		else
			return iter + 1;
	}

	string::const_iterator rightGo(string::const_iterator iter){
		if ((iter + 1 != rightEnd) && (*(iter + 1) == '*'))
			return iter + 2;
		else
			return iter + 1;
	}

public:
	bool recurMatch(string::const_iterator leftStart, string::const_iterator rightStart)
	{
		if (leftStart == leftEnd && rightStart == rightEnd)
			return true;

		if (leftStart == leftEnd)
		{
			if (nextRight(rightStart) > 255)
				return true;
			return false;
		}

		if (rightStart == rightEnd)
		{
			if (nextLeft(leftStart) > 255)
				return true;
			return false;
		}
		int nextLeftAlpha = nextLeft(leftStart);
		int nextRightAlpha = nextRight(rightStart);

		if (nextLeftAlpha == DOT_STAR || nextRightAlpha == DOT_STAR)
		{
			return recurMatch(leftStart, rightGo(rightStart)) || recurMatch(leftGo(leftStart), rightStart);
		}
		else if(nextLeftAlpha == '.')
		{
			if (nextRightAlpha < 256)
				return recurMatch(leftGo(leftStart), rightGo(rightStart));
			else
				return recurMatch(leftStart, rightGo(rightStart)) || recurMatch(leftGo(leftStart), rightStart);
		}
		else if(nextLeftAlpha < 256)
		{
			if (nextRightAlpha < 256)
			{
				if (nextRightAlpha == '.' || nextRightAlpha == nextLeftAlpha)
					return recurMatch(leftGo(leftStart), rightGo(rightStart));
				else
					return false;
			}
			else if (nextRightAlpha != nextLeftAlpha + 256)
			{
				return recurMatch(leftStart, rightGo(rightStart));
			}
			else
			{
				return recurMatch(leftStart, rightGo(rightStart)) || recurMatch(leftGo(leftStart), rightStart);
			}
		}
		else
		{
			if (nextRightAlpha > 255)
			{
				return recurMatch(leftGo(leftStart), rightGo(rightStart));
			}
			else if(nextRightAlpha == '.' || nextRightAlpha + 256 == nextLeftAlpha)
			{
				return recurMatch(leftStart, rightGo(rightStart)) || recurMatch(leftGo(leftStart), rightStart);
			}
			else
			{
				return recurMatch(leftGo(leftStart), rightStart);
			}
		}
	}
	bool isMatch(string s, string p){
		leftEnd = s.end();
		rightEnd = p.end();

		return recurMatch(s.begin(), p.begin());
	}
};
void test1()
{
	Solution test;
	string array[] = {"aa", "a",
			"aa", "aa",
			"aa", "a*",
			"aa", ".*",
			"ab", ".*",
			"aab", "c*a*b",
			"c*a*b", "b*a*c",
			"*c*a*b*", "*b*a*c*",
			"aaa", "aa"};
	int arrLen = sizeof(array) / sizeof(string);
	for (int i = 0; i < arrLen / 2; i++)
	{
		printf("str1:%s, str2:%s:\n", array[i * 2].c_str(), array[i * 2 + 1].c_str());
		printf("result:%s\n", test.isMatch(array[i * 2].c_str(), array[i * 2 + 1].c_str()) ? "true" : "false");
	}
}
void test2()
{
	Solution test;
	string array[] = {
			"aa", "a*"};
	int arrLen = sizeof(array) / sizeof(string);
	for (int i = 0; i < arrLen / 2; i++)
	{
		printf("str1:%s, str2:%s:\n", array[i * 2].c_str(), array[i * 2 + 1].c_str());
		printf("result:%s\n", test.isMatch(array[i * 2].c_str(), array[i * 2 + 1].c_str()) ? "true" : "false");
	}
}
int main(){
	test1();
	return 0;
}
