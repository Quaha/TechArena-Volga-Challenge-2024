# TechArena-Volga-Challenge-2024
Optimization Challenge Solution - Repository by team "everybody stay panic"
## General:
* [Competition website](https://techarena.unn.ru/)
* [Standings](https://codeforces.com/spectator/ranklist/d4a9d46a4854d6ea249a4c207b20354f)
* [Testing system](https://volga2024.contest.codeforces.com/group/uJZlaL3LbH/contests)
* [Statements](Statements.png)
## Statements [RU]:
Имеется граф, состоящий из ***n*** (**1<=*n*<=200**) вершин. У каждой вершины есть вещественный вес ***с<sub>i</sub>*** (с точностью до 15 знаков после запятой, однако, изначально это целые числа, **1<=*c<sub>i</sub>*<=10<sup>9</sup>**). Также есть последовательность из ***m*** (**1<=*m*<=200**) операций следующего вида:
* ***x*** ***y*** ***s*** - создать новую вершину с весом ***c<sub>result</sub>*** = ***s*** × ***c<sub>x</sub>*** × ***c<sub>y</sub>***, затем удалить вершины ***x*** и ***y*** и начать *ссылаться* на новую вершину по именам ***x*** и ***y***
* ***x*** ***x*** ***s*** - создать новую вершину с весом ***c<sub>result</sub>*** = ***s*** × ***c<sub>x</sub>***, затем удалить вершину ***x*** и начать *ссылаться* на новую вершину по имени ***x***

(**0<=*s<sub>i</sub>*<=1**, ***s<sub>i</sub>*** - вещественное число с точностью до 15 знаков после запятой)

**Операции имеют свою стоимость:**
* Стоимость операции первого типа ***cost<sub>1</sub>*** = ***c<sub>x</sub>*** + ***c<sub>y</sub>*** + ***c<sub>result</sub>*** + ***penalty***(***c<sub>result</sub>***)
* Стоимость операции второго типа ***cost<sub>2</sub>*** = ***c<sub>x</sub>*** + ***c<sub>result</sub>*** + ***penalty***(***c<sub>result</sub>***)

Где ***penalty***(***c***) = {0, ***c*** <= ***M*** **;** (***c*** - ***M***) × ***F***, ***c*** > ***M***

(***M*** и ***F*** - целые числа, 0<=***M***<=10<sup>16</sup>, 0<=***F***<=2)

**Задача:** Переупорядочить операции так, чтобы минимизировать их стоимость.

**Оценка решения**: Пусть ***A*** - суммарная стоимость всех операций, тогда количество баллов ***P*** за тест будет вычислено как:

***P*** = ***m*** × ***sum***<sub>***i***=1..**n**</sub>(***log***(***с<sub>i</sub>***)) / ***log***(***A***)

Итоговое число баллов равно сумме баллов по всем тестам. В данный момент в систему загружено 50 тестов, после окончания соревнования будет загружено ещё 30. Оба набора тестов (50 предварительных тестов и 30 итоговых тестов) содержат примеры из реальных тестов, а также некоторые синтетические данные.

## Development Notes [RU]:
* **20.11.2024** Генетический алгоритм (те же параметры) после фикса DBL_MAX: 63736.547
* **20.11.2024** Генетический алгоритм (PMX crossover, `1000, 500, 20, 100`): 63325.121
* **20.11.2024** Добавлены решения с помощью ДП для небольших `m <= 21` и генетический алгоритм для остальных `m`.
* **18.11.2024** Добавлен генератор тестов. Количество тестов можно изменять, он генерирует всегда одни и те же тесты, т.к. сид генератора зафиксирован (но может быть изменён). При запуске он создаёт папку test_samples, куда записывает файлы вида 1.txt, 2.txt...
* **18.11.2024** Переписан merger, теперь файлы для создания submission.cpp нужно указывать в merge_list.txt. Он записывает файлы в порядке, указаном в .txt файле, удаляя #include "что-то" и #pragma once и перенося остальные #include <что-то> в верхнюю часть.
* **18.11.2024** Random shuffle: 52333.928
* **18.11.2024** Основная структура проекта готова, написан merger.
* **14.11.2024** Нужно бы также, вероятно, заранее разобраться со структурой проекта (пока нет задания это нелегко). Можно сделать условные папки `include`, `src`, `checker`, `tests`, `builder` и, вероятно, добавить CMake файлы для удобства.
* **14.11.2024** Нужно написать скрипт, который объединяет несколько .h и .cpp файлов в один .cpp файл. Желательно, чтобы можно было указать папки, откуда брать файлы.
  
  **UPD:** Был найден [репозиторий](https://github.com/Straple/Techarena2024/tree/main) одной из команд с MSU TechArena 2024, там есть пример необходимого скрипта. Нужно сделать что-то похожее.
