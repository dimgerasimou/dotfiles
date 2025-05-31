import yfinance as yf

stock = yf.Ticker("MSFT")

hist = stock.history(period="5d", interval="30m")
open_prices = hist['Open'].tolist()

print(open_prices)