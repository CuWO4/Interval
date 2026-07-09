#include "expression.hpp"
#include "pbm.hpp"

#include <iostream>
#include <stack>
#include <utility>

class Range {
public:
  class iterator {
    int cur, stop;
  public:
    iterator(int cur, int stop) : cur(cur), stop(stop) {}
    int operator*() const { return cur; }
    iterator& operator++() { cur++; return *this; }
    iterator operator++(int) { cur++; return *this; }
    bool operator!=(const iterator& other) const {
      return cur < other.cur;
    }
  };

  Range(int stop) : start(0), stop(stop) {}
  Range(int start, int stop) : start(start), stop(stop) {}

  iterator begin() const { return iterator(start, stop); }
  iterator end()   const { return iterator(stop, stop); }

  int start, stop;

  int len() { return stop - start; }
};

enum RenderType { EqZero, GeZero };

template<std::size_t H, std::size_t W>
void render(std::array<std::array<bool, W>, H>& canvas, std::shared_ptr<Expression> e, Interval x, Interval y, RenderType type) {
  canvas = {};

  std::stack<std::tuple<Interval, Range, Interval, Range>> stack;
  stack.push(std::make_tuple(x, Range(W), y, Range(H)));
  while (!stack.empty()) {
    auto [x, canvas_x, y, canvas_y] = stack.top();
    stack.pop();

    if (canvas_x.len() <= 0 || canvas_y.len() <= 0) continue;

    auto res = e->eval(x, y);

    if (type == GeZero) {
      if (res.lo > 0) {
        for (int i : canvas_x) for (int j : canvas_y) canvas[j][i] = true;
      } else if (res.hi > 0) {
        if (canvas_x.len() == 1 && canvas_y.len() == 1) canvas[canvas_y.start][canvas_x.start] = true;
        else {
          double x_mid = (x.lo + x.hi) / 2;
          double y_mid = (y.lo + y.hi) / 2;
          int canvas_x_mid = (canvas_x.start + canvas_x.stop) / 2;
          int canvas_y_mid = (canvas_y.start + canvas_y.stop) / 2;
          stack.push(std::make_tuple(
            Interval(x.lo, x_mid), Range(canvas_x.start, canvas_x_mid),
            Interval(y.lo, y_mid), Range(canvas_y.start, canvas_y_mid)));
          stack.push(std::make_tuple(
            Interval(x_mid, x.hi), Range(canvas_x_mid, canvas_x.stop),
            Interval(y.lo, y_mid), Range(canvas_y.start, canvas_y_mid)));
          stack.push(std::make_tuple(
            Interval(x.lo, x_mid), Range(canvas_x.start, canvas_x_mid),
            Interval(y_mid, y.hi), Range(canvas_y_mid, canvas_y.stop)));
          stack.push(std::make_tuple(
            Interval(x_mid, x.hi), Range(canvas_x_mid, canvas_x.stop),
            Interval(y_mid, y.hi), Range(canvas_y_mid, canvas_y.stop)));
        }
      }
    }
    else /* type == EqZero */ {
      if (canvas_x.len() == 1 && canvas_y.len() == 1) canvas[canvas_y.start][canvas_x.start] = res.has(0);
      else if (res.lo > 0 || res.hi < 0) continue;
      else {
        double x_mid = (x.lo + x.hi) / 2;
        double y_mid = (y.lo + y.hi) / 2;
        int canvas_x_mid = (canvas_x.start + canvas_x.stop) / 2;
        int canvas_y_mid = (canvas_y.start + canvas_y.stop) / 2;
        stack.push(std::make_tuple(
          Interval(x.lo, x_mid), Range(canvas_x.start, canvas_x_mid),
          Interval(y.lo, y_mid), Range(canvas_y.start, canvas_y_mid)));
        stack.push(std::make_tuple(
          Interval(x_mid, x.hi), Range(canvas_x_mid, canvas_x.stop),
          Interval(y.lo, y_mid), Range(canvas_y.start, canvas_y_mid)));
        stack.push(std::make_tuple(
          Interval(x.lo, x_mid), Range(canvas_x.start, canvas_x_mid),
          Interval(y_mid, y.hi), Range(canvas_y_mid, canvas_y.stop)));
        stack.push(std::make_tuple(
          Interval(x_mid, x.hi), Range(canvas_x_mid, canvas_x.stop),
          Interval(y_mid, y.hi), Range(canvas_y_mid, canvas_y.stop)));
      }
    }
  }
}

int main() {
  static std::array<std::array<bool, 1000>, 1000> canvas;
  auto expr = (var_y - Const::make(5))
  * cos(Const::make(4) * sqrt(square(var_x - Const::make(4)) + square(var_y)))
  - var_x * sin(Const::make(2) * sqrt(square(var_x) + square(var_y)));
  render(canvas, expr, Interval(-20, 20), Interval(-20, 20), GeZero);
  save(canvas, "ge0_1.pbm");
  render(canvas, expr, Interval(-20, 20), Interval(-20, 20), EqZero);
  save(canvas, "eq0_1.pbm");
  expr = sin(square(var_x) + square(var_y)) - cos(var_x * var_y);
  render(canvas, expr, Interval(-20, 20), Interval(-20, 20), GeZero);
  save(canvas, "ge0_2.pbm");
  render(canvas, expr, Interval(-20, 20), Interval(-20, 20), EqZero);
  save(canvas, "eq0_2.pbm");
}