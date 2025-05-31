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

total_error_1 = 0.0
total_error_2 = 0.0
total_error_3 = 0.0

total_s_error_1 = 0.0
total_s_error_2 = 0.0
total_s_error_3 = 0.0


for i in range(len(x_test)):
	total_error_1 = total_error_1 + abs(poly_func_1(x_test[i]) - y_test[i])
	total_error_2 = total_error_2 + abs(poly_func_2(x_test[i]) - y_test[i])
	total_error_3 = total_error_3 + abs(poly_func_3(x_test[i]) - y_test[i])
	total_s_error_1 = total_s_error_1 + (poly_func_1(x_test[i]) - y_test[i])**2
	total_s_error_2 = total_s_error_2 + (poly_func_2(x_test[i]) - y_test[i])**2
	total_s_error_3 = total_s_error_3 + (poly_func_3(x_test[i]) - y_test[i])**2

MΑE_1 = total_error_1 / len(x_test)
MΑE_2 = total_error_2 / len(x_test)
MΑE_3 = total_error_3 / len(x_test)
MSE_1 = total_s_error_1 / len(x_test)
MSE_2 = total_s_error_2 / len(x_test)
MSE_3 = total_s_error_3 / len(x_test)


print(MΑE_1)
print(MΑE_2)
print(MΑE_3)

print(MSE_1)
print(MSE_2)
print(MSE_3)