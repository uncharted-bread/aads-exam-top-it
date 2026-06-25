#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include "dynarray.hpp"
#include "person.hpp"
#include "meeting.hpp"
#include "io.hpp"
#include "commands.hpp"

namespace gordejchik {
  struct args_t {
    std::string personsFile_;
    std::string dataFile_;
    bool hasPersons_;
    bool hasData_;
  };
}

static int parseArgs(int argc, char* argv[], gordejchik::args_t& args)
{
  args.hasPersons_ = false;
  args.hasData_ = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.size() > 3 && arg.compare(0, 3, "in:") == 0) {
      if (args.hasPersons_) {
        return 1;
      }
      args.personsFile_ = arg.substr(3);
      args.hasPersons_ = true;
    } else if (arg.size() > 5 && arg.compare(0, 5, "data:") == 0) {
      if (args.hasData_) {
        return 1;
      }
      args.dataFile_ = arg.substr(5);
      args.hasData_ = true;
    } else {
      return 1;
    }
  }
  if (!args.hasData_) {
    return 1;
  }
  return 0;
}

int main(int argc, char* argv[])
{
  gordejchik::args_t args;
  if (parseArgs(argc, argv, args) != 0) {
    std::cerr << "Invalid arguments" << "\n";
    return 1;
  }

  gordejchik::dynarray_t< gordejchik::person_t > persons;
  gordejchik::init(persons);
  gordejchik::dynarray_t< gordejchik::meeting_t > meetings;
  gordejchik::init(meetings);

  if (args.hasPersons_) {
    std::ifstream fin(args.personsFile_);
    if (!fin.is_open()) {
      std::cerr << "Cannot open persons file" << "\n";
      return 2;
    }
    gordejchik::readPersons(fin, persons);
  }

  std::ifstream fdata(args.dataFile_);
  if (!fdata.is_open()) {
    gordejchik::destroy(persons);
    std::cerr << "Cannot open data file" << "\n";
    return 2;
  }
  int meetResult = gordejchik::readMeetings(fdata, meetings, persons);
  if (meetResult != 0) {
    gordejchik::destroy(persons);
    gordejchik::destroy(meetings);
    std::cerr << "Malformed meeting data" << "\n";
    return meetResult;
  }

  gordejchik::runCommands(std::cin, persons, meetings);

  gordejchik::destroy(persons);
  gordejchik::destroy(meetings);
  return 0;
}
