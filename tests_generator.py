import os

num_of_files = 200
folder_name = 'test_samples'


class RandomGenerator:
    def __init__(self, seed, a=1664524365, c=10139042347423, m=2 ** 64 - 1):
        self.seed = seed
        self.a = a
        self.c = c
        self.m = m
        self.state = seed

    def next(self):
        self.state = (self.a * self.state + self.c) % self.m
        return self.state

    def randint(self, l, r):
        return l + self.next() % (r - l + 1)

    def random(self):
        return self.next() / self.m


def main():
    generator = RandomGenerator(seed=42)

    if not os.path.exists(folder_name):
        os.makedirs(folder_name)

    for i in range(1, num_of_files + 1):
        file_name = os.path.join(folder_name, f"{i}.txt")

        n = generator.randint(1, 200)
        m = generator.randint(1, 200)

        M = generator.randint(0, int(1e16))
        F = generator.randint(0, 2)

        data = [generator.randint(1, int(1e9)) for i in range(n)]
        operations = [[generator.randint(1, n), generator.randint(1, n), generator.random()] for i in range(m)]

        with open(file_name, 'w') as file:
            file.write(f"{n} {m}\n")
            file.write(f"{M} {F}\n")
            file.write(' '.join(map(str, data)) + '\n')
            for operation in operations:
                file.write(f"{operation[0]} {operation[1]} {operation[2]}\n")


if __name__ == "__main__":
    main()
