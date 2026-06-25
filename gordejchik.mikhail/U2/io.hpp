#ifndef GORDEJCHIK_IO_HPP
#define GORDEJCHIK_IO_HPP

#include <iostream>
#include <cstddef>
#include "dynarray.hpp"
#include "person.hpp"
#include "meeting.hpp"

namespace gordejchik {
  const size_t npos = static_cast< size_t >(-1);

  struct read_result_t {
    size_t success_;
    size_t ignored_;
  };

  size_t findPersonById(const dynarray_t< person_t >& arr, size_t id);
  read_result_t readPersons(std::istream& in, dynarray_t< person_t >& arr);
  void writePersons(std::ostream& out, const dynarray_t< person_t >& arr);
  void writeDescribedPersons(std::ostream& out, const dynarray_t< person_t >& arr);
  int readMeetings(std::istream& in, dynarray_t< meeting_t >& meetings, dynarray_t< person_t >& persons);
}

#endif
