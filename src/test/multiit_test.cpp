#include "multiit.h"

// A group of indexes tailored to the choices matrix.
struct ChoicesIterator
{
        const int n_periods, n_experiences, n_lagged_experiences;

	MultiIterator period_lagged_experineces;
	LimitedMultiIterator experiences;

	GenericMultiIterator<MultiIterator*> impl;

        uint32_t getPeriod() const { return period_lagged_experineces.vcurrent[0]; }

        const uint32_t* getExperiences() const
	{
		return reinterpret_cast<const uint32_t*>(&period_lagged_experineces.vcurrent[1]);
	}

        const uint32_t* getLaggedExperiences() const
	{
		return reinterpret_cast<const uint32_t*>(&experiences.vcurrent[0]);
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
		period_lagged_experineces(std::vector<uint32_t>(n_lagged_experiences_, n_choices)),
		// TODO The period value is not yet in place.
		experiences(experiences_, period_lagged_experineces.vcurrent[0]),
                impl({ &period_lagged_experineces, &experiences }) { }

        // TODO Add a constructor with opaque ChoicesIteratorState, containing the current state.
};

int main(int argc, char* argv[])
{
	ChoicesIterator choices(76, {{ 31, 35, 10 }}, 1, 20);

	int nchoices = 0;
	do
	{
		nchoices++;
	}
	while (choices.next());

	printf("%d choices visited\n", nchoices);

	return 0;
}

