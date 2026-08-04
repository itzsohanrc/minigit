#ifndef MINIGIT_INDEX_HPP
#define MINIGIT_INDEX_HPP

#include "repository.hpp"
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

namespace minigit {

struct IndexEntry {
    uint32_t ctime_s;
    uint32_t ctime_ns;
    uint32_t mtime_s;
    uint32_t mtime_ns;
    uint32_t dev;
    uint32_t ino;
    uint32_t mode_type;
    uint32_t mode_perms;
    uint32_t uid;
    uint32_t gid;
    uint32_t fsize;
    string sha;
    bool flag_assume_valid;
    uint16_t flag_stage;
    string name;
};

class Index {
private:
    uint32_t version_ = 2;
    vector<IndexEntry> entries_;

public:
    Index(uint32_t version = 2) : version_(version) {}

    static Index read_from_file(const Repository& repo);
    void write_to_file(const Repository& repo) const;

    void add_entry(IndexEntry entry) { entries_.push_back(std::move(entry)); }
    void remove_entry(const string& path);

    const vector<IndexEntry>& get_entries() const { return entries_; }
    vector<IndexEntry>& get_entries() { return entries_; }
};

} // namespace minigit

#endif