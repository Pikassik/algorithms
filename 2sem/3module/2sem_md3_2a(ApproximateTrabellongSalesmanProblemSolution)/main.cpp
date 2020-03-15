//Задача 2а). Приближенное решение метрической неориентированной задачи коммивояжера.
// Задачи в контесте нет.
// Найдите приближенное решение метрической неориентированной задачи
// коммивояжера в полном графе (на плоскости) с помощью минимального
// остовного дерева, построенного в первой задаче.
// Оцените качество приближения на случайном наборе точек, нормально
// распределенном на плоскости с дисперсией 1.
// Нормально распределенный набор точек
// получайте с помощью std::normal_distribution.
// При фиксированном N, количестве вершин графа, несколько раз запустите оценку
// качества приближения. Вычислите среднее значение и среднеквадратичное
// отклонение качества приближения для данного N.
// Запустите данный эксперимент для всех N в некотором диапазоне,
// например, [2, 10].
// Автоматизируйте запуск экспериментов.
// В решении требуется разумно разделить код на файлы. Каждому классу
// - свой заголовочный файл и файл с реализацией.
#include <iostream>
#include <headers/ApproximateTravellingSalesmanProblemSolution.h>
#include <headers/AccurateTravellingSalesmanProblemSolution.h>
#include <headers/GraphGenerator.h>

int main() {
  for (int vertices_count = 3; vertices_count < 11; ++vertices_count) {
    for (int i = 0; i < 2; ++i) {
      double accurate_path_length;
      double approximate_path_length;
      std::shared_ptr<IGraph<double>> graph = GraphGenerator(vertices_count);
      accurate_path_length = AccurateTravellingSalesmanProblemSolution(graph);
      approximate_path_length =
          ApproximateTravellingSalesmanProblemSolution(graph);
      std::cout << "Vertices count is:     " << vertices_count << std::endl;
      std::cout << "Accurate answer is:    " << accurate_path_length
                << std::endl;
      std::cout << "Approximate answer is: " << approximate_path_length
                << std::endl;
      std::cout << "Ratio is:              " << approximate_path_length /
          accurate_path_length << std::endl;
      std::cout
          << (approximate_path_length < accurate_path_length * 2 + 0.00001 ?
              "CORRECT" : "NOT CORRECT") << std::endl << std::endl;
    }
  }
}