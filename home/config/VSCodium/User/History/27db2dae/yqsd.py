import yfinance as yf

stock = yf.Ticker("MSFT")

hist = stock.history(period="5d", interval="30m")
open_prices = hist['Open'].tolist()
hist['Open'].to_csv('msft_open_prices.csv', header=True)
print(hist['Open'].head())