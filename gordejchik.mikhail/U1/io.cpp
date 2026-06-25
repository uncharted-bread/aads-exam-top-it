#include "io.hpp"
#include <string>

static void stripCr(std::string& line)
{
  if (!line.empty() && line.back() == '\r') {
    line.pop_back();
  }
}

static bool hasId(const gordejchik::dynarray_t< gordejchik::Person >& arr, size_t id)
{
  for (size_t i = 0; i < arr.size_; ++i) {
    if (arr.data_[i].id == id) {
      return true;
    }
  }
  return false;
}

static bool parseLine(const std::string& line, size_t& id, std::string& info)
{
  if (line.empty()) {
    return false;
  }
  size_t pos = 0;
  try {
    unsigned long long raw = std::stoull(line, &pos);
    id = static_cast< size_t >(raw);
  } catch (...) {
    return false;
  }
  if (pos == line.size()) {
    return false;
  }
  if (line[pos] != ' ' && line[pos] != '\t') {
    return false;
  }
  size_t descStart = pos;
  while (descStart < line.size() && (line[descStart] == ' ' || line[descStart] == '\t')) {
    ++descStart;
  }
  if (descStart == line.size()) {
    return false;
  }
  info = line.substr(descStart);
  return true;
}

gordejchik::read_result_t gordejchik::readPersons(std::istream& in, dynarray_t< Person >& arr)
{
  read_result_t result;
  result.success_ = 0;
  result.ignored_ = 0;
  std::string line;
  while (std::getline(in, line)) {
      stripCr(line);
      if (line.empty()) {
        continue;
      }
      size_t id = 0;
      std::string info;
      if (!parseLine(line, id, info)) {
        ++result.ignored_;
        continue;
      }
    if (hasId(arr, id)) {
      ++result.ignored_;
      continue;
    }
    Person p;
    p.id = id;
    p.info = info;
    pushBack(arr, p);
    ++result.success_;
  }
  return result;
}

void gordejchik::writePersons(std::ostream& out, const dynarray_t< Person >& arr)
{
  for (size_t i = 0; i < arr.size_; ++i) {
    out << arr.data_[i].id << " " << arr.data_[i].info << "\n";
  }
}
