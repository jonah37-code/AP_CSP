import requests
import key
import numpy as np
import statsmodels.tsa.stattools as ts
import time

base_url = "https://www.alphavantage.co/query?"
API_key = key.get_key()

def main():

    stock1 = input("Stock #1 (symbol): ").upper()
    stock2 = input("Stock #2 (symbol): ").upper()
    start = input("Start Date (YYYY-MM-DD): ")
    end = input("End Date (YYYY-MM-DD): ")

    statistic = input("Choose 1 (correlation coefficient), 2 (hedge ratio), or 3 (cointegration p-value): ")

    data1 = get_list_slice(retrieve_data(stock1), start, end)
    time.sleep(1)
    data2 = get_list_slice(retrieve_data(stock2), start, end)

    if statistic == "1":
        print(get_correlation(data1, data2))
    elif statistic == "2":
        print(get_hedge_ratio(data1, data2))
    elif statistic == "3":
        print(get_cointegration_p_value(data1, data2))
    else:
        print("Run again and enter valid number.")

    print(get_cointegration_p_value(data1, data2))

def retrieve_data(symbol):
    url = f"{base_url}function=TIME_SERIES_DAILY&symbol={symbol}&apikey={API_key}"
    try:
        response = requests.get(url)
    except Exception as e:
        print(f"Error: {e}")
    else:
        return response.json()
    
def get_list_slice(dict, start_date, end_date):
    data = []
    series = dict["Time Series (Daily)"]
    collecting = False
    for date in series:
        if date == end_date:
            collecting = True
        if collecting:
            data.append(float(series[date]["4. close"]))
        if date == start_date:
            break
    return data
    
def get_correlation(list_a, list_b):

    return np.corrcoef(list_a, list_b)[0, 1]

def get_hedge_ratio(list_a, list_b):

    slope, intercept = np.polyfit(list_a, list_b, 1) # Do linear regression
    return slope

def get_cointegration_p_value(list_a, list_b):

    x, p_value, y = ts.coint(list_a, list_b)
    return p_value

if __name__ == "__main__":
    main()