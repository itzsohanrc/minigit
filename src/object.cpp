#include "minigit/object.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

using namespace std;

namespace minigit {

string Object::write_object(const Repository* repo) const {
    string data = serialize();
    string header = get_format_string() + " " + to_string(data.size()) + '\0';
    string full_content = header + data;

    string sha = utils::compute_sha1(full_content);

    if (repo) {
        fs::path path = repo->repo_file(fs::path("objects") / sha.substr(0, 2) / sha.substr(2), true);
        if (!fs::exists(path)) {
            ofstream out(path, ios::binary);
            string compressed = utils::compress_data(full_content);
            out.write(compressed.data(), compressed.size());
        }
    }
    return sha;
}

unique_ptr<Object> Object::read_object(const Repository& repo, const string& sha) {
    fs::path path = repo.repo_file(fs::path("objects") / sha.substr(0, 2) / sha.substr(2));
    if (!fs::exists(path)) return nullptr;

    ifstream in(path, ios::binary);
    string compressed((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    string raw = utils::decompress_data(compressed);

    size_t space_pos = raw.find(' ');
    string fmt = raw.substr(0, space_pos);

    size_t null_pos = raw.find('\0', space_pos);
    size_t size = stoul(raw.substr(space_pos + 1, null_pos - space_pos - 1));

    string data = raw.substr(null_pos + 1);
    if (data.size() != size) {
        throw runtime_error("Malformed object " + sha + ": bad length");
    }

    unique_ptr<Object> obj;
    if (fmt == "blob") obj = make_unique<Blob>();
    else if (fmt == "tree") obj = make_unique<Tree>();
    else if (fmt == "commit") obj = make_unique<Commit>();
    else throw runtime_error("Unknown type " + fmt);

    obj->deserialize(data);
    return obj;
}

string Object::find_object(const Repository& repo, const string& name, const string& fmt, bool follow) {
    string sha = name;
    if (name == "HEAD") {
        fs::path head_file = repo.repo_file("HEAD");
        if (fs::exists(head_file)) {
            ifstream in(head_file);
            string ref;
            getline(in, ref);
            if (ref.rfind("ref: ", 0) == 0) {
                ref = ref.substr(5);
                if (!ref.empty() && ref.back() == '\r') ref.pop_back();
                fs::path ref_path = repo.repo_file(ref);
                if (fs::exists(ref_path)) {
                    ifstream ref_in(ref_path);
                    getline(ref_in, sha);
                }
            } else {
                sha = ref;
            }
        }
    }

    if (fmt.empty()) return sha;

    while (true) {
        auto obj = read_object(repo, sha);
        if (!obj) break;

        if (obj->get_format_string() == fmt) return sha;
        if (!follow) break;

        if (obj->get_format_string() == "commit" && fmt == "tree") {
            auto commit = dynamic_cast<Commit*>(obj.get());
            sha = commit->get_kvlm().at("tree")[0];
        } else {
            break;
        }
    }
    return sha;
}

string Tree::serialize() const {
    string result;
    for (const auto& item : items_) {
        result += item.mode + " " + item.path + '\0';
        for (size_t i = 0; i < 40; i += 2) {
            uint8_t byte = static_cast<uint8_t>(stoul(item.sha.substr(i, 2), nullptr, 16));
            result.push_back(static_cast<char>(byte));
        }
    }
    return result;
}

void Tree::deserialize(const string& data) {
    items_.clear();
    size_t pos = 0;
    while (pos < data.size()) {
        size_t space = data.find(' ', pos);
        string mode = data.substr(pos, space - pos);

        size_t null_p = data.find('\0', space);
        string path = data.substr(space + 1, null_p - space - 1);

        pos = null_p + 1;
        ostringstream sha_ss;
        for (size_t i = 0; i < 20; ++i) {
            sha_ss << hex << setw(2) << setfill('0')
                   << static_cast<int>(static_cast<unsigned char>(data[pos + i]));
        }
        pos += 20;

        items_.push_back({mode, path, sha_ss.str()});
    }
}

string Commit::serialize() const {
    string msg = kvlm_.count("msg") ? kvlm_.at("msg")[0] : "";
    return utils::serialize_kvlm(kvlm_, msg);
}

void Commit::deserialize(const string& data) {
    kvlm_ = utils::parse_kvlm(data);
}

} // namespace minigit