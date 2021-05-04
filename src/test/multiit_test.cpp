#include "multiit/multiit.h"

#include <cstdio>

// A group of indexes tailored to the choices matrix.
struct ChoicesIterator
{
	const int n_periods, n_experiences, n_lagged_experiences;

	multiit::runtime::MultiIterator period_lagged_experineces;
	multiit::runtime::LimitedMultiIterator experiences;

	multiit::runtime::GenericMultiIterator<multiit::runtime::MultiIterator*> impl;

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
		// Construct a simple multiiterator and compare results.
		multiit::runtime::MultiIterator mi_runtime({ 2, 3, 4 });
		multiit::compiletime::MultiIterator<2, 3, 4> mi_compiletime;

		if (mi_runtime.getSize() != mi_compiletime.size())
		{
			fprintf(stderr, "Runtime and compile-time multi iterators indexes sizes mismatch: %zu != %zu\n",
				mi_runtime.getSize(), mi_compiletime.size());
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
			for (int i = 0, size = mi_runtime.getSize(); i < size; i++)
			{
				if (current_runtime[i] != mi_compiletime.current[i])
				{
					fprintf(stderr, "Runtime and compile-time multi iterators current indexes mismatch @ i = %d: %u != %u\n",
						i, current_runtime[i], mi_compiletime.current[i]);
					exit(-1);
				}
			}
		}

		printf("multiit::runtime::MultiIterator<2, 3, 4> mi : %d iterations visited\n", niters);
		printf("multiit::compiletime::MultiIterator<2, 3, 4> mi : %d iterations visited\n", niters);
	}

        {
		// Construct a limited multiiterator and compare results.
		uint32_t limit = 5;
                multiit::runtime::LimitedMultiIterator mi_runtime({ 3, 4, 5 }, limit);
                multiit::compiletime::LimitedMultiIterator<3, 4, 5> mi_compiletime(limit);

                if (mi_runtime.getSize() != mi_compiletime.size())
                {
                        fprintf(stderr, "Runtime and compile-time multi iterators indexes sizes mismatch: %zu != %zu\n",
                                mi_runtime.getSize(), mi_compiletime.size());
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
                        for (int i = 0, size = mi_runtime.getSize(); i < size; i++)
                        {
                                if (current_runtime[i] != mi_compiletime.current[i])
                                {
                                        fprintf(stderr, "Runtime and compile-time multi iterators current indexes mismatch @ i = %d: %u != %u\n",
                                                i, current_runtime[i], mi_compiletime.current[i]);
                                        exit(-1);
                                }
                        }
                }

                printf("multiit::runtime::LimitedMultiIterator<3, 4, 5(5)> mi : %d iterations visited\n", niters);
                printf("multiit::compiletime::LimitedMultiIterator<3, 4, 5>(5) mi : %d iterations visited\n", niters);
        }

	{
		// Construct a simple multiiterator.
		multiit::runtime::MultiIterator mi_runtime1({ 2, 3, 4 });
                multiit::compiletime::MultiIterator<2, 3, 4> mi_compiletime1;

		// Construct a limited multiiterator,
		multiit::runtime::LimitedMultiIterator mi_runtime2({ 3, 4, 5 }, mi_runtime1[0]);
                multiit::compiletime::LimitedMultiIterator<3, 4, 5> mi_compiletime2(mi_compiletime1.current[0]);

		// Construct an iterator combining the two iterators above and compare results.
		multiit::runtime::GenericMultiIterator<multiit::runtime::MultiIterator*> gmi_runtime({ &mi_runtime1, &mi_runtime2 });
		multiit::compiletime::GenericMultiIterator gmi_compiletime(mi_compiletime1, mi_compiletime2);

		if (gmi_runtime.getSize() != gmi_compiletime.size())
                {
                        fprintf(stderr, "Runtime and compile-time multi iterators indexes sizes mismatch: %zu != %zu\n",
                                gmi_runtime.getSize(), gmi_compiletime.size());
                        exit(-1);
                }

		int niters = 0;
                while (1)
                {
                        niters++;

                        bool next = gmi_runtime.next();
                        if (next != gmi_compiletime.next())
                        {
                                fprintf(stderr, "Runtime and compile-time multi iterators volumes mismatch\n");
                                exit(-1);
                        }

                        if (!next) break;
#if 0
                        const auto& current_runtime = gmi_runtime.getCurrent();
                        for (int i = 0, size = gmi_runtime.getSize(); i < size; i++)
                        {
                                if (current_runtime[i] != std::get<i>(gmi_compiletime.current).current)
                                {
                                        fprintf(stderr, "Runtime and compile-time multi iterators current indexes mismatch @ i = %d\n", i);
                                        exit(-1);
                                }
                        }
#endif
		}

                printf("multiit::runtime::GenericMultiIterator gmi : %d iterations visited\n", niters);
                printf("multiit::compiletime::GenericMultiIterator gmi : %d iterations visited\n", niters);
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

