import csv
import pandas as pd
import numpy as np
import yfinance as yf
import matplotlib.pyplot as plt


df = pd.read_csv('msft_open_prices.csv')

x = np.arange(len(df))
y = df['Open'].values

# Split the data into training and testing
split_index = int(len(x) * 0.8)
x_train, x_test = x[:split_index], x[split_index:]
y_train, y_test = y[:split_index], y[split_index:]

# Polynomial degrees to test
degrees = [1, 2, 3]

# Initialize lists to store models, errors, and results
models = []
total_errors = np.zeros(len(degrees))
total_squared_errors = np.zeros(len(degrees))

# Fit polynomial models and calculate errors
for i, degree in enumerate(degrees):
	coeffs = np.polyfit(x_train, y_train, degree)
	poly_func = np.poly1d(coeffs)
	models.append(poly_func)
	
	# Calculate total errors (MAE and MSE)
	for j in range(len(x_test)):
		total_errors[i] += abs(poly_func(x_test[j]) - y_test[j])
		total_squared_errors[i] += (poly_func(x_test[j]) - y_test[j]) ** 2

# Calculate MAE and MSE
MAE = total_errors / len(x_test)
MSE = total_squared_errors / len(x_test)
min_MAE = MAE[0]
best_degree_idx = 0

# Print results
print(f"Data Length: {len(x)}")
print("MAE for each polynomial degree:")
for i, degree in enumerate(degrees):
	print(f"Degree {degree}: {MAE[i]}")
	if MAE[i] < min_MAE:
		best_degree_idx = i
		min_MAE = MAE[i]

print("\nMSE for each polynomial degree:")
for i, degree in enumerate(degrees):
	print(f"Degree {degree}: {MSE[i]}")

print(f"Best fitting model: Degree:{best_degree_idx + 1}, MAE:{min_MAE}")

best_model = models[degrees.index(best_degree_idx)]

# Create new x values for the next 3 days
new_x = np.arange(len(x), len(x) + 3 * 13)

# Step 3: Use the best polynomial model to predict the 'Open' prices for the next 3 days
predicted_prices = best_model(new_x)

# Δημιουργία γραφήματος
plt.figure(figsize=(10, 6))

# 1. Πραγματικές τιμές (y) ως προς το x
plt.plot(x, y, label="Actual Prices", color='blue', linestyle='-', marker='o')

# 2. Πολυωνυμική καμπύλη προσαρμογής
x_fit = np.linspace(0, len(x) + 3 * 13 - 1, 1000)  # Για να έχουμε πιο λείο διάγραμμα
y_fit = best_model(x_fit)
plt.plot(x_fit, y_fit, label=f"Polynomial Fit Degree {degrees[best_degree_idx]}", color='green', linestyle='--')

# 3. Προβλέψεις για τις επόμενες 3 ημέρες
plt.plot(new_x, predicted_prices, label="Predicted Prices (Next 3 Days)", color='red', linestyle='-', marker='x')

# Ρυθμίσεις γραφήματος
plt.xlabel('Time (days)')
plt.ylabel('Opening Price (USD)')
plt.title('MSFT Opening Prices and Polynomial Fit with Predictions for the Next 3 Days')
plt.legend()
plt.grid(True)

# Εμφάνιση του γραφήματος
plt.show()