#include <iostream>
#include <vector>
#include <array>
#include <cstdio>
#include <algorithm>
#include <tuple>
#include <math.h>

constexpr double kEpsilon = 1e-3;

struct Dot {
  std::array<double, 2> array;
  Dot() = default;
  const double& operator[](size_t i) const;
  double& operator[](size_t i);
  explicit Dot(double x, double y);
  Dot operator-() const;
  Dot operator+(const Dot& other) const;
  Dot operator-(const Dot& other) const;
};

double PolarAngle(const Dot& dot);

void InitFigure(const std::vector<Dot>& figure_in, std::vector<Dot>& figure);
void InitMinkovskySum(std::vector<Dot>& minkovsky_sum,
                     const std::vector<Dot>& first_figure,
                     const std::vector<Dot>& second_figure,
                     size_t& first_it,
                     size_t& second_it);
void BuildMinkovskySum(std::vector<Dot>& minkovsky_sum,
                       const std::vector<Dot>& first_figure,
                       const std::vector<Dot>& second_figure,
                       size_t& first_it,
                       size_t& second_it);
void ProcessTail(std::vector<Dot>& minkovsky_sum,
                 const std::vector<Dot>& first_figure,
                 const std::vector<Dot>& second_figure,
                 size_t& first_it,
                 size_t& second_it);
bool ContainsZero(const std::vector<Dot>& minkovsky_sum);
bool IsIntersect(const std::vector<Dot>& first_figure,
                 const std::vector<Dot>& second_figure);

///////////////////////////////////////////////////////////////////////////////

int main() {
  auto read_figure = [](std::vector<Dot>& figure) {
    size_t figure_size;
    std::cin >> figure_size;
    figure.resize(figure_size);
    for (auto& dot: figure) {
      std::cin >> dot[0] >> dot[1];
    }
  };

  std::vector<Dot> first_figure;
  read_figure(first_figure);
  std::vector<Dot> second_figure;
  read_figure(second_figure);

  std::cout << (IsIntersect(first_figure, second_figure) ? "YES" : "NO");
}

///////////////////////////////////////////////////////////////////////////////

const double& Dot::operator[](size_t i) const {
  return  array[i];
}

double& Dot::operator[](size_t i) {
  return  array[i];
}

Dot::Dot(double x, double y)
: array{{x, y}} {}

Dot Dot::operator-() const {
  return Dot(-array[0], -array[1]);
}

Dot Dot::operator+(const Dot& other) const {
  return Dot(array[0] + other[0], array[1] + other[1]);
}

Dot Dot::operator-(const Dot& other) const {
  return Dot(array[0] - other[0], array[1] - other[1]);
}

double PolarAngle(const Dot& dot) {
  if (dot[1] >= 0) {
    return acos(dot[0] / sqrt(dot[0] * dot[0] + dot[1] * dot[1]));
  }
  return 2 * M_PI - abs(acos(dot[0] / sqrt(dot[0] * dot[0] + dot[1] * dot[1])));
}

bool IsIntersect(const std::vector<Dot>& first_figure_in,
                 const std::vector<Dot>& second_figure_in) {

  std::vector<Dot> inversed_first_figure_in;
  inversed_first_figure_in.reserve(first_figure_in.size());
  for (auto& x: first_figure_in) {
    inversed_first_figure_in.push_back(-x);
  }

  std::vector<Dot> first_figure(inversed_first_figure_in.size());
  InitFigure(inversed_first_figure_in, first_figure);
  std::vector<Dot> second_figure(second_figure_in.size());
  InitFigure(second_figure_in, second_figure);

  std::vector<Dot> minkovsky_sum;
  size_t first_it = 1;
  size_t second_it = 1;
  InitMinkovskySum(minkovsky_sum,
                   first_figure,
                   second_figure,
                   first_it,
                   second_it);

  BuildMinkovskySum(minkovsky_sum,
                    first_figure,
                    second_figure,
                    first_it,
                    second_it);

  ProcessTail(minkovsky_sum,
              first_figure,
              second_figure,
              first_it,
              second_it);

  return ContainsZero(minkovsky_sum);
}

void InitFigure(const std::vector<Dot>& figure_in, std::vector<Dot>& figure) {
  std::copy(figure_in.crbegin(), figure_in.crend(), figure.begin());
  auto min_it = std::min_element(figure.begin(), figure.end(),
                                 [](const Dot& lhvalue, const Dot& rhvalue) {
    return std::tie(lhvalue.array[1], lhvalue.array[0]) <
           std::tie(rhvalue.array[1], rhvalue.array[0]);
  });
  std::vector<Dot> tmp_figure(figure);
  for (size_t i = 0; i < figure.size(); ++i) {
    figure[i] =
      tmp_figure[(i + std::distance(figure.begin(), min_it)) % figure.size()];
  }
  figure.push_back(figure[0]);
}

void InitMinkovskySum(std::vector<Dot>& minkovsky_sum,
                     const std::vector<Dot>& first_figure,
                     const std::vector<Dot>& second_figure,
                     size_t& first_it,
                     size_t& second_it) {
  minkovsky_sum.reserve(first_figure.size() + second_figure.size());
  minkovsky_sum.push_back(first_figure[0] + second_figure[0]);

  Dot first_edge = first_figure[1] - first_figure[0];
  Dot second_edge = second_figure[1] - second_figure[0];
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

void BuildMinkovskySum(std::vector<Dot>& minkovsky_sum,
                       const std::vector<Dot>& first_figure,
                       const std::vector<Dot>& second_figure,
                       size_t& first_it,
                       size_t& second_it) {
  while (first_it < first_figure.size() && second_it < second_figure.size()) {
    Dot first_edge =
      first_figure[first_it] - first_figure[first_it - 1];
    Dot second_edge =
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

void ProcessTail(std::vector<Dot>& minkovsky_sum,
                 const std::vector<Dot>& first_figure,
                 const std::vector<Dot>& second_figure,
                 size_t& first_it,
                 size_t& second_it) {
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

bool ContainsZero(const std::vector<Dot>& minkovsky_sum) {
  for (size_t i = 0; i < minkovsky_sum.size() - 1; ++i) {
    Dot to_zero = Dot(0, 0) - minkovsky_sum[i];
    Dot vector = minkovsky_sum[i + 1] - minkovsky_sum[i];
    if (vector[0] * to_zero[1] - vector[1] * to_zero[0] < -kEpsilon) {
      return false;
    }
  }

  return true;
}