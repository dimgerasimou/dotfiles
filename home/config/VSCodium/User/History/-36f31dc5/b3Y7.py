import csv
import pandas as pd
import numpy as np
import yfinance as yf

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
best_dergee = 0

# Print results
print(f"Data Length: {len(x)}")
print("MAE for each polynomial degree:")
for i, degree in enumerate(degrees):
	print(f"Degree {degree}: {MAE[i]}")
	if MAE[i] < min_MAE:
		best_dergee = degree
		min_MAE = MAE[i]

print("\nMSE for each polynomial degree:")
for i, degree in enumerate(degrees):
	print(f"Degree {degree}: {MSE[i]}")

print(f"Best fitting model: Degree:{best_dergee}, MAE:{min_MAE}")

