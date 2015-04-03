#include "xlinq.h"
#include <iostream>
#include <string>
using namespace xlinq;

vector<int> getSampleVector(){
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
	v.push_back(6);
	v.push_back(7);
	v.push_back(8);
	v.push_back(9);
	v.push_back(10);
	return v;
}

template<typename T, T threshold>
bool greaterThan(T value){
	return value > threshold;
}

template<typename T, T threshold>
bool lessThan(T value){
	return value < threshold;
}

template<typename T>
T product(T a, T b){
	return a* b;
}

double convertToPresentage(int value){
	return value / 100.0;
}

int main(int argc, char** argv){
	vector<int> sample = getSampleVector();

	//Example 1: Sum
	//Returns 0 if there is no item in the vecotr
	cout << "Sum of the vecotr is: " << EnumerableFactory::From<int>(sample).Sum();
	cout << endl;

	//Example 2: Count
	//Returns 0 if there is no item in the vecotr
	cout << "Number of the items in the vecotr is: " << EnumerableFactory::From<int>(sample).Count();
	cout << endl;

	//Example 3: Max
	//Throws item_not_found exception if there is no item in the vecotr
	try{
		cout << "The max item in the vecotr is: " << EnumerableFactory::From<int>(sample).Max();
	}
	catch(item_not_found ex){
	}
	cout << endl;

	//Example 3: Min
	//Throws item_not_found exception if there is no item in the vecotr
	try{
		cout << "The max item in the vecotr is: " << EnumerableFactory::From<int>(sample).Min();
	}
	catch(item_not_found ex){
	}
	cout << endl;

	//Example 4: First
	//Throws item_not_found exception if there is no item in the vecotr
	try{
		cout << "The first item in the vecotr is: " << EnumerableFactory::From<int>(sample).First();
	}
	catch(item_not_found ex){
	}
	cout << endl;

	//Example 5: FirstOrDefault
	//Returns 0 if there is no item in the vecotr
	cout << "The first item in the vecotr is: " << EnumerableFactory::From<int>(sample).First();
	cout << endl;

	//Example 6: ElementAt
	//Throws item_not_found exception if there is no item in the vecotr or index exceeds the vector length
	cout << "The third item in the vecotr is: " << EnumerableFactory::From<int>(sample).ElementAt(3);
	cout << endl;

	//Example 7: Contains
	cout << "Does the vector contain 3? " << EnumerableFactory::From<int>(sample).Contains(3);
	cout << endl;

	//Example 8: Any
	//Returns false if there is no item in the vecotr
	cout << "Is there any item in the list? " << EnumerableFactory::From<int>(sample).Any();
	cout << endl;

	//Example 9: Conditional Any
	cout << "Is there any item in the list which is greater than 5? " << EnumerableFactory::From<int>(sample).Any(greaterThan<int, 5>);
	cout << endl;

	//Example 10: Conditional Any
	cout << "Are all items in the list greater than 5? " << EnumerableFactory::From<int>(sample).All(greaterThan<int, 5>);
	cout << endl;

	//Example 11: Aggregate
	cout << "The product of the vector is: " << EnumerableFactory::From<int>(sample).Aggregate(product<int>);
	cout << endl;

	//Example 12: Select
	cout << "Convert the vector from int to presentage in double" << endl;
	vector<double> presentages = EnumerableFactory::From<int>(sample).Select<double>(convertToPresentage).ToVector();
	for(size_t i = 0; i < presentages.size(); i++){
		cout << presentages[i] << "\t";
	}
	cout << endl;

	//Example 13: Where
	cout << "Get all items which is greater than 5" << endl;
	vector<int> result = EnumerableFactory::From<int>(sample).Where(greaterThan<int, 5>).ToVector();
	for(size_t i = 0; i < result.size(); i++){
		cout << result[i] << "\t";
	}
	cout << endl;

	//Example 14: ToVector
	cout << "Convert to a vector and display it: " << endl;
	vector<int> v = EnumerableFactory::From<int>(sample).ToVector();
	for(size_t i = 0; i < v.size(); i++){
		cout << v[i] << "\t";
	}
	cout << endl;

	//Example 15: ToList
	cout << "Convert to a list and display it: " << endl;
	list<int> l = EnumerableFactory::From<int>(sample).ToList();
	for(list<int>::iterator itr = l.begin(); itr != l.end(); itr++){
		cout << *itr << "\t";
	}
	cout << endl;

	//Example 16: Conbime them together
	cout << "Get the number of items which is larger than 5 and less then 7" << endl;
	cout << EnumerableFactory::From<int>(sample).Where(greaterThan<int, 5>).Where(lessThan<int, 7>).Count();
	cout << endl;
}






