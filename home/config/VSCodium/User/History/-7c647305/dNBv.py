import matplotlib.pyplot as plt
import numpy as np

# Sample data
x = np.linspace(0, 10, 10000)
y = np.sin(x)

# Create the plot
plt.plot(x, y, label='sin(x)')
plt.title('Sine Wave')
plt.xlabel('x')
plt.ylabel('sin(x)')
plt.legend()
plt.grid(True)

# Display the plot
plt.savefig("plot.png", dpi = "600")