#ifndef INTERVAL_EXPRESSION_HPP_
#define INTERVAL_EXPRESSION_HPP_

#include "interval.hpp"

#include <variant>
#include <memory>

struct Expression {
  virtual Interval eval(Interval x, Interval y) = 0;
  virtual ~Expression() = default;
};

struct Var : Expression {
  char name;
  Var(char name) : name(name) {}
  Interval eval(Interval x, Interval y) {
    return name == 'x' ? x
      : name == 'y' ? y
      : (assert(false && "not supported variable"), Interval(0, 0))
    ;
  }
  static std::shared_ptr<Var> make(char name) { return std::make_shared<Var>(name); }
};

inline auto var_x = Var::make('x');
inline auto var_y = Var::make('y');

struct Const : Expression {
  double val;
  Const(double val) : val(val) {}
  Interval eval(Interval x, Interval y) { return Interval(val, val); }
  static std::shared_ptr<Const> make(double val) { return std::make_shared<Const>(val); }
};

struct Unary : Expression {
  enum UnaryType { Sqrt, Sin, Cos, Square } type;
  std::shared_ptr<Expression> sub;
  Unary(UnaryType type, std::shared_ptr<Expression> sub) : type(type), sub(sub) {}
  Interval eval(Interval x, Interval y) {
    switch (type) {
      case Sqrt: {
        return sqrt(sub->eval(x, y));
      } break;
      case Sin: {
        return sin(sub->eval(x, y));
      } break;
      case Cos: {
        return cos(sub->eval(x, y));
      } break;
      case Square: {
        return square(sub->eval(x, y));
      } break;
    }
    __builtin_unreachable();
  }
};

struct Binary : Expression {
  enum BinaryType { Add, Sub, Mul } type;
  std::shared_ptr<Expression> lc, rc;
  Binary(BinaryType type, std::shared_ptr<Expression> lc, std::shared_ptr<Expression> rc)
    : type(type), lc(lc), rc(rc) {}
  Interval eval(Interval x, Interval y) {
    switch (type) {
      case Add: {
        return lc->eval(x, y) + rc->eval(x, y);
      } break;
      case Sub: {
        return lc->eval(x, y) - rc->eval(x, y);
      } break;
      case Mul: {
        return lc->eval(x, y) * rc->eval(x, y);
      } break;
    }
    __builtin_unreachable();
  }
};

inline std::shared_ptr<Expression> operator+(std::shared_ptr<Expression> lc, std::shared_ptr<Expression> rc) {
  return std::make_shared<Binary>(Binary::Add, lc, rc);
}
inline std::shared_ptr<Expression> operator-(std::shared_ptr<Expression> lc, std::shared_ptr<Expression> rc) {
  return std::make_shared<Binary>(Binary::Sub, lc, rc);
}
inline std::shared_ptr<Expression> operator*(std::shared_ptr<Expression> lc, std::shared_ptr<Expression> rc) {
  return std::make_shared<Binary>(Binary::Mul, lc, rc);
}
inline std::shared_ptr<Expression> sqrt(std::shared_ptr<Expression> sub) {
  return std::make_shared<Unary>(Unary::Sqrt, sub);
}
inline std::shared_ptr<Expression> sin(std::shared_ptr<Expression> sub) {
  return std::make_shared<Unary>(Unary::Sin, sub);
}
inline std::shared_ptr<Expression> cos(std::shared_ptr<Expression> sub) {
  return std::make_shared<Unary>(Unary::Cos, sub);
}
inline std::shared_ptr<Expression> square(std::shared_ptr<Expression> sub) {
  return std::make_shared<Unary>(Unary::Square, sub);
}

#endif /* !defined(INTERVAL_EXPRESSION_HPP_) */