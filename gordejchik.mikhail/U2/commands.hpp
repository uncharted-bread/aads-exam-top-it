#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include "dynarray.hpp"
#include "person.hpp"
#include "meeting.hpp"

namespace gordejchik {
  void runCommands(std::istream& in, dynarray_t< person_t >& persons, dynarray_t< meeting_t >& meetings);
}

#endif
