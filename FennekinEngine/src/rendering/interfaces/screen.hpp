#pragma once

#include <ostream>

struct ImageSize {
    int width;
    int height;
};

inline bool operator==(const ImageSize& t_lhs, const ImageSize& t_rhs) {
    return t_lhs.width == t_rhs.width && t_lhs.height == t_rhs.height;
}
inline bool operator!=(const ImageSize& t_lhs, const ImageSize& t_rhs) {
    return !operator==(t_lhs, t_rhs);
}
inline std::ostream& operator<<(std::ostream& t_os, const ImageSize& t_size) {
    return t_os << "(" << t_size.width << ", " << t_size.height << ")";
}