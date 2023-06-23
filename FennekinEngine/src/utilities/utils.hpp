#pragma once

#include <limits.h>
#include <stdlib.h>

#include <regex>
#include <string>

#include <filesystem>
#include <string>


static inline float lerp(const float t_a, const float t_b, const float t_weight) {
  return t_a + t_weight * (t_b - t_a);
}

static inline std::string lTrim(std::string t_s) {
  t_s.erase(t_s.begin(), std::ranges::find_if(t_s,
                                          [](const int t_ch) { return !std::isspace(t_ch); }));
  return t_s;
}

static inline std::string rtrim(std::string t_s) {
  t_s.erase(std::find_if(t_s.rbegin(), t_s.rend(),
                       [](const int t_ch) { return !std::isspace(t_ch); })
              .base(),
          t_s.end());
  return t_s;
}

static inline std::string trim(const std::string& t_s) { return lTrim(rtrim(t_s)); }

static inline bool stringHasSuffix(const std::string& t_str,
                                     const std::string& t_suffix) {
  return t_str.size() >= t_suffix.size() &&
         t_str.compare(t_str.size() - t_suffix.size(), t_suffix.size(), t_suffix) == 0;
}

static inline std::string resolvePath(const std::string& t_path) {
  const std::filesystem::path absolutePath = std::filesystem::absolute(t_path);
	return absolutePath.string();
}

template <class TBiDirIt, class TTraits, class TCharT, class TUnaryFunction>
std::basic_string<TCharT> regexReplace(TBiDirIt t_first, TBiDirIt t_last,
                                      const std::basic_regex<TCharT, TTraits>& t_re,
                                      TUnaryFunction t_f) {
  std::basic_string<TCharT> s;

  typename std::match_results<TBiDirIt>::difference_type positionOfLastMatch = 0;
  auto endOfLastMatch = t_first;

  auto callback = [&](const std::match_results<TBiDirIt>& t_match) {
    auto positionOfThisMatch = t_match.position(0);
    auto diff = positionOfThisMatch - positionOfLastMatch;

    auto startOfThisMatch = endOfLastMatch;
    std::advance(startOfThisMatch, diff);

    s.append(endOfLastMatch, startOfThisMatch);
    s.append(t_f(t_match));

    auto lengthOfMatch = t_match.length(0);

    positionOfLastMatch = positionOfThisMatch + lengthOfMatch;

    endOfLastMatch = startOfThisMatch;
    std::advance(endOfLastMatch, lengthOfMatch);
  };

  std::sregex_iterator begin(t_first, t_last, t_re), end;
  std::for_each(begin, end, callback);

  s.append(endOfLastMatch, t_last);

  return s;
}

template <class TTraits, class TCharT, class TUnaryFunction>
std::string regexReplace(const std::string& t_s,
                         const std::basic_regex<TCharT, TTraits>& t_re,
                         TUnaryFunction t_f) {
  return regexReplace(t_s.cbegin(), t_s.cend(), t_re, t_f);
}
