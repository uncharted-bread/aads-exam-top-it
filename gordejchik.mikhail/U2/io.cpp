#include "io.hpp"
#include <string>

static void stripCr(std::string& line)
{
  if (!line.empty() && line.back() == '\r') {
    line.pop_back();
  }
}

static bool parsePerson(const std::string& line, size_t& id, std::string& info)
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

static void ensurePerson(gordejchik::dynarray_t< gordejchik::person_t >& arr, size_t id)
{
  if (gordejchik::findPersonById(arr, id) != gordejchik::npos) {
    return;
  }
  gordejchik::person_t p;
  p.id_ = id;
  gordejchik::pushBack(arr, p);
}

size_t gordejchik::findPersonById(const dynarray_t< person_t >& arr, size_t id)
{
  for (size_t i = 0; i < arr.size_; ++i) {
    if (arr.data_[i].id_ == id) {
      return i;
    }
  }
  return npos;
}

gordejchik::read_result_t gordejchik::readPersons(std::istream& in,
    dynarray_t< person_t >& arr)
{
  read_result_t result;
  result.success_ = 0;
  result.ignored_ = 0;
  std::string line;
  while (std::getline(in, line)) {
    stripCr(line);
    size_t id = 0;
    std::string info;
    if (!parsePerson(line, id, info)) {
      ++result.ignored_;
      continue;
    }
    if (findPersonById(arr, id) != npos) {
      ++result.ignored_;
      continue;
    }
    person_t p;
    p.id_ = id;
    p.info_ = info;
    pushBack(arr, p);
    ++result.success_;
  }
  return result;
}

void gordejchik::writePersons(std::ostream& out, const dynarray_t< person_t >& arr)
{
  for (size_t i = 0; i < arr.size_; ++i) {
    out << arr.data_[i].id_ << " " << arr.data_[i].info_ << "\n";
  }
}

void gordejchik::writeDescribedPersons(std::ostream& out,
    const dynarray_t< person_t >& arr)
{
  for (size_t i = 0; i < arr.size_; ++i) {
    if (!arr.data_[i].info_.empty()) {
      out << arr.data_[i].id_ << " " << arr.data_[i].info_ << "\n";
    }
  }
}

int gordejchik::readMeetings(std::istream& in, dynarray_t< meeting_t >& meetings,
    dynarray_t< person_t >& persons)
{
  size_t id1 = 0;
  size_t id2 = 0;
  size_t dur = 0;
  while (in >> id1 >> id2 >> dur) {
    if (id1 == id2) {
      continue;
    }
    ensurePerson(persons, id1);
    ensurePerson(persons, id2);
    meeting_t m;
    m.id1_ = id1;
    m.id2_ = id2;
    m.duration_ = dur;
    pushBack(meetings, m);
  }
  if (!in.eof()) {
    return 3;
  }
  return 0;
}
