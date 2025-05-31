import yfinance as yf

stock = yf.Ticker("MSFT")

hist = stock.history(start="2025-05-05", end="2025-05-09", interval="30m")
hist['Open'].to_csv('msft_open_prices.csv', header=True)

print(hist['Open'])