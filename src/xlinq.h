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
	class item_not_found : public logic_error {
	public:
		explicit item_not_found(const string& what_arg) : logic_error(what_arg) {
		}
	};

	class EnumerableFactory;

	template<typename TItem>
	class EnumeratorBase
	{
	public:
		virtual bool GetNext(TItem& value) = 0;
	};

	template<typename TContainer, typename TTo, typename TFrom = typename TContainer::value_type, typename TIter = typename TContainer::iterator>
	class Enumerable : public EnumeratorBase<TTo>
	{
	public:
		typedef TTo(*Transform)(TFrom);

		Enumerable<TContainer, TTo, TFrom>& Where(bool filter(TTo)) {
			_filters.push_back(filter);
			return *this;
		}

		template<typename TValue>
		Enumerable<TContainer, TValue, TTo> Select(TValue transform(TTo)) {
			return Enumerable<TContainer, TValue, TTo>(_begin, _end, transform, (EnumeratorBase<TTo>*)this);
		}

		TTo Aggregate(TTo base, TTo aggregator(TTo, TTo)) {
			TTo result = base;
			TTo current_value;
			while (GetNext(current_value)) {
				result = aggregator(result, current_value);
			};
			return result;
		}

		TTo Aggregate(TTo aggregator(TTo, TTo)) {
			TTo current_value;
			if (!GetNext(current_value)) {
				Reset();
				throw item_not_found("No item found");
			}
			return Aggregate(current_value, aggregator);
		}

		TTo Sum(TTo base) {
			TTo result = base;
			TTo current_value;
			while (GetNext(current_value)) {
				result = result + current_value;
			};
			return result;
		}

		TTo Sum() {
			TTo current_value;
			if (!GetNext(current_value)) {
				Reset();
				return 0;
			}
			return Sum(current_value);
		}

		TTo Max(TTo base) {
			TTo result = base;
			TTo current_value;
			while (GetNext(current_value)) {
				result = result > current_value ? result : current_value;
			};
			return result;
		}

		TTo Max() {
			TTo current_value;
			if (!GetNext(current_value)) {
				Reset();
				throw item_not_found("No item found");
			}
			return Max(current_value);
		}

		TTo Min(TTo base) {
			TTo result = base;
			TTo current_value;
			while (GetNext(current_value)) {
				result = result < current_value ? result : current_value;
			};
			return result;
		}

		TTo Min() {
			TTo current_value;
			if (!GetNext(current_value)) {
				Reset();
				throw item_not_found("No item found");
			}
			return Min(current_value);
		}

		TTo First() {
			TTo current_value;
			if (!GetNext(current_value)) {
				Reset();
				throw item_not_found("No item found");
			}
			else {
				Reset();
				return current_value;
			}
		}

		TTo FirstOrDefault() {
			TTo current_value;
			if (!GetNext(current_value)) {
				Reset();
				return 0;
			}
			else {
				Reset();
				return current_value;
			}
		}

		TTo ElementAt(size_t index) {
			size_t count = 1;
			TTo current_value;
			if (!GetNext(current_value)) {
				Reset();
				throw item_not_found("The expected item is found");
			}
			else {
				count++;
				while (GetNext(current_value)) {
					if (count == index) {
						Reset();
						return current_value;
					}
					count++;
				};
				throw item_not_found("The expected item is found");
			}
		}

		bool Any(bool condition(TFrom)) {
			TTo current_value;
			while (GetNext(current_value)) {
				if (condition(current_value))
				{
					Reset();
					return true;
				}
			};
			return false;
		}

		bool Any() {
			TTo current_value;
			while (GetNext(current_value)) {
				Reset();
				return true;
			};
			return false;
		}

		bool All(bool condition(TFrom)) {
			TTo current_value;
			while (GetNext(current_value)) {
				if (!condition(current_value))
				{
					Reset();
					return false;
				}
			};
			return true;
		}

		bool Contains(TFrom value) {
			TTo current_value;
			while (GetNext(current_value)) {
				if (value == current_value)
				{
					Reset();
					return true;
				}
			};
			return false;
		}

		int Count() {
			int count = 0;
			TTo current_value;
			while (GetNext(current_value)) {
				count++;
			};
			return count++;
		}

		vector<TTo> ToVector() {
			vector<TTo> v;
			TTo current_value;
			while (GetNext(current_value)) {
				v.push_back(current_value);
			};
			return v;
		}

		list<TTo> ToList() {
			list<TTo> l;
			TTo current_value;
			while (GetNext(current_value)) {
				l.push_back(current_value);
			};
			return l;
		}

	private:
		Enumerable(TIter begin, TIter end) : _begin(begin), _end(end), _cur(begin),
			_filters(vector<bool(*)(TTo)>()), _transform(NULL), _prevGetNext(NULL) {
		}

		Enumerable(TIter begin, TIter end, Transform transform, EnumeratorBase<TFrom>* prevGetNext) : _begin(begin), _end(end), _cur(begin),
			_filters(vector<bool(*)(TTo)>()), _transform(transform), _prevGetNext(prevGetNext) {
		}

		TIter _begin;
		TIter _end;
		TIter _cur;
		vector<bool(*)(TTo)> _filters;
		Transform _transform;
		EnumeratorBase<TFrom>* _prevGetNext;

		inline void Reset() {
			_cur = _begin;
		}

		bool IsSelected(TTo& value) {
			for (size_t i = 0; i < _filters.size(); i++) {
				if (!_filters[i](value)) {
					return false;
				}
			}
			return true;
		}

		bool GetNext(TTo& result) {
			if (_prevGetNext == NULL) {
				while (_cur != _end) {
					result = (TTo)*_cur;
					_cur++;

					if (IsSelected(result)) {
						
						return true;
					}
				};
				_cur = _begin;
				return false;
			}
			else {
				TFrom current;
				while (_prevGetNext->GetNext(current)) {
					bool selected = true;
					result = (*_transform)(current);

					if (IsSelected(result)) {
						return true;
					}
				};
				return false;
			}
		}

		friend class EnumerableFactory;

		template<typename T1, typename T2, typename T3, typename T4> friend class Enumerable;
	};

	class EnumerableFactory {
	public:
		template<typename TValue>
		static Enumerable< vector<TValue>, TValue > From(vector<TValue>& v) {
			return Enumerable< vector<TValue>, TValue >(v.begin(), v.end());
		}

		template<typename TValue>
		static Enumerable< list<TValue>, TValue > From(list<TValue>& l) {
			return Enumerable< list<TValue>, TValue >(l.begin(), l.end());
		}

		template<typename TValue>
		static Enumerable< deque<TValue>, TValue > From(deque<TValue>& d) {
			return Enumerable< deque<TValue>, TValue >(d.begin(), d.end());
		}

		template<typename TValue>
		static Enumerable< set<TValue>, TValue > From(set<TValue>& s) {
			return Enumerable< set<TValue>, TValue >(s.begin(), s.end());
		}

		template<typename TValue>
		static Enumerable< unordered_set<TValue>, TValue > From(unordered_set<TValue>& s) {
			return Enumerable< unordered_set<TValue>, TValue >(s.begin(), s.end());
		}

		template<typename TKey, typename TValue>
		static Enumerable< map<TKey, TValue>, pair<TKey, TValue> > From(map<TKey, TValue>& m) {
			return Enumerable< map<TKey, TValue>, pair<TKey, TValue> >(m.begin(), m.end());
		}

		template<typename TKey, typename TValue>
		static Enumerable< unordered_map<TKey, TValue>, pair<TKey, TValue> > From(unordered_map<TKey, TValue>& m) {
			return Enumerable< unordered_map<TKey, TValue>, pair<TKey, TValue> >(m.begin(), m.end());
		}
	};

}

#endif
