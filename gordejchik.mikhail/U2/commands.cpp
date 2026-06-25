#include "commands.hpp"
#include "io.hpp"
#include <string>
#include <fstream>
#include <algorithm>

static void printInvalid()
{
  std::cout << "<INVALID COMMAND>" << "\n";
}

static void cmdAnons(const gordejchik::dynarray_t< gordejchik::person_t >& persons)
{
  gordejchik::dynarray_t< size_t > ids;
  gordejchik::init(ids);
  for (size_t i = 0; i < persons.size_; ++i) {
    if (persons.data_[i].info_.empty()) {
      gordejchik::pushBack(ids, persons.data_[i].id_);
    }
  }
  std::sort(ids.data_, ids.data_ + ids.size_);
  for (size_t i = 0; i < ids.size_; ++i) {
    std::cout << ids.data_[i] << "\n";
  }
  gordejchik::destroy(ids);
}

static void cmdDesc(const gordejchik::dynarray_t< gordejchik::person_t >& persons,
    size_t id)
{
  size_t idx = gordejchik::findPersonById(persons, id);
  if (idx == gordejchik::npos) {
    printInvalid();
    return;
  }
  if (persons.data_[idx].info_.empty()) {
    std::cout << "<ANON>" << "\n";
  } else {
    std::cout << persons.data_[idx].info_ << "\n";
  }
}

static void cmdRedesc(gordejchik::dynarray_t< gordejchik::person_t >& persons,
    size_t id, const std::string& desc)
{
  size_t idx = gordejchik::findPersonById(persons, id);
  if (idx == gordejchik::npos) {
    printInvalid();
    return;
  }
  persons.data_[idx].info_ = desc;
}

static void cmdOutPersons(const gordejchik::dynarray_t< gordejchik::person_t >& persons,
    const std::string& filename)
{
  std::ofstream fout(filename);
  if (!fout.is_open()) {
    printInvalid();
    return;
  }
  gordejchik::writeDescribedPersons(fout, persons);
}

static bool parseId(std::istream& in, size_t& id)
{
  if (!(in >> id)) {
    return false;
  }
  return true;
}

static bool parseQuotedDesc(std::istream& in, std::string& desc)
{
  std::string rest;
  std::getline(in, rest);
  size_t qStart = rest.find('"');
  size_t qEnd = rest.rfind('"');
  if (qStart == std::string::npos || qEnd == std::string::npos || qStart == qEnd) {
    return false;
  }
  desc = rest.substr(qStart + 1, qEnd - qStart - 1);
  return !desc.empty();
}

void gordejchik::runCommands(std::istream& in, dynarray_t< person_t >& persons,
    dynarray_t< meeting_t >& meetings)
{
  std::string cmd;
  while (in >> cmd) {
    if (cmd == "anons") {
      cmdAnons(persons);
    } else if (cmd == "desc") {
      size_t id = 0;
      if (!parseId(in, id)) {
        printInvalid();
        continue;
      }
      cmdDesc(persons, id);
    } else if (cmd == "redesc") {
      size_t id = 0;
      if (!parseId(in, id)) {
        printInvalid();
        continue;
      }
      std::string desc;
      if (!parseQuotedDesc(in, desc)) {
        printInvalid();
        continue;
      }
      cmdRedesc(persons, id, desc);
    } else if (cmd == "out-persons") {
      std::string filename;
      if (!(in >> filename)) {
        printInvalid();
        continue;
      }
      cmdOutPersons(persons, filename);
    } else {
      printInvalid();
      std::string rest;
      std::getline(in, rest);
    }
  }
}
