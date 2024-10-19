from matplotlib import pyplot as plt
import math


timestamps = []

with open("measurements", "r") as f:
    for not_first_line, line in enumerate(f):
        timestamp, _, _ = line.strip().split()
        timestamps.append(int(timestamp))

diffs = []
for i in range(1, len(timestamps)):
    diffs.append(timestamps[i] - timestamps[i - 1])


def variance(data: list):
    n = len(data)
    mean = sum(data) / n
    variance = sum((x - mean) ** 2 for x in data) / n
    return variance


print(math.sqrt(variance(diffs)))

plt.hist(diffs, bins=10)
plt.xlabel("micro seconds")
plt.ylabel("count")
plt.savefig("hist1.png")
plt.show()
