#ifndef MINIGIT_OBJECT_HPP
#define MINIGIT_OBJECT_HPP

#include "repository.hpp"
#include "utils.hpp"
#include <string>
#include <vector>
#include <memory>

using namespace std;

namespace minigit {

enum class ObjectType { BLOB, TREE, COMMIT, TAG };

class Object {
public:
    virtual ~Object() = default;

    virtual string serialize() const = 0;
    virtual void deserialize(const string& data) = 0;
    virtual string get_format_string() const = 0;

    string write_object(const Repository* repo = nullptr) const;
    static unique_ptr<Object> read_object(const Repository& repo, const string& sha);
    static string find_object(const Repository& repo, const string& name, const string& fmt = "", bool follow = true);
};

class Blob : public Object {
private:
    string blobdata_;

public:
    Blob() = default;
    explicit Blob(string data) : blobdata_(std::move(data)) {}

    string serialize() const override { return blobdata_; }
    void deserialize(const string& data) override { blobdata_ = data; }
    string get_format_string() const override { return "blob"; }

    const string& get_data() const { return blobdata_; }
};

struct TreeLeaf {
    string mode;
    string path;
    string sha;
};

class Tree : public Object {
private:
    vector<TreeLeaf> items_;

public:
    Tree() = default;

    string serialize() const override;
    void deserialize(const string& data) override;
    string get_format_string() const override { return "tree"; }

    void add_item(TreeLeaf leaf) { items_.push_back(std::move(leaf)); }
    const vector<TreeLeaf>& get_items() const { return items_; }
    vector<TreeLeaf>& get_items() { return items_; }
};

class Commit : public Object {
private:
    utils::KVLM kvlm_;

public:
    Commit() = default;

    string serialize() const override;
    void deserialize(const string& data) override;
    string get_format_string() const override { return "commit"; }

    utils::KVLM& get_kvlm() { return kvlm_; }
    const utils::KVLM& get_kvlm() const { return kvlm_; }
};

} // namespace minigit

#endif