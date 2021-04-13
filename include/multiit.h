#ifndef MULTIIT_H
#define MULTIIT_H

#include <stdint.h>
#include <vector>

// A group of indexes that iterate from 0 to the given upper value.
struct MultiIterator : public std::vector<uint32_t>
{
	int size;

	inline uint32_t* getCurrent() { return std::vector<uint32_t>::data(); }
	inline const uint32_t* getCurrent() const { return std::vector<uint32_t>::data(); }

        inline uint32_t* getDims() { return std::vector<uint32_t>::data() + size; }
	inline const uint32_t* getDims() const { return std::vector<uint32_t>::data() + size; }

	MultiIterator(int size_) : std::vector<uint32_t>(2 * size_), size(size_) { }

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
		for (int i = size - 1; i >= 0; i--)
		{
			current[i]++;
			if (current[i] >= dims[i])
				current[i] = 0;
			else
				return true;
		}

		return false;
	}

	virtual void zero()
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

	// Note limit is a reference, as we may want the limit to change dynamically.
        LimitedMultiIterator(const std::vector<uint32_t>& dims, uint32_t& limit_) :
		limit(limit_), MultiIterator(dims) { }

	// Note limit is a reference, as we may want the limit to change dynamically.
	LimitedMultiIterator(const std::vector<uint32_t>& dims, const std::vector<uint32_t>& current, uint32_t& limit_) :
		limit(limit_), MultiIterator(dims, current) { }

        virtual bool next()
        {
		auto current = getCurrent();
                const auto dims = getDims();
		uint32_t sum = 0;
                for (int i = size - 1; i >= 0; i--)
                {
                        current[i]++;
			sum += current[i];

			// Also check the sum is within the limit.
                        if ((current[i] >= dims[i]) || (sum > limit))
			{
				sum -= current[i];
                                current[i] = 0;
			}
                        else
                                return true;
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
                for (int i = size - 1; i >= 0; i--)
                {
                        if (!current[i]->next())
				current[i]->zero();
                        else
                                return true;
                }

                return false;
        }
};

#endif // MULTIIT_H

