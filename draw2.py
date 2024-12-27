import matplotlib.pyplot as plt
import sys

# Get the filenames from command-line arguments
file_name_1 = sys.argv[1]
file_name_2 = sys.argv[2]

# Lists to store the x and y values for both files
x_list_1 = []
y_list_1 = []
x_list_2 = []
y_list_2 = []

# Read the first file
with open(f"{file_name_1}.txt") as values_1:
    for line in values_1:
        x, y = map(float, line.split())
        x_list_1.append(x)
        y_list_1.append(y)

# Read the second file
with open(f"{file_name_2}.txt") as values_2:
    for line in values_2:
        x, y = map(float, line.split())
        x_list_2.append(x)
        y_list_2.append(y)

# Plot both files on the same graph with different colors
plt.plot(x_list_1, y_list_1, label=file_name_1, color='b')  # Blue for the first file
plt.plot(x_list_2, y_list_2, label=file_name_2, color='r')  # Red for the second file

# Labeling the axes and the graph
plt.xlabel("Tempo (segundos)")
plt.ylabel("Custo")
plt.grid(True)

# Add a legend to differentiate the files
plt.legend()

# Show the plot
plt.show()
