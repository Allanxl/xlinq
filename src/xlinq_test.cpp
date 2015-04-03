#include "xlinq.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <list>
using namespace std;
using namespace xlinq;

vector<int> getIntVector(){
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

vector<int> getEmptyIntVector(){
	vector<int> v;
	return v;
}

vector<float> getFloatVector(){
	vector<float> v;
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

vector<float> getEmptyFloatVector(){
	vector<float> v;
	return v;
}

vector<string> getStringVector(){
	vector<string> v;
	v.push_back("1");
	v.push_back("2");
	v.push_back("3");
	v.push_back("4");
	v.push_back("5");
	v.push_back("6");
	v.push_back("7");
	v.push_back("8");
	v.push_back("9");
	v.push_back("10");
	return v;
}

vector<string> getEmptyStringVector(){
	vector<string> v;
	return v;
}

template<typename T>
void equal(vector<T> a, vector<T> b){
	EXPECT_EQ(a.size(), b.size());
	for(size_t i = 0; i < a.size(); i++){
		EXPECT_EQ(a[i], b[i]);
	}
}

template<typename T>
void test_vector2vector(vector<T> expected){
	vector<T> actual = EnumerableFactory::From<T>(expected).ToVector();
	equal(expected, actual);
}

TEST(Instantiation, ToVectorOrList){
	test_vector2vector<int>(getIntVector());
	test_vector2vector<float>(getFloatVector());
}

template<typename T>
void test_any(vector<T> expected, bool hasValue){
	EXPECT_EQ(hasValue, EnumerableFactory::From<T>(expected).Any());
}

TEST(BoolCalculation, Any){
	test_any<int>(getIntVector(), true);
	test_any<float>(getFloatVector(), true);
	test_any<string>(getStringVector(), true);
	
	test_any<int>(getEmptyIntVector(), false);
	test_any<float>(getEmptyFloatVector(), false);
	test_any<string>(getEmptyStringVector(), false);
}

template<typename T, int threshold>
bool GT(T val){
	return val > threshold;
}

template<typename T>
void test_conditional_any(vector<T> expected, bool condition(T), bool hasValue){
	EXPECT_EQ(hasValue, EnumerableFactory::From<T>(expected).Any(condition));
}

TEST(BoolCalculation, ConditionalAny){
	test_conditional_any<int>(getIntVector(), GT<int, 5>, true);
	test_conditional_any<float>(getFloatVector(), GT<float, 5>, true);
	
	test_conditional_any<int>(getIntVector(), GT<int, 10>, false);
	test_conditional_any<float>(getFloatVector(), GT<float, 10>, false);
	
	test_conditional_any<int>(getEmptyIntVector(), GT<int, 5>, false);
	test_conditional_any<float>(getEmptyFloatVector(), GT<float, 5>, false);
}

template<typename T>
void test_all(vector<T> expected, bool condition(T), bool hasValue){
	EXPECT_EQ(hasValue, EnumerableFactory::From<T>(expected).All(condition));
}

TEST(BoolCalculation, ConditionalAll){
	test_all<int>(getIntVector(), GT<int, 0>, true);
	test_all<float>(getFloatVector(), GT<float, 0>, true);
	
	test_all<int>(getIntVector(), GT<int, 5>, false);
	test_all<float>(getFloatVector(), GT<float, 5>, false);
	
	test_all<int>(getEmptyIntVector(), GT<int, 5>, true);
	test_all<float>(getEmptyFloatVector(), GT<float, 5>, true);
}

template<typename T>
void test_contains(vector<T> expected, T val, bool hasValue){
	EXPECT_EQ(hasValue, EnumerableFactory::From<T>(expected).Contains(val));
}

TEST(BoolCalculation, Contains){
	test_contains<int>(getIntVector(), 5, true);
	test_contains<float>(getFloatVector(), 5, true);
	
	test_contains<int>(getIntVector(), 11, false);
	test_contains<float>(getFloatVector(), 11, false);
	
	test_contains<int>(getEmptyIntVector(), 5, false);
	test_contains<float>(getEmptyFloatVector(), 5, false);
}

template<typename T>
void test_count(vector<T> expected, int count){
	EXPECT_EQ(count, EnumerableFactory::From<T>(expected).Count());
}

TEST(Aggregation, Count){
	test_count<int>(getIntVector(), 10);
	test_count<float>(getFloatVector(), 10);
	
	test_count<int>(getEmptyIntVector(), 0);
	test_count<float>(getEmptyFloatVector(), 0);
}

template<typename T>
void test_sum(vector<T> expected){
	T sum = 0;
	for(size_t i = 0; i < expected.size(); i++){
		sum += expected[i];
	}
	EXPECT_EQ(sum, EnumerableFactory::From<T>(expected).Sum());
}

TEST(Aggregation, Sum){
	test_sum<int>(getIntVector());
	test_sum<float>(getFloatVector());
	
	test_sum<int>(getEmptyIntVector());
	test_sum<float>(getEmptyFloatVector());
}

template<typename T>
void test_additional_sum(vector<T> expected, T additional){
	T sum = additional;
	for(size_t i = 0; i < expected.size(); i++){
		sum += expected[i];
	}
	EXPECT_EQ(sum, EnumerableFactory::From<T>(expected).Sum(additional));
}

TEST(Aggregation, AdditionalSum){
	test_additional_sum<int>(getIntVector(), 10);
	test_additional_sum<float>(getFloatVector(), 10);
	
	test_additional_sum<int>(getEmptyIntVector(), 10);
	test_additional_sum<float>(getEmptyFloatVector(), 10);
}

template<typename T>
void test_max(vector<T> expected, T lowLimit, bool expectException){
	try{
		T max = lowLimit;
		for(size_t i = 0; i < expected.size(); i++){
			max = max > expected[i] ? max : expected[i];
		}
		EXPECT_EQ(max, EnumerableFactory::From<T>(expected).Max());
		EXPECT_EQ(false, expectException);
	}
	catch(item_not_found ex){
		EXPECT_EQ(true, expectException);
	}
}

TEST(Aggregation, Max){
	test_max<int>(getIntVector(), 0, false);
	test_max<float>(getFloatVector(), 0, false);
	
	test_max<int>(getEmptyIntVector(), 0, true);
	test_max<float>(getEmptyFloatVector(), 0, true);
}

template<typename T>
void test_conditional_max(vector<T> expected, T lowLimit){
	T max = lowLimit;
	for(size_t i = 0; i < expected.size(); i++){
		max = max > expected[i] ? max : expected[i];
	}
	EXPECT_EQ(max, EnumerableFactory::From<T>(expected).Max(lowLimit));
}

TEST(Aggregation, ConditionalMax){
	test_conditional_max<int>(getIntVector(), 0);
	test_conditional_max<float>(getFloatVector(), 0);
	
	test_conditional_max<int>(getIntVector(), 10);
	test_conditional_max<float>(getFloatVector(), 10);
	
	test_conditional_max<int>(getEmptyIntVector(), 0);
	test_conditional_max<float>(getEmptyFloatVector(), 0);
}

template<typename T>
void test_min(vector<T> expected, T highLimit, bool expectException){
	try{
		T min = highLimit;
		for(size_t i = 0; i < expected.size(); i++){
			min = min < expected[i] ? min : expected[i];
		}
		EXPECT_EQ(min, EnumerableFactory::From<T>(expected).Min());
		EXPECT_EQ(false, expectException);
	}
	catch(item_not_found ex){
		EXPECT_EQ(true, expectException);
	}
}

TEST(Aggregation, Min){
	test_min<int>(getIntVector(), 11, false);
	test_min<float>(getFloatVector(), 11, false);
	
	test_min<int>(getEmptyIntVector(), 11, true);
	test_min<float>(getEmptyFloatVector(), 11, true);
}

template<typename T>
void test_conditional_min(vector<T> expected, T highLimit){
	T min = highLimit;
	for(size_t i = 0; i < expected.size(); i++){
		min = min < expected[i] ? min : expected[i];
	}
	EXPECT_EQ(min, EnumerableFactory::From<T>(expected).Min(highLimit));
}

TEST(Aggregation, ConditionalMin){
	test_conditional_min<int>(getIntVector(), 0);
	test_conditional_min<float>(getFloatVector(), 0);
	
	test_conditional_min<int>(getIntVector(), 11);
	test_conditional_min<float>(getFloatVector(), 11);
	
	test_conditional_min<int>(getEmptyIntVector(), 0);
	test_conditional_min<float>(getEmptyFloatVector(), 0);
}

template<typename T>
T aggregate(T a, T b){
	return a * b;
}

template<typename T>
void test_aggregate(vector<T> expected, bool expectException){
	try
	{
		T result = 1;
		for(size_t i = 0; i < expected.size(); i++){
			result *= expected[i];
		}
		EXPECT_EQ(result, EnumerableFactory::From<T>(expected).Aggregate(aggregate<T>));
		EXPECT_EQ(false, expectException);
	}
	catch(item_not_found ex){
		EXPECT_EQ(true, expectException);
	}
}

TEST(Aggregation, Aggregate){
	test_aggregate<int>(getIntVector(), false);
	test_aggregate<float>(getFloatVector(), false);
	
	test_aggregate<int>(getEmptyIntVector(), true);
	test_aggregate<float>(getEmptyFloatVector(), true);
}

template<typename T>
void test_conditional_aggregate(vector<T> expected, T base){
	T result = base;
	for(size_t i = 0; i < expected.size(); i++){
		result *= expected[i];
	}
	EXPECT_EQ(result, EnumerableFactory::From<T>(expected).Aggregate(base, aggregate<T>));
}

TEST(Aggregation, ConditionalAggregate){
	test_conditional_aggregate<int>(getIntVector(), 2);
	test_conditional_aggregate<float>(getFloatVector(), 2);
	
	test_conditional_aggregate<int>(getIntVector(), 2);
	test_conditional_aggregate<float>(getFloatVector(), 2);
	
	test_conditional_aggregate<int>(getEmptyIntVector(), 2);
	test_conditional_aggregate<float>(getEmptyFloatVector(), 2);
}

template<typename T>
void test_elementat(vector<T> expected, size_t index, bool expectException){
	try
	{
		T result = 0;
		if(expected.size() > index - 1){
			result = expected[index - 1];
		}
		EXPECT_EQ(result, EnumerableFactory::From<T>(expected).ElementAt(index));
		EXPECT_EQ(false, expectException);
	}
	catch(item_not_found ex){
		EXPECT_EQ(true, expectException);
	}
}

TEST(Iteration, ElementAt){
	test_elementat<int>(getIntVector(), 5, false);
	test_elementat<float>(getFloatVector(), 5, false);
	
	test_elementat<int>(getIntVector(), 11, true);
	test_elementat<float>(getFloatVector(), 11, true);
	
	test_elementat<int>(getEmptyIntVector(), 2, true);
	test_elementat<float>(getEmptyFloatVector(), 2, true);
}

template<typename T>
void test_first(vector<T> expected, bool expectException){
	try
	{
		T result = 0;
		if(expected.size() > 0){
			result = expected[0];
		}
		EXPECT_EQ(result, EnumerableFactory::From<T>(expected).First());
		EXPECT_EQ(false, expectException);
	}
	catch(item_not_found ex){
		EXPECT_EQ(true, expectException);
	}
}

TEST(Iteration, First){
	test_first<int>(getIntVector(), false);
	test_first<float>(getFloatVector(), false);
	
	test_first<int>(getEmptyIntVector(), true);
	test_first<float>(getEmptyFloatVector(), true);
}

template<typename T>
void test_firstordefault(vector<T> expected){
	T result = 0;
	if(expected.size() > 0){
		result = expected[0];
	}
	EXPECT_EQ(result, EnumerableFactory::From<T>(expected).FirstOrDefault());
}

TEST(Iteration, FirstOrDefault){
	test_firstordefault<int>(getIntVector());
	test_firstordefault<float>(getFloatVector());
	
	test_firstordefault<int>(getEmptyIntVector());
	test_firstordefault<float>(getEmptyFloatVector());
}

template<typename T>
void test_where(vector<T> expected, bool condition(T val)){
	int count = 0;
	for(size_t i = 0; i < expected.size(); i++){
		if(condition(expected[i])){
			count++;
		}
	}
	EXPECT_EQ(count, EnumerableFactory::From<T>(expected).Where(condition).Count());
}

TEST(Filter, Where){
	test_where<int>(getIntVector(), GT<int, 5>);
	test_where<float>(getFloatVector(), GT<float, 5>);
	
	test_where<int>(getIntVector(), GT<int, 10>);
	test_where<float>(getFloatVector(), GT<float, 10>);
	
	test_where<int>(getEmptyIntVector(), GT<int, 5>);
	test_where<float>(getEmptyFloatVector(), GT<float, 5>);
}

void equal(vector<int> a, vector<float> b){
	EXPECT_EQ(a.size(), b.size());
	for(size_t i = 0; i < a.size(); i++){
		EXPECT_EQ((float)a[i], b[i]);
	}
}

template<typename TFrom, typename TTo>
TTo convert(TFrom val){
	return (TTo)val;
}

void test_select(vector<int> expected){
	equal(expected, EnumerableFactory::From<int>(expected).Select(convert<int, float>).ToVector());
}

TEST(Filter, Select){
	test_select(getIntVector());
}

template<typename T>
void test_rerun(vector<T> expected, T maxBase, T minBase, T key, bool condition(T val)){
	T sum = 0;
	int count = 0;
	T max = maxBase;
	T min = minBase;
	bool hasItem = expected.size() > 0;
	bool containsKey = false;
	bool all = true;
	bool any = false;
	for(size_t i = 0; i < expected.size(); i++){
		sum += expected[i];
		count++;
		max = max > expected[i] ? max : expected[i];
		min = min < expected[i] ? min : expected[i];
		if(key == expected[i]){
			containsKey = true;
		}
		if(condition(expected[i])){
			any = true;
		}
		else{
			all =false;
		}
	}
	
	Enumerable<vector<T>, T> enumerable = EnumerableFactory::From<T>(expected);
	T actual_sum = enumerable.Sum();
	int actual_count = enumerable.Count();
	T actual_max = enumerable.Max();
	T actual_min = enumerable.Min();
	bool actual_hasItem = enumerable.Any();
	bool actual_containsKey = enumerable.Contains(key);
	bool actual_all = enumerable.All(condition);
	bool actual_any = enumerable.Any(condition);
	vector<T> actual_v = enumerable.ToVector();
	EXPECT_EQ(sum, actual_sum);
	EXPECT_EQ(count, actual_count);
	EXPECT_EQ(max, actual_max);
	EXPECT_EQ(min, actual_min);
	EXPECT_EQ(hasItem, actual_hasItem);
	EXPECT_EQ(containsKey, actual_containsKey);
	EXPECT_EQ(all, actual_all);
	EXPECT_EQ(any, actual_any);
	equal(expected, actual_v);
}

TEST(Robust, ReRun){
	test_rerun<int>(getIntVector(), 0, 11, 5, GT<int, 5>);
}


