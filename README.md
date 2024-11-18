# TechArena-Volga-Challenge-2024
Optimization Challenge Solution - Repository by team "everybody stay panic"
## General:
* [Competition website](https://techarena.unn.ru/)
* [Statements (None)]()
## Development Notes [RU]:
* **18.11.2024** Переписан merger, теперь файлы для создания submission.cpp нужно указывать в merge_list.txt. Он записывает файлы в порядке, указаном в .txt файле, удаляя #include "что-то" и #pragma once и перенося остальные #include <что-то> в верхнюю часть.
* **18.11.2024** Random shuffle: 52333.928
* **18.11.2024** Основная структура проекта готова, написан merger.
* **14.11.2024** Нужно бы также, вероятно, заранее разобраться со структурой проекта (пока нет задания это нелегко). Можно сделать условные папки `include`, `src`, `checker`, `tests`, `builder` и, вероятно, добавить CMake файлы для удобства.
* **14.11.2024** Нужно написать скрипт, который объединяет несколько .h и .cpp файлов в один .cpp файл. Желательно, чтобы можно было указать папки, откуда брать файлы.
  
  **UPD:** Был найден [репозиторий](https://github.com/Straple/Techarena2024/tree/main) одной из команд с MSU TechArena 2024, там есть пример необходимого скрипта. Нужно сделать что-то похожее.
