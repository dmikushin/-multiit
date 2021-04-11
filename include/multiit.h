#ifndef MULTIIT_H
#define MULTIIT_H

#include <stdint.h>
#include <vector>

// A group of indexes that iterate from 0 to the given upper value.
struct MultiIterator
{
	const std::vector<uint32_t> vdims;
	std::vector<uint32_t> vcurrent;
	int size;
	
	MultiIterator(const std::vector<uint32_t>& dims_) : vdims(dims_), vcurrent(dims_.size()), size(dims_.size()) { }

	MultiIterator(const std::vector<uint32_t>& dims_, const std::vector<uint32_t>& current_) : vdims(dims_), vcurrent(current_), size(dims_.size()) { }

	virtual bool increment()
	{
		const auto dims = vdims.data();
		auto current = vcurrent.data();
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

        virtual bool increment()
        {
                const auto dims = vdims.data();
                auto current = vcurrent.data();
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
struct GenericMultiIterator
{
        const std::vector<T> vdims;
        std::vector<T> vcurrent;
        int size;

        GenericMultiIterator(const std::vector<T>& dims_) : vdims(dims_), vcurrent(dims_.size()), size(dims_.size()) { }

        GenericMultiIterator(const std::vector<T>& dims_, const std::vector<T>& current_) : vdims(dims_), vcurrent(current_), size(dims_.size()) { }

        virtual bool increment()
        {
                const auto dims = vdims.data();
                auto current = vcurrent.data();
                for (int i = size - 1; i >= 0; i--)
                {
                        if (!current[i].increment())
				current[i].zero();
                        else
                                return true;
                }

                return false;
        }
};

#endif // MULTIIT_H

