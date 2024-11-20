import os

merge_list_file = "merge_list.txt"
output_file = "submission.cpp"

def find_file(file_name, search_path):
    for root, dirs, files in os.walk(search_path):
        if file_name in files:
            return os.path.join(root, file_name)
    return None


def merge_files(list_of_files_to_merge, output_path):

    includes = set()
    content_to_merge = []

    for file in list_of_files_to_merge:
        if file is None:
            continue

        with open(file, 'r') as input_file_obj:
            for line in input_file_obj.read().split('\n'):

                if line.startswith("#include <"):
                    includes.add(line)

                elif line.startswith("#include \""):
                    continue

                elif line == "#pragma once":
                    continue

                else:
                    content_to_merge.append(line)

    with open(output_path, 'w') as output_file_obj:
        for include in sorted(includes):
            output_file_obj.write(include + '\n')

        for line in content_to_merge:
            output_file_obj.write(line)
            output_file_obj.write("\n")


def main():
    if not os.path.exists(merge_list_file):
        print(f"Файл {merge_list_file} не найден.")
        return

    with open(merge_list_file, 'r') as merge_list_file_obj:
        list_of_files_to_merge = merge_list_file_obj.read().split()

    current_dir = os.getcwd()
    file_paths = [find_file(file_name, current_dir) for file_name in list_of_files_to_merge]

    merge_files(file_paths, output_file)

if __name__ == "__main__":
    main()