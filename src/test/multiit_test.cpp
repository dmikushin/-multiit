#include "multiit.h"

// A group of indexes tailored to the choices matrix.
struct ChoicesIterator
{
        const int n_periods, n_experiences, n_lagged_experiences;

	MultiIterator period_lagged_experineces;
	LimitedMultiIterator experiences;

	GenericMultiIterator<MultiIterator*> impl;

        uint32_t period() const { return period_lagged_experineces.vcurrent[0]; }

        const uint32_t* experiences() const
	{
		return reinterpret_cast<uint32_t*>(&period_lagged_experineces.vcurrent[1]);
	}

        const uint32_t* lagged_experiences() const
	{
		return reinterpret_cast<uint32_t*>(&experiences.vcurrent[0]);
	}

        // FIXME Cannot copy-pass, because the period is referenced => std::move?
        static std::vector<MultiIterator*> createGenericDimensions(
                int n_periods, const std::vector<uint32_t>& experiences,
                int n_lagged_experiences, int n_choices)
        {
                // TODO Create a multi iterator of period and lagged experiences.

                // TODO Create a limited multi iterator of experiences.
        }

	// TODO Avoid vectors, as we want it to be compatbile with GPUs.
        ChoicesIterator(int n_periods_, const std::vector<uint32_t>& experiences,
                int n_lagged_experiences_, int n_choices) :
                n_periods(n_periods_), n_experiences(n_experiences_), n_lagged_experiences(n_lagged_experiences_),
		// TODO MultiIterator(...)
		// TODO LimitedMultiIterator(...)
                GenericMultiIterator<MultiIterator*>(createGenericDimensions(n_periods_, experiences,
                        n_lagged_experiences_, n_choices)) { }

        // TODO Add a constructor with opaque ChoicesIteratorState, containing the current state.
};

