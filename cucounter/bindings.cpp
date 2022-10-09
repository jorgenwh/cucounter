#include <iostream>
#include <inttypes.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/numpy.h>

#include "hashtable.h"

namespace py = pybind11;

PYBIND11_MODULE(cucounter_C, m) {
  m.doc() = "Temporary documentation for the cucounter module";

  py::class_<HashTable>(m, "HashTable")
    .def(py::init([](py::array_t<uint64_t> &keys, const uint32_t capacity) { 
      const uint64_t *data = (uint64_t *)keys.data();
      const uint32_t size = keys.size();
      const bool cuda_keys = false;
      return new HashTable(data, cuda_keys, size, capacity);
    }))
    .def(py::init([](long keys_ptr, const uint32_t size, const uint32_t capacity) { 
      const uint64_t *data = reinterpret_cast<uint64_t*>(keys_ptr);
      const bool cuda_keys = true;
      return new HashTable(data, cuda_keys, size, capacity);
    }))
    .def("size", &HashTable::size)
    .def("capacity", &HashTable::capacity)
    .def("__repr__", &HashTable::to_string)
    .def("count", [](HashTable &self, py::array_t<uint64_t> &keys) {
      const uint64_t *data = (uint64_t *)keys.data();
      const uint32_t size = keys.size();
      self.count(data, size);
    })
    .def("count", [](HashTable &self, long data_ptr, uint32_t size) {
      uint64_t *data = reinterpret_cast<uint64_t*>(data_ptr);
      self.countcu(data, size);
    })
    .def("get", [](HashTable &self, py::array_t<uint64_t> &keys) {
      py::buffer_info buf = keys.request();

      const uint64_t *keys_data = (uint64_t *)keys.data();
      const uint32_t size = keys.size();

      auto ret = py::array_t<uint32_t>(buf.size);
      uint32_t *counts_data = ret.mutable_data();

      self.get(keys_data, counts_data, size);

      return ret;
    })
    .def("get", [](HashTable &self, long keys_ptr, long counts_ptr, uint32_t size) {
      uint64_t *keys = reinterpret_cast<uint64_t*>(keys_ptr);
      uint32_t *counts = reinterpret_cast<uint32_t*>(counts_ptr);
      self.getcu(keys, counts, size);
    })
    ;
}