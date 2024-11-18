sources = ['include/graph.h', 'src/graph.cpp', 'src/checker.cpp', 'src/tester.cpp', 'src/main.cpp']
includes = []
other = []

for src in sources:
    with open(src, 'r') as input:
        lines = list(filter(None, input.read().split('\n')))
        for line in lines:
            if line[0] == '#' and line[1:8] == 'include':
                if line[-2] != 'h':
                    includes.append(line)
            else:
                other.append(line)
includes = sorted(set(includes))
with open('submission.cpp', 'w') as output:
    for line in includes:
        print(line, file=output, end='\n')
    for line in other:
        print(line, file=output, end='\n')
