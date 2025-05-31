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

print(x)
print(x_train)
print(x_test)