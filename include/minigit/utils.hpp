#ifndef MINIGIT_UTILS_HPP
#define MINIGIT_UTILS_HPP

#include <string>
#include <vector>
#include <map>

using namespace std;

namespace minigit::utils {

string compute_sha1(const string& input);
string compress_data(const string& data);
string decompress_data(const string& compressed_data);

using KVLM = map<string, vector<string>>;
KVLM parse_kvlm(const string& raw, size_t start = 0);
string serialize_kvlm(const KVLM& kvlm, const string& message);

} // namespace minigit::utils

#endif