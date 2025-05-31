import yfinance as yf

stock = yf.Ticker("MSFT")

hist = stock.history(start="2025-05-09", end="2025-05-14", interval="30m")
hist['Open'].to_csv('msft_open_prices.csv', header=True)

print(hist['Open'])