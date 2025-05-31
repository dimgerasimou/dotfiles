import csv
import pandas as pd
import numpy as np

df = pd.read_csv('msft_open_prices.csv')


x = np.arange(len(df))
y = df['Open'].values

# Υπολογισμός ορίου split
split_index = int(len(x) * 0.8)

# Διαχωρισμός σε training και testing
x_train, x_test = x[:split_index], x[split_index:]
y_train, y_test = y[:split_index], y[split_index:]

coeffs_1 = np.polyfit(x_train, y_train, 1)
poly_func_1 = np.poly1d(coeffs_1)

coeffs_2 = np.polyfit(x_train, y_train, 2)
poly_func_2 = np.poly1d(coeffs_2)

coeffs_3 = np.polyfit(x_train, y_train, 3)
poly_func_3 = np.poly1d(coeffs_3)

MAE = 0.0

for i in x:
	print(x)