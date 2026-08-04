#ifndef MINIGIT_COMMANDS_HPP
#define MINIGIT_COMMANDS_HPP

#include <vector>
#include <string>

using namespace std;

namespace minigit::commands {

int cmd_init(const vector<string>& args);
int cmd_hash_object(const vector<string>& args);
int cmd_cat_file(const vector<string>& args);
int cmd_ls_files(const vector<string>& args);
int cmd_add(const vector<string>& args);
int cmd_commit(const vector<string>& args);
int cmd_log(const vector<string>& args);

} // namespace minigit::commands

#endif