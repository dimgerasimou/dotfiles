import yfinance as yf

stock = yf.Ticker("MSFT")

hist = stock.history(start="2025-05-12", end="2025-05-17", interval="30m")
hist['Open'].to_csv('msft_open_prices.csv', header=True)

print(hist['Open'].head())