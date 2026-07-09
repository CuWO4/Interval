#ifndef INTERVAL_INTERVAL_HPP_
#define INTERVAL_INTERVAL_HPP_

#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>

struct Interval {
  double lo, hi;
  Interval(double lo, double hi) : lo(lo), hi(hi) {}
  double len() { return hi - lo; }
  bool has(double p) { return lo <= p && p <= hi; }

  Interval operator+(Interval y) { return Interval(lo + y.lo, hi + y.hi); }
  Interval operator-(Interval y) { return Interval(lo - y.hi, hi - y.lo); }
  Interval operator*(Interval y) {
    return Interval(
      std::min({lo * y.lo, lo * y.hi, hi * y.lo, hi * y.hi}),
      std::max({lo * y.lo, lo * y.hi, hi * y.lo, hi * y.hi})
    );
  }

  friend std::ostream& operator<<(std::ostream& out, Interval self) {
    out << '[' << self.lo << ',' << self.hi << ']';
    return out;
  }
};

inline Interval sqrt(Interval x) {
  if (x.lo < 0 || x.hi < 0) {
    fprintf(stderr, "warning: sqrt([%lf, %lf])", x.lo, x.hi);
  }
  x.lo = std::max(0., x.lo);
  x.hi = std::max(0., x.hi);
  return Interval(std::sqrt(x.lo), std::sqrt(x.hi));
}

inline Interval sin(Interval x) {
  if (x.len() >= 2 * M_PI) return Interval(-1, 1);

  double min_val = std::min(std::sin(x.lo), std::sin(x.hi));
  double max_val = std::max(std::sin(x.lo), std::sin(x.hi));

  double k_min_peak = std::ceil((x.lo - M_PI / 2) / (2.0 * M_PI));
  double k_max_peak = std::floor((x.hi - M_PI / 2) / (2.0 * M_PI));
  if (k_min_peak <= k_max_peak) max_val = 1;

  double k_min_valley = std::ceil((x.lo + M_PI / 2) / (2.0 * M_PI));
  double k_max_valley = std::floor((x.hi + M_PI / 2) / (2.0 * M_PI));
  if (k_min_valley <= k_max_valley) min_val = -1;

  return Interval(min_val, max_val);
}

inline Interval cos(Interval x) {
  return sin(Interval(x.lo + M_PI / 2, x.hi + M_PI / 2));
}

inline Interval square(Interval x) {
  return Interval(
    x.has(0) ? 0 : std::min(x.lo * x.lo, x.hi * x.hi),
    std::max(x.lo * x.lo, x.hi * x.hi)
  );
}


#endif /* !defined(INTERVAL_INTERVAL_HPP_) */