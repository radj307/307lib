#include <iostream>
#include <vector>
#include <string>

using namespace std;

bool isAscOrder(vector<int> arr)
{
    int size_array = arr.size()-1;
    int min = 0;
    int max = 0;
    for (int j = 0; j < size_array; j++)
    {
        if (min == 0)
        {
            min = arr[j];
            max = arr[j];
        }

        if (arr[j] > min)
        {
            return true;
        }
        if(arr[j]<min)
        {
            return false;
        }
    }
    return false;
}

int main()
{
	vector<int> inAscOrder_1 = { 1, 2, 4, 7, 19 }; // returns true
	vector<int> inAscOrder_2 = { 1, 2, 3, 4, 5 }; // returns true
	vector<int> inAscOrder_3 = { 1, 6, 10, 18, 2, 4, 20 }; // returns false
	vector<int> inAscOrder_4 = { 9, 8, 7, 6, 5, 4, 3, 2, 1 }; // returns false because the numbers are in DESCENDING order


	//test 1

	if (isAscOrder(inAscOrder_1) == true) {
		cout << "test #1 passed! " << endl;;
	}
	if (isAscOrder(inAscOrder_2) == true) {
		cout << "test #2 passed! " << endl;
	}
	if (isAscOrder(inAscOrder_3) == false) {
		cout << "test #3 passed! " << endl;
	}
	if (isAscOrder(inAscOrder_4) == false) {
		cout << "test #4 passed! ";
	}


	return 0;
}
