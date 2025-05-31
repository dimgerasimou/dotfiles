import yfinance as yf

stock = yf.Ticker("MSFT")

hist = stock.history(period="5d", interval="30m")
print(hist.Open)