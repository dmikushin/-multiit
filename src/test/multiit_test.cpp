#include "multiit.h"

#include <cstdio>

using namespace multiit::runtime;

// A group of indexes tailored to the choices matrix.
struct ChoicesIterator
{
	const int n_periods, n_experiences, n_lagged_experiences;

	MultiIterator period_lagged_experineces;
	LimitedMultiIterator experiences;

	GenericMultiIterator<MultiIterator*> impl;

	uint32_t getPeriod() const { return period_lagged_experineces.getCurrent()[0]; }

	const uint32_t* getExperiences() const
	{
		return reinterpret_cast<const uint32_t*>(&period_lagged_experineces.getCurrent()[1]);
	}

	const uint32_t* getLaggedExperiences() const
	{
		return reinterpret_cast<const uint32_t*>(experiences.getCurrent());
	}

	bool next()
	{
		return impl.next();
	}

	// TODO Avoid vectors, as we want it to be compatbile with GPUs.
	ChoicesIterator(int n_periods_, const std::vector<uint32_t>& experiences_,
		int n_lagged_experiences_, int n_choices) :
		n_periods(n_periods_), n_experiences(experiences_.size()),
		n_lagged_experiences(n_lagged_experiences_),
		period_lagged_experineces(n_lagged_experiences_ + 1),
		experiences(experiences_, period_lagged_experineces.getCurrent()[0]),
		impl({ &period_lagged_experineces, &experiences })
	{
		auto dims = period_lagged_experineces.getDims();
		dims[0] = n_periods;
		for (int i = 1; i < n_lagged_experiences + 1; i++)
			dims[i] = n_choices;
	}

	// TODO Add a constructor with opaque ChoicesIteratorState, containing the current state.
};

int main(int argc, char* argv[])
{
	{
		multiit::runtime::MultiIterator mi_runtime({ 2, 3, 4 });
		multiit::compiletime::MultiIterator<2, 3, 4> mi_compiletime;

		if (mi_runtime.getSize() != mi_compiletime.getSize())
		{
			fprintf(stderr, "Runtime and compile-time multi iterators indexes sizes mismatch: %zu != %zu\n",
				mi_runtime.getSize(), mi_compiletime.getSize());
			exit(-1);
		}

		int niters = 0;
		while (1)
		{
			niters++;

			bool next = mi_runtime.next();
			if (next != mi_compiletime.next())
			{
				fprintf(stderr, "Runtime and compile-time multi iterators volumes mismatch\n");
				exit(-1);
			}

			if (!next) break;

			const auto& current_runtime = mi_runtime.getCurrent();
			const auto& current_compiletime = mi_compiletime.getCurrent();
			for (int i = 0, size = mi_runtime.getSize(); i < size; i++)
			{
				if (current_runtime[i] != current_compiletime[i])
				{
					fprintf(stderr, "Runtime and compile-time multi iterators current indexes mismatch @ i = %d: %u != %u\n",
						i, current_runtime[i], current_compiletime[i]);
					exit(-1);
				}
			}
		}

		printf("multiit::runtime::MultiIterator<2, 3, 4> mi : %d iterations visited\n", niters);
		printf("multiit::compiletime::MultiIterator<2, 3, 4> mi : %d iterations visited\n", niters);
	}

	{
		uint32_t limit = 5;
		LimitedMultiIterator mi({ 4, 4, 4 }, limit);

		int niters = 0;
		do
		{
			niters++;
		}
		while(mi.next());

		printf("LimitedMultiIterator mi({ 4, 4, 4 }, 5) : %d iterations visited\n", niters);
	}

	ChoicesIterator choices(76, {{ 31, 35, 10 }}, 1, 20);

	int nchoices = 0;
	do
	{
		nchoices++;
	}
	while (choices.next());

	printf("ChoicesIterator choices(76, {{ 31, 35, 10 }}, 1, 20) : %d choices visited\n", nchoices);

	return 0;
}

