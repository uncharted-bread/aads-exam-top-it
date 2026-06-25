#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include "dynarray.hpp"
#include "person.hpp"

static bool parseArgs(int argc, char* argv[], std::string& inFile, std::string& outFile)
{
  if (argc > 3) {
    return false;
  }
  bool hasIn = false;
  bool hasOut = false;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.rfind("in:", 0) == 0) {
      if (hasIn) {
        return false;
      }
      inFile = arg.substr(3);
      hasIn = true;
    } else if (arg.rfind("out:", 0) == 0) {
      if (hasOut) {
        return false;
      }
      outFile = arg.substr(4);
      hasOut = true;
    } else {
      return false;
    }
  }
  return true;
}

int main(int argc, char* argv[])
{
  if (argc > 3) {
    std::cerr << "Too many arguments" << "\n";
    return 0;
  }
  std::string inFile;
  std::string outFile;
  if (!parseArgs(argc, argv, inFile, outFile)) {
    std::cerr << "Invalid arguments" << "\n";
    return 1;
  }

  std::ifstream fin;
  if (!inFile.empty()) {
    fin.open(inFile);
    if (!fin.is_open()) {
      std::cerr << "Cannot open input file" << "\n";
      return 2;
    }
  }
  std::istream& input = inFile.empty() ? std::cin : fin;

  gordejchik::dynarray_t< gordejchik::Person > persons;
  gordejchik::init(persons);
  size_t success = 0;
  size_t ignored = 0;

  std::string line;
  while (std::getline(input, line)) {
    size_t first = line.find_first_not_of(" \t");
    if (first == std::string::npos) {
      continue;
    }
    size_t space = line.find_first_of(" \t", first);
    if (space == std::string::npos) {
      ++ignored;
      continue;
    }
    std::string idStr = line.substr(first, space - first);
    size_t descStart = line.find_first_not_of(" \t", space);
    if (descStart == std::string::npos) {
      ++ignored;
      continue;
    }
    std::string description = line.substr(descStart);
    if (description.find_first_not_of(" \t") == std::string::npos) {
      ++ignored;
      continue;
    }
    size_t id = 0;
    try {
      size_t pos = 0;
      unsigned long long raw = std::stoull(idStr, &pos);
      if (pos != idStr.size()) {
        ++ignored;
        continue;
      }
      id = static_cast< size_t >(raw);
    } catch (...) {
      ++ignored;
      continue;
    }
    bool duplicate = false;
    for (size_t i = 0; i < persons.size_; ++i) {
      if (persons.data_[i].id == id) {
        duplicate = true;
        break;
      }
    }
    if (duplicate) {
      ++ignored;
      continue;
    }
    gordejchik::Person p;
    p.id = id;
    p.info = description;
    gordejchik::pushBack(persons, p);
    ++success;
  }

  std::ofstream fout;
  if (!outFile.empty()) {
    fout.open(outFile);
    if (!fout.is_open()) {
      std::cerr << "Cannot open output file" << "\n";
      gordejchik::destroy(persons);
      return 2;
    }
  }
  std::ostream& output = outFile.empty() ? std::cout : fout;

  for (size_t i = 0; i < persons.size_; ++i) {
    output << persons.data_[i].id << " " << persons.data_[i].info << "\n";
  }

  std::cerr << "\n" << "\n";
  gordejchik::destroy(persons);
  return 0;
}
