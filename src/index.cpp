#include "minigit/index.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <stdexcept>
#include <algorithm>

using namespace std;

namespace minigit {

static uint32_t read_be32(const char* buf) {
    const auto* u = reinterpret_cast<const unsigned char*>(buf);
    return (uint32_t(u[0]) << 24) | (uint32_t(u[1]) << 16) | (uint32_t(u[2]) << 8) | uint32_t(u[3]);
}

static uint16_t read_be16(const char* buf) {
    const auto* u = reinterpret_cast<const unsigned char*>(buf);
    return (uint16_t(u[0]) << 8) | uint16_t(u[1]);
}

static void write_be32(ostream& out, uint32_t val) {
    unsigned char buf[4] = {
        static_cast<unsigned char>((val >> 24) & 0xFF),
        static_cast<unsigned char>((val >> 16) & 0xFF),
        static_cast<unsigned char>((val >> 8) & 0xFF),
        static_cast<unsigned char>(val & 0xFF)
    };
    out.write(reinterpret_cast<char*>(buf), 4);
}

static void write_be16(ostream& out, uint16_t val) {
    unsigned char buf[2] = {
        static_cast<unsigned char>((val >> 8) & 0xFF),
        static_cast<unsigned char>(val & 0xFF)
    };
    out.write(reinterpret_cast<char*>(buf), 2);
}

Index Index::read_from_file(const Repository& repo) {
    fs::path index_file = repo.repo_file("index");
    if (!fs::exists(index_file)) return Index();

    ifstream in(index_file, ios::binary);
    string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

    if (content.size() < 12) return Index();

    string sig = content.substr(0, 4);
    if (sig != "DIRC") throw runtime_error("Invalid index file signature");

    uint32_t version = read_be32(content.data() + 4);
    uint32_t count = read_be32(content.data() + 8);

    Index index(version);
    size_t idx = 12;

    for (uint32_t i = 0; i < count; ++i) {
        IndexEntry entry{};
        entry.ctime_s = read_be32(content.data() + idx);
        entry.ctime_ns = read_be32(content.data() + idx + 4);
        entry.mtime_s = read_be32(content.data() + idx + 8);
        entry.mtime_ns = read_be32(content.data() + idx + 12);
        entry.dev = read_be32(content.data() + idx + 16);
        entry.ino = read_be32(content.data() + idx + 20);

        uint16_t mode = read_be16(content.data() + idx + 26);
        entry.mode_type = mode >> 12;
        entry.mode_perms = mode & 0777;

        entry.uid = read_be32(content.data() + idx + 28);
        entry.gid = read_be32(content.data() + idx + 32);
        entry.fsize = read_be32(content.data() + idx + 36);

        ostringstream sha_ss;
        for (size_t k = 0; k < 20; ++k) {
            sha_ss << hex << setw(2) << setfill('0')
                   << static_cast<int>(static_cast<unsigned char>(content[idx + 40 + k]));
        }
        entry.sha = sha_ss.str();

        uint16_t flags = read_be16(content.data() + idx + 60);
        entry.flag_assume_valid = (flags & 0x8000) != 0;
        entry.flag_stage = flags & 0x3000;
        size_t name_length = flags & 0x0FFF;

        idx += 62;

        string raw_name = content.substr(idx, name_length);
        entry.name = raw_name;
        idx += name_length + 1;

        idx = 8 * static_cast<size_t>(ceil(static_cast<double>(idx) / 8.0));
        index.add_entry(entry);
    }

    return index;
}

void Index::write_to_file(const Repository& repo) const {
    ofstream out(repo.repo_file("index"), ios::binary);

    out.write("DIRC", 4);
    write_be32(out, version_);
    write_be32(out, static_cast<uint32_t>(entries_.size()));

    size_t idx = 12;

    for (const auto& e : entries_) {
        write_be32(out, e.ctime_s);
        write_be32(out, e.ctime_ns);
        write_be32(out, e.mtime_s);
        write_be32(out, e.mtime_ns);
        write_be32(out, e.dev);
        write_be32(out, e.ino);

        uint32_t mode = (e.mode_type << 12) | e.mode_perms;
        write_be32(out, mode);

        write_be32(out, e.uid);
        write_be32(out, e.gid);
        write_be32(out, e.fsize);

        for (size_t i = 0; i < 40; i += 2) {
            uint8_t byte = static_cast<uint8_t>(stoul(e.sha.substr(i, 2), nullptr, 16));
            out.put(static_cast<char>(byte));
        }

        uint16_t flags = (e.flag_assume_valid ? 0x8000 : 0) | e.flag_stage | (e.name.size() & 0x0FFF);
        write_be16(out, flags);

        out.write(e.name.data(), e.name.size());
        out.put('\0');

        idx += 62 + e.name.size() + 1;

        if (idx % 8 != 0) {
            size_t pad = 8 - (idx % 8);
            for (size_t p = 0; p < pad; ++p) out.put('\0');
            idx += pad;
        }
    }
}

void Index::remove_entry(const string& path) {
    entries_.erase(remove_if(entries_.begin(), entries_.end(),
        [&path](const IndexEntry& e) { return e.name == path; }), entries_.end());
}

} // namespace minigit