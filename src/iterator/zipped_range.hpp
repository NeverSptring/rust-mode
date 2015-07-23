#ifndef RUST_ZIPPED_RANGE_HPP
#define RUST_ZIPPED_RANGE_HPP

#include "range_modifier.hpp"
#include "all_range_modifiers.hpp"

#include <utility>

namespace rust {

	template<class OriginRange, class OtherRange>
	class ZippedRange : public RangeModifier<OriginRange> {

		typedef ZippedRange<OriginRange, OtherRange> CurrentType;
		typedef RangeModifier<OriginRange>           ParentType;

	public:

		typedef std::pair<typename OriginRange::value_type,
		                  typename OtherRange ::value_type> value_type;

		ZippedRange(OriginRange range, OtherRange other)
			: ParentType(range), other(other) {}

		virtual typename CurrentType::difference_type size() {
			try {
				typename CurrentType::difference_type otherSize  = other.size();

				try {
					typename CurrentType::difference_type originSize = this->origin.size();
					return (otherSize < originSize) ? otherSize : originSize;
				} catch(InfiniteRangeException) {
					return otherSize;
				}

			} catch(InfiniteRangeException) {
				// Don't catch InfiniteRangeException here because we want it passed down
				return this->origin.size();
			}
		}

		virtual bool empty() {
			return other.empty() || this->origin.empty();
		}

		RANGE_MODIFIERS

		template<typename Container>
		Container collect() {
			try {
				size_t contSize = size();
				return collectSizeAware<Container>(contSize);
			} catch(UnknownValueException) {
				return collectSizeUnaware<Container>();
			}
		}

		typename ParentType::iterator& begin() {
			if(other.begin() != other.end()) {
				return this->origin.begin();
			} else {
				return this->origin.end();
			}
		}

		typename CurrentType::value_type beginValue() {
			return std::make_pair(this->origin.beginValue(), other.beginValue());
		}

		typename ParentType::iterator& end() {
			return this->origin.end();
		}

		CurrentType& operator++() {
			++this->origin;
			++other;
			return *this;
		}

		CurrentType operator++(int) {
			CurrentType thisBefore = *this;
			++this->origin;
			++other;
			return thisBefore;
		}

	private:
		OtherRange other;

		template<typename Container>
		Container collectSizeAware(size_t size) {
			Container cont(size);
			for(typename CurrentType::value_type& value: cont) {
				value = beginValue();
				++(*this);
			}
			return cont;
		}

		template<typename Container>
		Container collectSizeUnaware() {
			std::vector<typename CurrentType::value_type> temp;
			auto inserter = std::back_inserter(temp);

			while(begin() != end()) {
				*inserter++ = beginValue();
				++(*this);
			}

			// Can't use template specialization in this case but I can do
			// THIS:
			if(std::is_same<Container, std::vector<int> >::value) {
				return temp;
			} else {
				Container cont(temp.size());
				std::copy(temp.begin(), temp.end(), cont.begin());
				return cont;
			}
		}
	};

}

#endif