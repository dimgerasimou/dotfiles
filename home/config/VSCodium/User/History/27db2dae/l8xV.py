import yfinance as yf

stock = yf.Ticker("MSFT")

hist = stock.history(start="2024-05-14", end="2024-05-19", interval="30m")
hist['Open'].to_csv('msft_open_prices.csv', header=True)

print(hist['Open'].head())