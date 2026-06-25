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

static bool comparePairs(const std::pair< size_t, size_t >& a,
    const std::pair< size_t, size_t >& b)
{
  if (a.first != b.first) {
    return a.first < b.first;
  }
  return a.second < b.second;
}

static void collectMeets(const gordejchik::dynarray_t< gordejchik::meeting_t >& meetings,
    size_t id, gordejchik::dynarray_t< std::pair< size_t, size_t > >& result)
{
  for (size_t i = 0; i < meetings.size_; ++i) {
    const gordejchik::meeting_t& m = meetings.data_[i];
    if (m.id1_ == id) {
      gordejchik::pushBack(result, std::pair< size_t, size_t >(m.id2_, m.duration_));
    } else if (m.id2_ == id) {
      gordejchik::pushBack(result, std::pair< size_t, size_t >(m.id1_, m.duration_));
    }
  }
}

static void printMeets(gordejchik::dynarray_t< std::pair< size_t, size_t > >& entries)
{
  std::sort(entries.data_, entries.data_ + entries.size_, comparePairs);
  for (size_t i = 0; i < entries.size_; ++i) {
    std::cout << entries.data_[i].first << " " << entries.data_[i].second << "\n";
  }
}

static void cmdMeets(const gordejchik::dynarray_t< gordejchik::person_t >& persons,
    const gordejchik::dynarray_t< gordejchik::meeting_t >& meetings,
    size_t id)
{
  if (gordejchik::findPersonById(persons, id) == gordejchik::npos) {
    printInvalid();
    return;
  }
  gordejchik::dynarray_t< std::pair< size_t, size_t > > entries;
  gordejchik::init(entries);
  collectMeets(meetings, id, entries);
  printMeets(entries);
  gordejchik::destroy(entries);
}

static void cmdLess(const gordejchik::dynarray_t< gordejchik::person_t >& persons,
    const gordejchik::dynarray_t< gordejchik::meeting_t >& meetings,
    size_t time, size_t id)
{
  if (gordejchik::findPersonById(persons, id) == gordejchik::npos) {
    printInvalid();
    return;
  }
  gordejchik::dynarray_t< std::pair< size_t, size_t > > entries;
  gordejchik::init(entries);
  for (size_t i = 0; i < meetings.size_; ++i) {
    const gordejchik::meeting_t& m = meetings.data_[i];
    if (m.duration_ >= time) {
      continue;
    }
    if (m.id1_ == id) {
      gordejchik::pushBack(entries, std::pair< size_t, size_t >(m.id2_, m.duration_));
    } else if (m.id2_ == id) {
      gordejchik::pushBack(entries, std::pair< size_t, size_t >(m.id1_, m.duration_));
    }
  }
  printMeets(entries);
  gordejchik::destroy(entries);
}

static void cmdGreater(const gordejchik::dynarray_t< gordejchik::person_t >& persons,
    const gordejchik::dynarray_t< gordejchik::meeting_t >& meetings,
    size_t time, size_t id)
{
  if (gordejchik::findPersonById(persons, id) == gordejchik::npos) {
    printInvalid();
    return;
  }
  gordejchik::dynarray_t< std::pair< size_t, size_t > > entries;
  gordejchik::init(entries);
  for (size_t i = 0; i < meetings.size_; ++i) {
    const gordejchik::meeting_t& m = meetings.data_[i];
    if (m.duration_ <= time) {
      continue;
    }
    if (m.id1_ == id) {
      gordejchik::pushBack(entries, std::pair< size_t, size_t >(m.id2_, m.duration_));
    } else if (m.id2_ == id) {
      gordejchik::pushBack(entries, std::pair< size_t, size_t >(m.id1_, m.duration_));
    }
  }
  printMeets(entries);
  gordejchik::destroy(entries);
}

