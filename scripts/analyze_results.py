import pandas as pd
import matplotlib.pyplot as plt
import os

def analyze():
    # Load data
    try:
        trades = pd.read_csv("trades.csv")
        book = pd.read_csv("book.csv")
        quotes = pd.read_csv("quotes.csv")
    except Exception as e:
        print(f"Error reading CSVs: {e}")
        return

    # Process Book Data
    book['timestamp'] = book['timestamp'].astype(float)
    book.set_index('timestamp', inplace=False)
    
    # Process Quotes Data
    quotes['timestamp'] = quotes['timestamp'].astype(float)
    
    fig, axes = plt.subplots(4, 1, figsize=(12, 12), sharex=True)
    
    # 1. Price and Quotes
    ax = axes[0]
    ax.plot(book['timestamp'], book['mid_price'], label='Mid Price', color='black', alpha=0.5)
    ax.plot(quotes['timestamp'], quotes['bid_price'], label='MM Bid', color='green', linestyle='--')
    ax.plot(quotes['timestamp'], quotes['ask_price'], label='MM Ask', color='red', linestyle='--')
    ax.plot(quotes['timestamp'], quotes['reservation_price'], label='Reservation Price', color='blue', alpha=0.3)
    ax.set_title("Market Price & MM Quotes (Avellaneda-Stoikov)")
    ax.legend(loc='upper right')
    
    # 2. Spread
    ax = axes[1]
    quotes['spread'] = quotes['ask_price'] - quotes['bid_price']
    ax.plot(quotes['timestamp'], quotes['spread'], label='MM Spread', color='purple')
    ax.set_title("Quoted Spread")
    ax.set_ylabel("Spread")
    
    # 3. Inventory
    ax = axes[2]
    # Merge book (which has inventory)
    ax.plot(book['timestamp'], book['inventory'], label='MM Inventory', color='brown')
    ax.axhline(0, color='black', linestyle=':', alpha=0.5)
    ax.set_title("Inventory Position")
    ax.legend()

    # 4. Inventory Skew Pressure
    ax = axes[3]
    merged = pd.merge_asof(quotes, book, on='timestamp', direction='nearest')
    merged['skew'] = merged['reservation_price'] - merged['mid_price']
    
    ax.plot(merged['timestamp'], merged['skew'], label='Reservation Price Skew (r - mid)', color='orange')
    ax.axhline(0, color='black', linestyle=':', alpha=0.5)
    ax.set_title("Inventory Skew Pressure (Reservation - Mid)")
    ax.legend()
    
    plt.tight_layout()
    plt.savefig("results.png")
    print("Analysis complete. Saved to results.png")

if __name__ == "__main__":
    analyze()
