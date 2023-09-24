import sys
import matplotlib.pyplot as plt
import numpy as np

# Check if the correct number of command-line arguments is provided
if len(sys.argv) != 2:
    print("Usage: python histogram.py filename.txt")
    sys.exit(1)

# Get the filename from the command line argument
filename = sys.argv[1]

# Step 1: Read data from txt file
try:
    with open(filename, 'r') as file:
        data = file.readlines()
except FileNotFoundError:
    print(f"Error: File '{filename}' not found.")
    sys.exit(1)

# Step 2: Parse data into two lists for the two columns
x_values = []
y_values = []
for line in data:
    parts = line.strip().split('\t')
    if len(parts) == 2:
        x_values.append(float(parts[0]))
        y_values.append(float(parts[1]))

# Step 3: Create a histogram
plt.hist(x_values, bins=10, color='blue', edgecolor='black')
plt.xlabel('X Values')
plt.ylabel('Frequency')
plt.title('Histogram of X Values')
plt.show()

# If you want to create a histogram for the second column (Y Values), you can do the following:
# plt.hist(y_values, bins=10, color='red', edgecolor='black')
# plt.xlabel('Y Values')
# plt.ylabel('Frequency')
# plt.title('Histogram of Y Values')
# plt.show()