static void cmdDeanon(gordejchik::dynarray_t< gordejchik::person_t >& persons,
    gordejchik::dynarray_t< gordejchik::meeting_t >& meetings,
    size_t anonId, size_t targetId)
{
  size_t anonIdx = gordejchik::findPersonById(persons, anonId);
  size_t targetIdx = gordejchik::findPersonById(persons, targetId);
  if (anonIdx == gordejchik::npos || targetIdx == gordejchik::npos) {
    printInvalid();
    return;
  }
  if (!persons.data_[anonIdx].info_.empty() || persons.data_[targetIdx].info_.empty()) {
    printInvalid();
    return;
  }
  for (size_t i = 0; i < meetings.size_; ++i) {
    if (meetings.data_[i].id1_ == anonId) {
      meetings.data_[i].id1_ = targetId;
    }
    if (meetings.data_[i].id2_ == anonId) {
      meetings.data_[i].id2_ = targetId;
    }
  }
  size_t i = meetings.size_;
  while (i > 0) {
    --i;
    if (meetings.data_[i].id1_ == meetings.data_[i].id2_) {
      gordejchik::removeAt(meetings, i);
    }
  }
  gordejchik::removeAt(persons, anonIdx);
}

static void cmdCommons(const gordejchik::dynarray_t< gordejchik::person_t >& persons,
    const gordejchik::dynarray_t< gordejchik::meeting_t >& meetings,
    size_t id1, size_t id2)
{
  if (gordejchik::findPersonById(persons, id1) == gordejchik::npos
      || gordejchik::findPersonById(persons, id2) == gordejchik::npos) {
    printInvalid();
    return;
  }
  gordejchik::dynarray_t< size_t > partners1;
  gordejchik::init(partners1);
  gordejchik::dynarray_t< size_t > partners2;
  gordejchik::init(partners2);
  for (size_t i = 0; i < meetings.size_; ++i) {
    const gordejchik::meeting_t& m = meetings.data_[i];
    if (m.id1_ == id1) {
      gordejchik::pushBack(partners1, m.id2_);
    } else if (m.id2_ == id1) {
      gordejchik::pushBack(partners1, m.id1_);
    }
    if (m.id1_ == id2) {
      gordejchik::pushBack(partners2, m.id2_);
    } else if (m.id2_ == id2) {
      gordejchik::pushBack(partners2, m.id1_);
    }
  }
  std::sort(partners1.data_, partners1.data_ + partners1.size_);
  std::sort(partners2.data_, partners2.data_ + partners2.size_);
  size_t i = 0;
  size_t j = 0;
  size_t prev = gordejchik::npos;
  while (i < partners1.size_ && j < partners2.size_) {
    if (partners1.data_[i] < partners2.data_[j]) {
      ++i;
    } else if (partners1.data_[i] > partners2.data_[j]) {
      ++j;
    } else {
      if (partners1.data_[i] != prev) {
        std::cout << partners1.data_[i] << "\n";
        prev = partners1.data_[i];
      }
      ++i;
      ++j;
    }
  }
  gordejchik::destroy(partners1);
  gordejchik::destroy(partners2);
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
      } else if (cmd == "meets") {
      size_t id = 0;
      if (!parseId(in, id)) {
        printInvalid();
        continue;
      }
      cmdMeets(persons, meetings, id);
    } else if (cmd == "less") {
      size_t time = 0;
      size_t id = 0;
      if (!(in >> time) || !parseId(in, id)) {
        printInvalid();
        continue;
      }
      cmdLess(persons, meetings, time, id);
    } else if (cmd == "greater") {
      size_t time = 0;
      size_t id = 0;
      if (!(in >> time) || !parseId(in, id)) {
        printInvalid();
        continue;
      }
      cmdGreater(persons, meetings, time, id);
    } else if (cmd == "deanon") {
      size_t anonId = 0;
      size_t targetId = 0;
      if (!parseId(in, anonId) || !parseId(in, targetId)) {
        printInvalid();
        continue;
      }
      cmdDeanon(persons, meetings, anonId, targetId);
    } else if (cmd == "commons") {
      size_t id1 = 0;
      size_t id2 = 0;
      if (!parseId(in, id1) || !parseId(in, id2)) {
        printInvalid();
        continue;
      }
      cmdCommons(persons, meetings, id1, id2);
    } else {
      printInvalid();
      std::string rest;
      std::getline(in, rest);
    }
  }
}
