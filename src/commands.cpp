#include "minigit/commands.hpp"
#include "minigit/repository.hpp"
#include "minigit/object.hpp"
#include "minigit/index.hpp"
#include "minigit/utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <sys/stat.h>

using namespace std;

namespace minigit::commands {

int cmd_init(const vector<string>& args) {
    fs::path target = args.empty() ? fs::current_path() : fs::path(args[0]);
    Repository::create(target);
    cout << "Initialized empty MiniGit repository in " << fs::canonical(target / ".minigit") << "\n";
    return 0;
}

int cmd_hash_object(const vector<string>& args) {
    if (args.empty()) return 1;
    bool write = false;
    string filename;

    for (const auto& arg : args) {
        if (arg == "-w") write = true;
        else filename = arg;
    }

    ifstream in(filename, ios::binary);
    string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

    Blob blob(content);
    auto repo = Repository::find(fs::current_path(), false);
    string sha = blob.write_object(write && repo ? &(*repo) : nullptr);
    cout << sha << "\n";
    return 0;
}

int cmd_cat_file(const vector<string>& args) {
    if (args.size() < 2) return 1;
    string sha_arg = args[1];
    auto repo = Repository::find();
    string sha = Object::find_object(*repo, sha_arg, args[0]);

    auto obj = Object::read_object(*repo, sha);
    if (obj) {
        cout << obj->serialize();
    }
    return 0;
}

int cmd_ls_files(const vector<string>& args) {
    auto repo = Repository::find();
    Index index = Index::read_from_file(*repo);

    for (const auto& entry : index.get_entries()) {
        cout << entry.name << "\n";
    }
    return 0;
}

int cmd_add(const vector<string>& args) {
    if (args.empty()) return 1;
    auto repo = Repository::find();
    Index index = Index::read_from_file(*repo);

    for (const auto& filepath : args) {
        fs::path abs_path = fs::absolute(filepath);
        string rel_path = fs::relative(abs_path, repo->worktree()).string();

        ifstream in(abs_path, ios::binary);
        string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

        Blob blob(content);
        string sha = blob.write_object(&(*repo));

        struct stat st{};
        stat(abs_path.c_str(), &st);

        index.remove_entry(rel_path);

        IndexEntry entry{};
        entry.ctime_s = static_cast<uint32_t>(st.st_ctime);
        entry.mtime_s = static_cast<uint32_t>(st.st_mtime);
        entry.dev = static_cast<uint32_t>(st.st_dev);
        entry.ino = static_cast<uint32_t>(st.st_ino);
        entry.mode_type = 0b1000;
        entry.mode_perms = 0644;
        entry.uid = static_cast<uint32_t>(st.st_uid);
        entry.gid = static_cast<uint32_t>(st.st_gid);
        entry.fsize = static_cast<uint32_t>(st.st_size);
        entry.sha = sha;
        entry.name = rel_path;

        index.add_entry(entry);
    }

    index.write_to_file(*repo);
    return 0;
}

static string build_tree_from_index(const Repository& repo, const Index& index) {
    Tree root_tree;
    for (const auto& entry : index.get_entries()) {
        root_tree.add_item({"100644", entry.name, entry.sha});
    }
    return root_tree.write_object(&repo);
}

int cmd_commit(const vector<string>& args) {
    string message = "Commit";
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "-m" && i + 1 < args.size()) message = args[i + 1];
    }

    auto repo = Repository::find();
    Index index = Index::read_from_file(*repo);

    string tree_sha = build_tree_from_index(*repo, index);

    Commit commit;
    commit.get_kvlm()["tree"] = {tree_sha};

    string parent_sha = Object::find_object(*repo, "HEAD", "", false);
    if (!parent_sha.empty() && parent_sha != "HEAD") {
        commit.get_kvlm()["parent"] = {parent_sha};
    }

    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    string author = "Developer <dev@example.com> " + to_string(now) + " +0000";
    commit.get_kvlm()["author"] = {author};
    commit.get_kvlm()["committer"] = {author};
    commit.get_kvlm()["msg"] = {message + "\n"};

    string commit_sha = commit.write_object(&(*repo));

    ofstream head_file(repo->repo_file("HEAD"));
    head_file << commit_sha << "\n";

    cout << "[" << commit_sha.substr(0, 7) << "] " << message << "\n";
    return 0;
}

int cmd_log(const vector<string>& args) {
    auto repo = Repository::find();
    string sha = Object::find_object(*repo, args.empty() ? "HEAD" : args[0]);

    while (!sha.empty()) {
        auto obj = Object::read_object(*repo, sha);
        if (!obj || obj->get_format_string() != "commit") break;

        auto commit = dynamic_cast<Commit*>(obj.get());
        const auto& kvlm = commit->get_kvlm();

        cout << "commit " << sha << "\n";
        if (kvlm.count("author")) cout << "Author: " << kvlm.at("author")[0] << "\n";
        if (kvlm.count("msg")) cout << "\n    " << kvlm.at("msg")[0] << "\n";

        if (kvlm.count("parent")) {
            sha = kvlm.at("parent")[0];
        } else {
            break;
        }
    }
    return 0;
}

} // namespace minigit::commands