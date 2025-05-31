import matplotlib.pyplot as plt
import numpy as np

def mechanical_power(T, rpm):
	omega = 2 * np.pi * rpm / 60
	return T * omega

rpm12 = [1355, 1340, 1320, 1275, 1265, 1235, 1225]
T12 = [0, 1, 2.3, 3.8, 5.2, 6.7, 8.0]
P12 = [0, 0, 0, 0, 0, 0, 0]

rpm21 = [1840, 1685, 1575, 1464, 1375, 1325, 1280, 1205]
T21 = [2.4, 3.1, 3.9, 4.5, 5.1, 6.0, 7.1, 8]
P21 = [0, 0, 0, 0, 0, 0, 0, 0]



for i in range(len(rpm12)):
	P12[i] = mechanical_power(T12[i], rpm12[i])

for i in range(len(rpm21)):
	P21[i] = mechanical_power(T21[i], rpm21[i])

# Create the plot
plt.plot(P12, rpm12, label='1.2')
plt.plot(P21, rpm21, label='2.1')
plt.title('n = f(P_out)')
plt.xlabel('P_out')
plt.ylabel('n (rpm)')
plt.legend()
plt.grid(True)

# Display the plot
plt.savefig("th-p.svg", dpi=300, bbox_inches='tight', transparent=False)
plt.clf()

# Create the plot
plt.plot(rpm12, T12, label='1.2')
plt.plot(rpm21, T21, label='2.1')
plt.title('T = f(n)')
plt.xlabel('n (rpm)')
plt.ylabel('T')
plt.legend()
plt.grid(True)

# Display the plot
plt.savefig("tt-n.svg", dpi=300, bbox_inches='tight', transparent=False)