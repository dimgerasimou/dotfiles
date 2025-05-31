import matplotlib.pyplot as plt
import numpy as np

def mechanical_power(T, rpm):
	omega = 2 * np.pi * rpm / 60
	return T * omega

rpm = [1355, 1340, 1320, 1275, 1265, 1235, 1225]
T = [0, 1, 2.3, 3.8, 5.2, 6.7, 8.0]

y = np.sin(x)

# Create the plot
plt.plot(x, y, label='sin(x)')
plt.title('Sine Wave')
plt.xlabel('x')
plt.ylabel('sin(x)')
plt.legend()
plt.grid(True)

# Display the plot
plt.savefig("plot.svg", dpi=300, bbox_inches='tight', transparent=False)

