/*!
 * Даны n точек в пространстве. Никакие 4 точки не лежат в одной плоскости.
 * Найдите выпуклую оболочку этих точек за O(n^2).
 */
#include <array>
#include <vector>
#include <math.h>
#include <tuple>
#include <algorithm>
#include <map>
#include <iostream>
#include <stack>


struct Point {
  std::array<double, 3> array;
  Point() = default;
  Point(const Point&) = default;
  Point& operator=(const Point&) = default;
  bool operator==(const Point& other) const;
  const double& operator[](size_t i) const;
  double& operator[](size_t i);
  explicit Point(double x, double y, double z);
  Point operator-() const;
  Point operator+(const Point& other) const;
  Point operator-(const Point& other) const;
  Point operator*(double x) const;

  static double Distance(const Point& first, const Point& second);
  static double  Length(const Point& dot);
  static double PointProduct(const Point& first, const Point& second);
  static double Angle(const Point& first, const Point& second);
  static Point CrossProduct(const Point& first, const Point& second);
  static double Cos(const Point& first, const Point& second);
};

Point NormalVector(const Point& first_dot,
                 const Point& second_dot,
                 const Point& third_dot);

struct Segment {
  size_t begin;
  size_t end;
};

bool operator==(const Segment& left, const Segment& right);
bool operator<(const Segment& left, const Segment& right);

Segment OrderedSegment(size_t begin, size_t end);

class LowerSegmentCmp {
 public:
  explicit LowerSegmentCmp(const Point& lowest_dot);
  bool operator()(const Point& lhvalue,
                  const Point& rhvalue) const;
 private:
  Point lowest_dot_;
};

class LowerFaceCmp {
 public:
  explicit LowerFaceCmp(const Point& first_dot,
                        const Point& second_dot);
  bool operator()(const Point& lhvalue,
                  const Point& rhvalue) const;
 private:
  Point first_dot_;
  Point second_dot_;
};

void InitBeforeHullBuilding(const std::vector<std::array<size_t, 3>>& faces,
                            std::stack<std::pair<Segment, size_t>>& edges_stack,
                            std::map<Segment, size_t>& edges_count);

bool IsFreeEdge(const std::map<Segment, size_t>& edges_count,
                size_t left, size_t right);

double CurrentFacesAngle(const std::vector<Point>& dots,
                         Segment current_edge,
                         size_t current_point,
                         size_t current_index);

void UpdateHullBuildingState(std::vector<std::array<size_t, 3>>& faces,
                             std::stack<std::pair<Segment, size_t>>& edges_stack,
                             std::map<Segment, size_t>& edges_count,
                             Segment current_edge,
                             size_t current_vertex);

void BuildHull(const std::vector<Point>& dots,
               std::vector<std::array<size_t, 3>>& faces,
               std::stack<std::pair<Segment, size_t>>& edges_stack,
               std::map<Segment, size_t>& edges_count);

void RotateFaces(const std::vector<Point>& dots,
                 std::vector<std::array<size_t, 3>>& faces);

std::vector<std::array<size_t, 3>> ConvexHull(const std::vector<Point>& dots);

///////////////////////////////////////////////////////////////////////////////

