// StringUtil.h - small text helpers that reproduce the MFC behaviours the
// original code relied on (CString::Mid clamping, _ttoi/_ttof leading-number
// parsing, whitespace tokenisation).
#pragma once

#include <cctype>
#include <cstdlib>
#include <string>
#include <vector>

namespace vm {

// Like CString::Mid(start, len): clamps to the string bounds and never throws.
inline std::string SafeMid(const std::string& s, std::size_t start, std::size_t len) {
    if (start >= s.size()) return std::string();
    return s.substr(start, len);
}

// Like _ttoi: parse a leading integer, ignore trailing junk, 0 if none.
inline int ParseInt(const std::string& s) {
    return static_cast<int>(std::strtol(s.c_str(), nullptr, 10));
}

// Like _ttof: parse a leading double, ignore trailing junk, 0.0 if none.
inline double ParseDouble(const std::string& s) {
    return std::strtod(s.c_str(), nullptr);
}

inline std::string Trim(const std::string& s) {
    const char* ws = " \t\r\n\f\v";
    const auto b = s.find_first_not_of(ws);
    if (b == std::string::npos) return std::string();
    const auto e = s.find_last_not_of(ws);
    return s.substr(b, e - b + 1);
}

inline bool StartsWith(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

// True if the trimmed string is a base-10 integer (optionally signed).
inline bool IsInteger(const std::string& s) {
    const std::string t = Trim(s);
    if (t.empty()) return false;
    std::size_t i = (t[0] == '+' || t[0] == '-') ? 1 : 0;
    if (i >= t.size()) return false;
    for (; i < t.size(); ++i)
        if (!std::isdigit(static_cast<unsigned char>(t[i]))) return false;
    return true;
}

// True if the trimmed string parses fully as a floating-point number.
inline bool IsNumber(const std::string& s) {
    const std::string t = Trim(s);
    if (t.empty()) return false;
    char* end = nullptr;
    std::strtod(t.c_str(), &end);
    return end == t.c_str() + t.size();
}

// Split on any of the delimiter characters, dropping empty tokens.
// Matches GeneralFunction::SplitString semantics (runs of delimiters collapse).
inline std::vector<std::string> Split(const std::string& s, const std::string& delims) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (delims.find(c) != std::string::npos) {
            if (!cur.empty()) { out.push_back(cur); cur.clear(); }
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

} // namespace vm
