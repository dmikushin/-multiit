#include "multiit.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>

using namespace multiit::runtime;

namespace py = pybind11;

PYBIND11_MODULE(glaphyra, glaphyra)
{
	py::class_<MultiIterator>(glaphyra, "MultiIterator")
		.def(py::init<const std::vector<uint32_t>&>(), "")
		.def(py::init<const std::vector<uint32_t>&, const std::vector<uint32_t>&>(), "")
		.def("current", [](MultiIterator const & self) { return *(const std::vector<uint32_t>*)&self; }, "")
		.def("next", &MultiIterator::next)
		.def("reset", &MultiIterator::reset);
}

