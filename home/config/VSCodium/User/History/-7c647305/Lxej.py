import matplotlib.pyplot as plt
import numpy as np

def mechanical_power(T, rpm):
	omega = 2 * np.pi * rpm / 60
	return T * omega

rpm = [1355, 1340, 1320, 1275, 1265, 1235, 1225]
T = [0, 1, 2.3, 3.8, 5.2, 6.7, 8.0]
P = [0, 0, 0, 0, 0, 0, 0]

for i in range(len(rpm)):
	P[i] = mechanical_power(T[i], rpm[i])

# Create the plot
plt.plot(P, rpm, label='n')
plt.title('n = f(P_out)')
plt.xlabel('P_out')
plt.ylabel('n (rpm)')
plt.legend()
plt.grid(True)

# Display the plot
plt.savefig("plot.svg", dpi=300, bbox_inches='tight', transparent=False)

