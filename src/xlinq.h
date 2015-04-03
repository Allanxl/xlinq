#ifndef SRC_TLINQ_H_
#define SRC_TLINQ_H_

#include <vector>
#include <list>
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

namespace xlinq
{
	class item_not_found : public logic_error  {
	public:
	  explicit item_not_found (const string& what_arg) : logic_error (what_arg){
	  }
	};

	class EnumerableFactory;

	template<typename TContainer,  typename TRes, typename TIter = typename TContainer::iterator, typename TItem = typename TContainer::value_type >
	class Enumerable
	{
	public:
		Enumerable<TContainer, TRes> Where(bool filter(TItem)){
			vector<bool (*)(TItem)> filters = _filters;
			filters.push_back(filter);
			return Enumerable(_begin, _end, filters, _transforms);
		}

		template<typename TValue>
		Enumerable<TContainer, TValue> Select(TValue transform(TItem)){
			vector<TValue (*)(TItem)> transforms;
			transforms.push_back(transform);
			return Enumerable<TContainer, TValue>(_begin, _end, _filters, transforms);
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
			_filters(vector<bool (*)(TItem)>()), _transforms(vector<TRes (*)(TItem)>()){
		}

		Enumerable(TIter begin, TIter end, vector<bool (*)(TItem)> filters, vector<TRes (*)(TItem)> transforms): _begin(begin), _end(end), _cur(begin),
			_filters(filters), _transforms(transforms){
		}

		TIter _begin;
		TIter _end;
		TIter _cur;
		vector<bool (*)(TItem)> _filters;
		vector<TRes (*)(TItem)>  _transforms;

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
				if(_transforms.empty()){
					result = *_cur;
				}
				else{
					result = _transforms[0](*_cur);
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
	};
}

#endif /* SRC_TLINQ_H_ */
