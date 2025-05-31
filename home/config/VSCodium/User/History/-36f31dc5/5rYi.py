import csv
import pandas as pd

df = pd.read_csv('msft_open_prices.csv')
data = df['Open'].tolist()

print(data)