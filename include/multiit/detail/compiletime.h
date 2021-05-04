#ifndef MULTIIT_COMPILETIME_H
#define MULTIIT_COMPILETIME_H

#include <array>
#include <cassert>
#include <cstring>
#include <stdint.h>
#include <tuple>

namespace multiit {

namespace compiletime {

// A group of indexes that iterate from 0 to the given upper value.
template <uint32_t... Dims>
struct MultiIterator
{
	std::array<uint32_t, sizeof...(Dims)> current;

	inline size_t size() const { return sizeof...(Dims); }

	MultiIterator() { reset(); }

	MultiIterator(const std::array<uint32_t, sizeof...(Dims)>& current_) : current(current_) { }

	template <int i>
	constexpr bool next()
	{
		return false;
	}

	template <int i, uint32_t Dim, uint32_t... Rest>
	bool next()
	{
		current[i]++;
		if (current[i] >= Dim)
			current[i] = 0;
		else
			return true;

		return next<i + 1, Rest...>();
	}

	virtual bool next()
	{
		return next<0, Dims...>();
	}

	virtual void reset()
	{
		memset(&current[0], 0, sizeof(current));
	}

	virtual ~MultiIterator() { }
};

// A group of indexes that iterates only through indexes with total sum no greater than limit.
template <uint32_t... Dims>
struct LimitedMultiIterator : public MultiIterator<Dims...>
{
	const uint32_t& limit;

	uint32_t sum;

	// Note limit is a reference, as we may want the limit to change dynamically.
        LimitedMultiIterator(uint32_t& limit_) :
		limit(limit_), MultiIterator<Dims...>(), sum(0) { }

	// Note limit is a reference, as we may want the limit to change dynamically.
	LimitedMultiIterator(const std::array<uint32_t, sizeof...(Dims)>& current_, uint32_t& limit_) :
		limit(limit_), MultiIterator<Dims...>(current_)
	{
		for (int i = 0, e = MultiIterator<Dims...>::size(); i < e; i++)
			sum += MultiIterator<Dims...>::current[i];
	}

        template <int i>
        constexpr bool next()
        {
                return false;
        }

        template <int i, uint32_t Dim, uint32_t... Rest>
        bool next()
        {
                MultiIterator<Dims...>::current[i]++;
		sum++;

		// Also check the sum is within the limit.
                if ((MultiIterator<Dims...>::current[i] >= Dim) || (sum > limit))
		{
			sum -= MultiIterator<Dims...>::current[i];
                        MultiIterator<Dims...>::current[i] = 0;
		}
                else
		{
#ifndef NDEBUG
                        uint32_t sum_check = 0;
                        for (int j = 0, e = MultiIterator<Dims...>::size(); j < e; j++)
                                sum_check += MultiIterator<Dims...>::current[j];
                        assert(sum == sum_check);
#endif
                        return true;
		}

                return next<i + 1, Rest...>();
        }

        virtual bool next()
        {
                return next<0, Dims...>();
        }
};

// A group of indexes, whose indexes are themselves groups of indexes.
template <typename... Dims>
struct GenericMultiIterator
{
	std::tuple<Dims&...> current;

        inline size_t size() const { return sizeof...(Dims); }

	GenericMultiIterator() { reset(); }

        GenericMultiIterator(Dims&... current_) : current(current_ ...) { }

	template <int i>
        constexpr bool next()
        {
                return false;
        }

        template <int i, typename Dim, typename... Rest>
        bool next()
        {
		auto& current_i = std::get<i>(current);
		if (!current_i.next())
			current_i.reset();
		else
			return true;

		return next<i + 1, Rest...>();
        }

	virtual bool next()
        {
                return next<0, Dims...>();
        }

	virtual void reset()
	{
		std::apply([](auto&&... current_i)
                {
                        ((current_i.reset()), ...);
                },
                current);
	}
};

} // namespace compiletime

} // namespace multiit

#endif // MULTIIT_COMPILETIME_H

