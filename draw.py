import matplotlib.pyplot as plt
import sys

file_name = sys.argv[1]
x_list = []
y_list = []

with open(f"{file_name}.txt") as values:
    for line in values:
        x, y = map(float, line.split())
        x_list.append(x)
        y_list.append(y)

plt.plot(x_list, y_list)
plt.xlabel("Tempo (segundos)")
plt.ylabel("Custo")
plt.grid(True)

plt.show()