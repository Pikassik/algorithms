#include <iostream>
#include <vector>
#include <array>
#include <cstdio>
#include <algorithm>
#include <tuple>
#include <math.h>

constexpr double kEpsilon = 1e-3;

struct Point {
  std::array<double, 2> array;
  Point() = default;
  const double& operator[](size_t i) const;
  double& operator[](size_t i);
  explicit Point(double x, double y);
  Point operator-() const;
  Point operator+(const Point& other) const;
  Point operator-(const Point& other) const;
};

double PolarAngle(const Point& dot);

class IntersectingFigureDetecter {
 public:
  void SetFigures(const std::vector<Point>& first_figure,
                  const std::vector<Point>& second_figure);
  bool AreIntersecting();
 private:
  void InitFigure(const std::vector<Point>& figure_in, std::vector<Point>& figure);
  void InitMinkovskySum();
  void BuildMinkovskySum();
  void ProcessTail();
  bool ContainsZero();

  std::vector<Point> first_figure;
  std::vector<Point> second_figure;
  std::vector<Point> minkovsky_sum;
  size_t first_it = 1;
  size_t second_it = 1;
};

///////////////////////////////////////////////////////////////////////////////

int main() {
  auto read_figure = [](std::vector<Point>& figure) {
    size_t figure_size;
    std::cin >> figure_size;
    figure.resize(figure_size);
    for (auto& dot: figure) {
      std::cin >> dot[0] >> dot[1];
    }
  };

  std::vector<Point> first_figure;
  read_figure(first_figure);
  std::vector<Point> second_figure;
  read_figure(second_figure);

  IntersectingFigureDetecter detecter;
  detecter.SetFigures(first_figure, second_figure);
  std::cout <<
    (detecter.AreIntersecting() ? "YES" : "NO");
}

///////////////////////////////////////////////////////////////////////////////

const double& Point::operator[](size_t i) const {
  return  array[i];
}

double& Point::operator[](size_t i) {
  return  array[i];
}

Point::Point(double x, double y)
: array{{x, y}} {}

Point Point::operator-() const {
  return Point(-array[0], -array[1]);
}

Point Point::operator+(const Point& other) const {
  return Point(array[0] + other[0], array[1] + other[1]);
}

Point Point::operator-(const Point& other) const {
  return Point(array[0] - other[0], array[1] - other[1]);
}

double PolarAngle(const Point& dot) {
  if (dot[0] == 0 && dot[1] == 0)
    return 0.;
  if (dot[1] >= 0) {
    return acos(dot[0] / sqrt(dot[0] * dot[0] + dot[1] * dot[1]));
  }
  return 2 * M_PI - abs(acos(dot[0] / sqrt(dot[0] * dot[0] + dot[1] * dot[1])));
}

void IntersectingFigureDetecter::
SetFigures(const std::vector<Point>& first_figure_in,
           const std::vector<Point>& second_figure_in) {
  first_figure.clear();
  second_figure.clear();
  minkovsky_sum.clear();
  first_it = 1;
  second_it = 1;

  std::vector<Point> inversed_first_figure_in;
  inversed_first_figure_in.reserve(first_figure_in.size());
  for (auto& x: first_figure_in) {
    inversed_first_figure_in.push_back(-x);
  }

  first_figure.resize(inversed_first_figure_in.size());
  InitFigure(inversed_first_figure_in, first_figure);
  second_figure.resize(second_figure_in.size());
  InitFigure(second_figure_in, second_figure);
}

bool IntersectingFigureDetecter::
AreIntersecting() {
  InitMinkovskySum();

  BuildMinkovskySum();

  ProcessTail();

  return ContainsZero();
}

void IntersectingFigureDetecter::InitFigure(const std::vector<Point>& figure_in,
                                            std::vector<Point>& figure) {
  std::copy(figure_in.crbegin(), figure_in.crend(), figure.begin());
  auto min_it = std::min_element(figure.begin(), figure.end(),
                                 [](const Point& lhvalue, const Point& rhvalue) {
    return std::tie(lhvalue.array[1], lhvalue.array[0]) <
           std::tie(rhvalue.array[1], rhvalue.array[0]);
  });
  std::vector<Point> tmp_figure(figure);
  for (size_t i = 0; i < figure.size(); ++i) {
    figure[i] =
      tmp_figure[(i + std::distance(figure.begin(), min_it)) % figure.size()];
  }
  figure.push_back(figure[0]);
}

void IntersectingFigureDetecter::InitMinkovskySum() {
  minkovsky_sum.reserve(first_figure.size() + second_figure.size());
  minkovsky_sum.push_back(first_figure[0] + second_figure[0]);

  Point first_edge = first_figure[1] - first_figure[0];
  Point second_edge = second_figure[1] - second_figure[0];
  minkovsky_sum.push_back(PolarAngle(first_edge) <
                PolarAngle(second_edge) ?
                minkovsky_sum[0] + first_edge  : minkovsky_sum[0] + second_edge);

  if (PolarAngle(first_figure[1] - first_figure[0]) <
      PolarAngle(second_figure[1] - second_figure[0])) {
    first_it = 2;
  } else {
    second_it = 2;
  }
}

void IntersectingFigureDetecter::BuildMinkovskySum() {
  while (first_it < first_figure.size() && second_it < second_figure.size()) {
    Point first_edge =
      first_figure[first_it] - first_figure[first_it - 1];
    Point second_edge =
      second_figure[second_it] - second_figure[second_it - 1];
    if (PolarAngle(first_edge) < PolarAngle(second_edge)) {
      minkovsky_sum.push_back(first_edge + minkovsky_sum.back());
      ++first_it;
    } else {
      minkovsky_sum.push_back(second_edge + minkovsky_sum.back());
      ++second_it;
    }
  }
}

void IntersectingFigureDetecter::ProcessTail() {
  for (size_t i = first_it; i < first_figure.size(); ++i) {
    minkovsky_sum.push_back(first_figure[i] -
                            first_figure[i - 1] +
                            minkovsky_sum.back());
  }

  for (size_t i = second_it; i < second_figure.size(); ++i) {
    minkovsky_sum.push_back(second_figure[i] -
                            second_figure[i - 1] +
                            minkovsky_sum.back());
  }
}

bool IntersectingFigureDetecter::ContainsZero() {
  for (size_t i = 0; i < minkovsky_sum.size() - 1; ++i) {
    Point to_zero = Point(0, 0) - minkovsky_sum[i];
    Point vector = minkovsky_sum[i + 1] - minkovsky_sum[i];
    if (vector[0] * to_zero[1] - vector[1] * to_zero[0] < -kEpsilon) {
      return false;
    }
  }

  return true;
}