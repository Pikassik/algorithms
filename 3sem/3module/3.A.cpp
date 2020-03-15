#include <iostream>
#include <iomanip>
#include <array>
#include <functional>
#include <cmath>

const double kPrecision = 1e-10;

template <size_t n>
using Dot = std::array<double, n>;

template <size_t n>
struct Segment {
  Dot<n> begin = {0.};
  Dot<n> end = {0.};
};

template <class Function>
double TernaryMinSearch(double left_bound,
                        double right_bound,
                        double precision,
                        Function func);

template <size_t n>
double ParameterizedDistance(const Segment<n>& first,
                             const Segment<n>& second,
                             double first_parameter,
                             double second_parameter);

template <size_t n>
double SegmentDistance(const Segment<n>& first,
                       const Segment<n>& second);

///////////////////////////////////////////////////////////////////////////////

int main() {
  Segment<3> first;
  Segment<3> second;
  std::cin >> first.begin[0]  >> first.begin[1]  >> first.begin[2]
           >> first.end[0]    >> first.end[1]    >> first.end[2]
           >> second.begin[0] >> second.begin[1] >> second.begin[2]
           >> second.end[0]   >> second.end[1]   >> second.end[2];

  std::cout << std::setprecision(10)
            << SegmentDistance(first, second);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

template <class Function>
double TernaryMinSearch(double left_bound,
                        double right_bound,
                        double precision,
                        Function func) {
  while (right_bound - left_bound > precision) {
    double left_third = left_bound + (right_bound - left_bound) / 3;
    double right_third = left_bound + (right_bound - left_bound) * 2 / 3;
    if (func(right_third) > func(left_third)) {
      right_bound = right_third;
    } else {
      left_bound = left_third;
    }
  }
  return left_bound + (right_bound - left_bound) / 2;
}


/*!
 * @brief calculates distance between points
 	first.begin + (first.end - first.begin) * first_parameter
 	and
 	second.begin + (second.end - first.end) * second_parameter
 */
template <size_t n>
double ParameterizedDistance(const Segment<n>& first,
                             const Segment<n>& second,
                             double first_parameter,
                             double second_parameter) {
  double result = 0;
  for (size_t i = 0; i < n; ++i) {
    double first_dot_i =
      first.begin[i] + (first.end[i] - first.begin[i]) * first_parameter;
    double second_dot_i =
      second.begin[i] + (second.end[i] - second.begin[i]) * second_parameter;
    result += (second_dot_i - first_dot_i) * (second_dot_i - first_dot_i);
  }
  return std::sqrt(result);
}

template <size_t n>
double SegmentDistance(const Segment<n>& first,
                       const Segment<n>& second) {

  auto dot_to_dot_distance = [&first, &second](double parameter) {
    return std::bind(ParameterizedDistance<n>,
                     first,
                     second,
                     parameter,
                     std::placeholders::_1);
  };

  auto dot_to_segment_distance = [&](double parameter) {
    return
      ParameterizedDistance(first, second, parameter,
                            TernaryMinSearch(0,
                                             1,
                                             kPrecision,
                                             dot_to_dot_distance(parameter)));
  };

  double first_p = TernaryMinSearch(0, 1, kPrecision, dot_to_segment_distance);
  return
    ParameterizedDistance(first, second, first_p,
                          TernaryMinSearch(0,
                                           1,
                                           kPrecision,
                                           dot_to_dot_distance(first_p)));
}