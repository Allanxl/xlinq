#ifndef SRC_XLINQ_H_
#define SRC_XLINQ_H_

#include <set>
#include <map>
#include <deque>
#include <vector>
#include <list>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;
using namespace std::tr1;

namespace xlinq
{
	class item_not_found : public logic_error  {
	public:
	  explicit item_not_found (const string& what_arg) : logic_error (what_arg){
	  }
	};

	class select_before_where : public logic_error  {
	public:
	  explicit select_before_where (const string& what_arg) : logic_error (what_arg){
	  }
	};

	class EnumerableFactory;

	template<typename TContainer,  typename TRes, typename TIter = typename TContainer::iterator, typename TItem = typename TContainer::value_type >
	class Enumerable
	{
	public:
		typedef TRes (*Transform)(TItem);

		Enumerable<TContainer, TRes> Where(bool filter(TItem)){
			if(_transform != NULL){
				throw select_before_where("Select should be placed after Where");
			}
			vector<bool (*)(TItem)> filters = _filters;
			filters.push_back(filter);
			return Enumerable(_begin, _end, filters, _transform);
		}

		template<typename TValue>
		Enumerable<TContainer, TValue> Select(TValue transform(TItem)){
			return Enumerable<TContainer, TValue>(_begin, _end, _filters, transform);
		}

		TRes Aggregate(TRes base, TRes aggregator(TRes, TRes)){
			TRes result = base;
			TRes current_value;
			while(GetNext(current_value)){
				result = aggregator(result, current_value);
			};
			return result;
		}

		TRes Aggregate(TRes aggregator(TRes, TRes)){
			TRes current_value;
			if(!GetNext(current_value)){
				Reset();
				throw item_not_found("No item found");
			}
			return Aggregate(current_value, aggregator);
		}

		TRes Sum(TRes base){
			TRes result = base;
			TRes current_value;
			while(GetNext(current_value)){
				result = result + current_value;
			};
			return result;
		}

		TRes Sum(){
			TRes current_value;
			if(!GetNext(current_value)){
				Reset();
				return 0;
			}
			return Sum(current_value);
		}

		TRes Max(TRes base){
			TRes result = base;
			TRes current_value;
			while(GetNext(current_value)){
				result = result > current_value ? result : current_value;
			};
			return result;
		}

		TRes Max(){
			TRes current_value;
			if(!GetNext(current_value)){
				Reset();
				throw item_not_found("No item found");
			}
			return Max(current_value);
		}

		TRes Min(TRes base){
			TRes result = base;
			TRes current_value;
			while(GetNext(current_value)){
				result = result < current_value ? result : current_value;
			};
			return result;
		}

		TRes Min(){
			TRes current_value;
			if(!GetNext(current_value)){
				Reset();
				throw item_not_found("No item found");
			}
			return Min(current_value);
		}

		TRes First(){
			TRes current_value;
			if(!GetNext(current_value)){
				Reset();
				throw item_not_found("No item found");
			}
			else{
				Reset();
				return current_value;
			}
		}

		TRes FirstOrDefault(){
			TRes current_value;
			if(!GetNext(current_value)){
				Reset();
				return 0;
			}
			else{
				Reset();
				return current_value;
			}
		}

		TRes ElementAt(size_t index){
			size_t count = 1;
			TRes current_value;
			if(!GetNext(current_value)){
				Reset();
				throw item_not_found("The expected item is found");
			}
			else{
				count++;
				while(GetNext(current_value)){
					if(count == index){
						Reset();
						return current_value;
					}
					count++;
				};
				throw item_not_found("The expected item is found");
			}
		}

		bool Any(bool condition(TItem)){
			TRes current_value;
			while(GetNext(current_value)){
				if(condition(current_value))
				{
					Reset();
					return true;
				}
			};
			return false;
		}

		bool Any(){
			TRes current_value;
			while(GetNext(current_value)){
				Reset();
				return true;
			};
			return false;
		}

		bool All(bool condition(TItem)){
			TRes current_value;
			while(GetNext(current_value)){
				if(!condition(current_value))
				{
					Reset();
					return false;
				}
			};
			return true;
		}

		bool Contains(TItem value){
			TRes current_value;
			while(GetNext(current_value)){
				if(value == current_value)
				{
					Reset();
					return true;
				}
			};
			return false;
		}

		int Count(){
			int count = 0;
			TRes current_value;
			while(GetNext(current_value)){
				count++;
			};
			return count++;
		}

		vector<TRes> ToVector(){
			vector<TRes> v;
			TRes current_value;
			while(GetNext(current_value)){
				v.push_back(current_value);
			};
			return v;
		}

		list<TRes> ToList(){
			list<TRes> l;
			TRes current_value;
			while(GetNext(current_value)){
				l.push_back(current_value);
			};
			return l;
		}

	private:
		Enumerable(TIter begin, TIter end): _begin(begin), _end(end), _cur(begin),
			_filters(vector<bool (*)(TItem)>()), _transform(NULL){
		}

		Enumerable(TIter begin, TIter end, vector<bool (*)(TItem)> filters, Transform transform): _begin(begin), _end(end), _cur(begin),
			_filters(filters), _transform(transform){
		}

		TIter _begin;
		TIter _end;
		TIter _cur;
		vector<bool (*)(TItem)> _filters;
		Transform _transform;

		inline void Reset(){
			_cur = _begin;
		}

		inline bool GetNext(TRes& result){
			while(_cur != _end){
				bool selected = true;
				for(size_t i = 0; i < _filters.size(); i++){
					if(!_filters[i](*_cur)){
						selected = false;
						break;
					}
				}

				if(selected){
					break;
				}
				_cur++;
			}

			if(_cur == _end){
				_cur = _begin;
				return false;
			}
			else{
				if(_transform == NULL){
					result = *_cur;
				}
				else{
					result = (*_transform)(*_cur);
				}
				_cur++;
				return true;
			}
		}

		friend class EnumerableFactory;

		template<typename T1,  typename T2, typename T3,  typename T4> friend class Enumerable;
	};

	class EnumerableFactory{
	public:
		template<typename TValue>
		static Enumerable< vector<TValue>, TValue > From(vector<TValue>& v){
			return Enumerable< vector<TValue>, TValue >(v.begin(), v.end());
		}

		template<typename TValue>
		static Enumerable< list<TValue>, TValue > From(list<TValue>& l){
			return Enumerable< list<TValue>, TValue >(l.begin(), l.end());
		}

		template<typename TValue>
		static Enumerable< deque<TValue>, TValue > From(deque<TValue>& d){
			return Enumerable< deque<TValue>, TValue >(d.begin(), d.end());
		}

		template<typename TValue>
		static Enumerable< set<TValue>, TValue > From(set<TValue>& s){
			return Enumerable< set<TValue>, TValue >(s.begin(), s.end());
		}

		template<typename TValue>
		static Enumerable< unordered_set<TValue>, TValue > From(unordered_set<TValue>& s){
			return Enumerable< unordered_set<TValue>, TValue >(s.begin(), s.end());
		}

		template<typename TKey, typename TValue>
		static Enumerable< map<TKey, TValue>, pair<TKey, TValue> > From(map<TKey, TValue>& m){
			return Enumerable< map<TKey, TValue>, pair<TKey, TValue> >(m.begin(), m.end());
		}

		template<typename TKey, typename TValue>
		static Enumerable< unordered_map<TKey, TValue>, pair<TKey, TValue> > From(unordered_map<TKey, TValue>& m){
			return Enumerable< unordered_map<TKey, TValue>, pair<TKey, TValue> >(m.begin(), m.end());
		}
	};

}

#endif
