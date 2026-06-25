#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include "dynarray.hpp"
#include "person.hpp"
#include "io.hpp"

struct args_t {
  std::string inFile_;
  std::string outFile_;
  bool hasIn_;
  bool hasOut_;
};

static int parseArgs(int argc, char* argv[], args_t& args)
{
  args.hasIn_ = false;
  args.hasOut_ = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.size() > 3 && arg.compare(0, 3, "in:") == 0) {
      if (args.hasIn_) {
        return 1;
      }
      args.inFile_ = arg.substr(3);
      args.hasIn_ = true;
    } else if (arg.size() > 4 && arg.compare(0, 4, "out:") == 0) {
      if (args.hasOut_) {
        return 1;
      }
      args.outFile_ = arg.substr(4);
      args.hasOut_ = true;
    } else {
      return 1;
    }
  }
  return 0;
}

int main(int argc, char* argv[])
{
  args_t args;
  if (parseArgs(argc, argv, args) != 0) {
    std::cerr << "Invalid arguments" << "\n";
    return 1;
  }

  gordejchik::dynarray_t< gordejchik::Person > persons;
  gordejchik::init(persons);
  gordejchik::read_result_t counts;
  counts.success_ = 0;
  counts.ignored_ = 0;

  if (args.hasIn_) {
    std::ifstream fin(args.inFile_);
    if (!fin.is_open()) {
      std::cerr << "Не получается открыть input файл" << "\n";
      return 2;
    }
    counts = gordejchik::readPersons(fin, persons);
  } else {
    counts = gordejchik::readPersons(std::cin, persons);
  }

  if (args.hasOut_) {
    std::ofstream fout(args.outFile_);
    if (!fout.is_open()) {
      gordejchik::destroy(persons);
      std::cerr << "Не получается открыть output файл" << "\n";
      return 2;
    }
    gordejchik::writePersons(fout, persons);
  } else {
    gordejchik::writePersons(std::cout, persons);
  }

  std::cerr << counts.success_ << " " << counts.ignored_ << "\n";
  gordejchik::destroy(persons);
  return 0;
}