int main() {
  size_t tests_count = 0;
  std::cin >> tests_count;
  std::vector<Point> dots;
  for (size_t i = 0; i < tests_count; ++i) {
    size_t vertices_count = 0;
    std::cin >> vertices_count;
    dots.resize(vertices_count);
    for (auto& dot: dots) {
      std::cin >> dot[0] >> dot[1] >> dot[2];
    }
    auto faces = ConvexHull(dots);
    std::cout << faces.size() << std::endl;
    for (const auto& face: faces) {
      std::cout << '3'     << ' '
                << face[0] << ' '
                << face[1] << ' ' <<
                   face[2] << std::endl;
    }
    dots.clear();
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

bool Point::operator==(const Point& other) const {
  return array == other.array;
}

const double& Point::operator[](size_t i) const {
  return  array[i];
}

double& Point::operator[](size_t i) {
  return  array[i];
}

Point::Point(double x, double y, double z): array{{x, y, z}} {}

Point Point::operator-() const {
  return Point(-array[0], -array[1], -array[2]);
}

Point Point::operator+(const Point& other) const {
  return Point(array[0] + other[0], array[1] + other[1], array[2] + other[2]);
}

Point Point::operator-(const Point& other) const {
  return Point(array[0] - other[0], array[1] - other[1], array[2] - other[2]);
}

Point Point::operator*(double x) const {
  return Point(array[0] * x, array[1] * x, array[2] * x);
}

double Point::Distance(const Point& first, const Point& second) {
  return std::sqrt((second[0] - first[0]) * (second[0] - first[0]) +
                   (second[1] - first[1]) * (second[1] - first[1]) +
                   (second[2] - first[2]) * (second[2] - first[2]));
}

double Point::Length(const Point& dot) {
  return Distance(Point(0, 0, 0), dot);
}

double Point::PointProduct(const Point& first, const Point& second) {
  return first[0] * second[0] + first[1] * second[1] + first[2] * second[2];
}

double Point::Angle(const Point& first, const Point& second) {
  return acos(
    PointProduct(first, second)  / (Length(first) * Length(second))
  );
}

Point Point::CrossProduct(const Point& first, const Point& second) {
  return Point(first[1] * second[2] - first[2] * second[1],
             -(first[0] * second[2] - first[2] * second[0]),
             first[0] * second[1] - first[1] * second[0]
  );
}

double Point::Cos(const Point& first, const Point& second) {
  return PointProduct(first, second) / (Length(first) * Length(second));
}

Point NormalVector(const Point& first_dot,
                   const Point& second_dot,
                   const Point& third_dot) {
  return Point::CrossProduct(first_dot - second_dot, third_dot - second_dot);
}

bool operator==(const Segment& left, const Segment& right) {
  return left.begin == right.begin && left.end == right.end;
}

bool operator<(const Segment& left, const Segment& right) {
  return std::tie(left.begin, left.end) < std::tie(right.begin, right.end);
}

Segment OrderedSegment(size_t begin, size_t end) {
  if (begin < end)
    return {begin, end};
  return {end, begin};
}

LowerSegmentCmp::LowerSegmentCmp(const Point& lowest_dot)
: lowest_dot_(lowest_dot) {}

bool LowerSegmentCmp::operator()(const Point& lhvalue, const Point& rhvalue) const {
  if (lhvalue == lowest_dot_)
      return true;

  if (rhvalue == lowest_dot_)
    return false;

  return Point::Angle(lhvalue - lowest_dot_, Point(0, 0, 1)) <
         Point::Angle(rhvalue - lowest_dot_, Point(0, 0, 1));
}

LowerFaceCmp::LowerFaceCmp(const Point& first_dot, const Point& second_dot)
: first_dot_(first_dot)
, second_dot_(second_dot) {}

bool LowerFaceCmp::operator()(const Point& lhvalue, const Point& rhvalue) const {
  auto normalized_applicate = [](const Point& dot) -> double {
    return std::abs(dot[2]) / Point::Length(dot);
  };

  if (lhvalue == first_dot_ ||
      lhvalue == second_dot_)
      return true;

    if (rhvalue == first_dot_ ||
        rhvalue == second_dot_)
      return false;

    Point left_normal_vector = NormalVector(first_dot_, second_dot_, lhvalue);
    Point right_normal_vector = NormalVector(first_dot_, second_dot_, rhvalue);

    return  normalized_applicate(left_normal_vector) <
            normalized_applicate(right_normal_vector);
}

void InitBeforeHullBuilding(const std::vector<std::array<size_t, 3>>& faces,
                            std::stack<std::pair<Segment, size_t>>& edges_stack,
                            std::map<Segment, size_t>& edges_count) {
  edges_stack.emplace(OrderedSegment(faces[0][1], faces[0][2]),
                      faces[0][0]);
  edges_stack.emplace(OrderedSegment(faces[0][0], faces[0][2]),
                      faces[0][1]);
  edges_stack.emplace(OrderedSegment(faces[0][0], faces[0][1]),
                      faces[0][2]);

  edges_count[OrderedSegment(faces[0][2], faces[0][1])] = 1;
  edges_count[OrderedSegment(faces[0][0], faces[0][2])] = 1;
  edges_count[OrderedSegment(faces[0][0], faces[0][1])] = 1;
}

bool IsFreeEdge(const std::map<Segment, size_t>& edges_count,
                size_t left, size_t right) {
  auto it = edges_count.find(OrderedSegment(left, right));
  if (it == edges_count.end())
    return true;
  return (*it).second < 2;
}

double CurrentFacesAngle(const std::vector<Point>& dots,
                         Segment current_edge,
                         size_t current_point,
                         size_t current_index) {
  Point first_middle = (dots[current_edge.begin] +
                      dots[current_edge.end] +
                      dots[current_index]) * (1./3);
      Point second_middle = (dots[current_edge.begin] +
                           dots[current_edge.end] +
                           dots[current_point]) * (1./3);

      std::array<size_t, 3> new_face =
        {current_edge.begin, current_edge.end, current_index};
      Point first_normal = NormalVector(dots[current_point],
                                      dots[current_edge.begin],
                                      dots[current_edge.end]);

      if (Point::Cos(first_normal, first_middle - second_middle) < -0)
        first_normal = -first_normal;

      Point second_normal = NormalVector(dots[new_face[0]],
                                       dots[new_face[1]],
                                       dots[new_face[2]]);
      if (Point::Cos(second_normal, second_middle - first_middle) < -0)
        second_normal = -second_normal;

  return std::acos(-Point::Cos(first_normal, second_normal));
}

void UpdateHullBuildingState(std::vector<std::array<size_t, 3>>& faces,
                             std::stack<std::pair<Segment, size_t>>& edges_stack,
                             std::map<Segment, size_t>& edges_count,
                             Segment current_edge,
                             size_t current_vertex) {
  ++edges_count[OrderedSegment(current_edge.begin, current_edge.end)];
  ++edges_count[OrderedSegment(current_vertex, current_edge.begin)];
  ++edges_count[OrderedSegment(current_edge.end, current_vertex)];

  faces.push_back({current_edge.begin, current_edge.end, current_vertex});

  edges_stack.emplace(OrderedSegment(current_vertex, current_edge.end),
                      current_edge.begin);
  edges_stack.emplace(OrderedSegment(current_vertex, current_edge.begin),
                      current_edge.end);
}

void BuildHull(const std::vector<Point>& dots,
               std::vector<std::array<size_t, 3>>& faces,
               std::stack<std::pair<Segment, size_t>>& edges_stack,
               std::map<Segment, size_t>& edges_count) {
  while (!edges_stack.empty()) {
    double current_angle = 0;
    size_t current_vertex = 0;

    Segment current_edge = edges_stack.top().first;
    size_t current_point = edges_stack.top().second;
    edges_stack.pop();

    if (!IsFreeEdge(edges_count, current_edge.begin, current_edge.end))
      continue;

    for (size_t i = 0; i < dots.size(); ++i) {
      if (!IsFreeEdge(edges_count, i, current_edge.begin) ||
          !IsFreeEdge(edges_count, i, current_edge.end))
        continue;

      if (i == current_point ||
          i == current_edge.begin ||
          i == current_edge.end)
        continue;

      double angle = CurrentFacesAngle(dots, current_edge, current_point, i);
      if (angle > current_angle) {
        current_vertex = i;
        current_angle = angle;
      }
    }

    UpdateHullBuildingState(faces,
                            edges_stack,
                            edges_count,
                            current_edge,
                            current_vertex);
  }
}

void RotateFaces(const std::vector<Point>& dots,
                 std::vector<std::array<size_t, 3>>& faces) {
  for (size_t i = 0; i < faces.size(); ++i) {
    std::sort(faces[i].begin(), faces[i].end());
    Point first_middle =
      (dots[faces[i][0]] +
       dots[faces[i][1]] +
       dots[faces[i][2]]) * (1./3);
    Point second_middle =
      (dots[faces[(i + 1) % faces.size()][0]] +
       dots[faces[(i + 1) % faces.size()][1]] +
       dots[faces[(i + 1) % faces.size()][2]]) * (1./3);

    if (Point::Cos(second_middle - first_middle,
                 Point::CrossProduct(
                   dots[faces[i][1]] - dots[faces[i][0]],
                   dots[faces[i][2]] - dots[faces[i][0]])) < -0) {
      std::swap(faces[i][1], faces[i][2]);
    }
    std::swap(faces[i][1], faces[i][2]);
  }
}

std::vector<std::array<size_t, 3>> ConvexHull(const std::vector<Point>& dots) {
  Segment first_segment;
  first_segment.begin = std::distance(dots.begin(),
    std::min_element(dots.begin(), dots.end(),
                     [&dots](const Point& lhvalue, const Point& rhvalue) {
    return std::tie(lhvalue[2], lhvalue[1], lhvalue[0]) <
           std::tie(rhvalue[2], rhvalue[1], rhvalue[0]);
  }));

  LowerSegmentCmp lower_segment_cmp(dots[first_segment.begin]);
  first_segment.end = std::distance(dots.begin(),
                                std::max_element(dots.begin(),  dots.end(),
                                                 lower_segment_cmp));

  LowerFaceCmp lower_face_cmp(dots[first_segment.begin],
                              dots[first_segment.end]);
  size_t third_dot = std::distance(dots.begin(),
                                   std::max_element(dots.begin(), dots.end(),
                                                    lower_face_cmp));

  std::vector<std::array<size_t, 3>> faces;
  faces.push_back({first_segment.begin, first_segment.end, third_dot});
  std::stack<std::pair<Segment, size_t>> edges_stack;
  std::map<Segment, size_t> edges_count;
  InitBeforeHullBuilding(faces, edges_stack, edges_count);
  BuildHull(dots, faces, edges_stack, edges_count);

  RotateFaces(dots, faces);
  std::sort(faces.begin(), faces.end());

  return faces;
}