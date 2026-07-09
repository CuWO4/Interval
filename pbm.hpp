#ifndef INTERVAL_PBM_HPP_
#define INTERVAL_PBM_HPP_

#include <array>
#include <string_view>
#include <fstream>
#include <cstddef>

template<std::size_t H, std::size_t W>
int save(const std::array<std::array<bool, W>, H>& canvas, std::string_view path) {
  std::ofstream out(path.data(), std::ios::out | std::ios::trunc);
  if (!out) {
    return -1;
  }

  out << "P1\n" << W << ' ' << H << '\n';

  for (int i = H - 1; i >= 0; i--) {
    for (int j = 0; j < (int) W; j++) {
      out << (canvas[i][j] ? '1' : '0') << ' ';
    }
    out << "\n";
  }

  return out.good() ? 0 : -2;
}

#endif /* !defined(INTERVAL_PBM_HPP_) */