import csv
import pandas as pd
import numpy as np

df = pd.read_csv('msft_open_prices.csv')
data = df['Open'].tolist()

print(data)