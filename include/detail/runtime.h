#ifndef MULTIIT_RUNTIME_H
#define MULTIIT_RUNTIME_H

#include <cassert>
#include <stdint.h>
#include <vector>

namespace multiit {

namespace runtime {

// A group of indexes that iterate from 0 to the given upper value.
struct MultiIterator : public std::vector<uint32_t>
{
	size_t size;

	inline size_t getSize() const { return size; }

	inline uint32_t* getCurrent() { return std::vector<uint32_t>::data(); }
	inline const uint32_t* getCurrent() const { return std::vector<uint32_t>::data(); }

        inline uint32_t* getDims() { return std::vector<uint32_t>::data() + size; }
	inline const uint32_t* getDims() const { return std::vector<uint32_t>::data() + size; }

	MultiIterator(size_t size_) : std::vector<uint32_t>(2 * size_), size(size_) { }

	MultiIterator(const std::vector<uint32_t>& dims_) : std::vector<uint32_t>(2 * dims_.size()), size(dims_.size())
	{
		auto dims = getDims();
		for (int i = 0; i < size; i++)
			dims[i] = dims_[i];
	}

	MultiIterator(const std::vector<uint32_t>& dims_, const std::vector<uint32_t>& current_) : std::vector<uint32_t>(2 * dims_.size()), size(dims_.size())
	{
		auto current = getCurrent();
                auto dims = getDims();
		for (int i = 0; i < size; i++)
		{
			current[i] = current_[i];
			dims[i] = dims_[i];
		}
	}

	virtual bool next()
	{
		auto current = getCurrent();
		const auto dims = getDims();
		for (int i = 0; i < size; i++)
		{
			current[i]++;
			if (current[i] >= dims[i])
				current[i] = 0;
			else
				return true;
		}

		return false;
	}

	virtual void reset()
	{
		auto current = getCurrent();
		for (int i = 0; i < size; i++)
			current[i] = 0;
	}

	virtual ~MultiIterator() { }
};

// A group of indexes that iterates only through indexes with total sum no greater than limit.
struct LimitedMultiIterator : public MultiIterator
{
	const uint32_t& limit;

	uint32_t sum;

	// Note limit is a reference, as we may want the limit to change dynamically.
        LimitedMultiIterator(const std::vector<uint32_t>& dims, uint32_t& limit_) :
		limit(limit_), MultiIterator(dims), sum(0) { }

	// Note limit is a reference, as we may want the limit to change dynamically.
	LimitedMultiIterator(const std::vector<uint32_t>& dims, const std::vector<uint32_t>& current_, uint32_t& limit_) :
		limit(limit_), MultiIterator(dims, current_)
	{
		auto current = getCurrent();
		for (int i = 0; i < size; i++)
			sum += current[i];
	}

        virtual bool next()
        {
		auto current = getCurrent();
                const auto dims = getDims();
                for (int i = 0; i < size; i++)
                {
                        current[i]++;
			sum++;

			// Also check the sum is within the limit.
                        if ((current[i] >= dims[i]) || (sum > limit))
			{
				sum -= current[i];
                                current[i] = 0;
			}
                        else
			{
#ifndef NDEBUG
				uint32_t sum_check = 0;
				for (int j = 0; j < size; j++)
					sum_check += current[j];
				assert(sum == sum_check);
#endif
                                return true;
			}
                }

                return false;
        }
};

// A group of indexes, whose indexes are themselves groups of indexes.
template<typename T>
struct GenericMultiIterator : private std::vector<T>
{
        int size;

        GenericMultiIterator(const std::vector<T>& current_) : std::vector<T>(current_), size(current_.size()) { }

        virtual bool next()
        {
                auto current = std::vector<T>::data();
                for (int i = 0; i < size; i++)
                {
                        if (!current[i]->next())
				current[i]->reset();
                        else
                                return true;
                }

                return false;
        }
};

} // namespace runtime

} // namespace multiit

#endif // MULTIIT_RUNTIME_H

