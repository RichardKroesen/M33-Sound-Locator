import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from scipy.fft import fft, fftfreq

# DATASET 
data_file = '17125_measurements/FM_BF600/static.csv'  # Replace with your filename
df = pd.read_csv(data_file)

# DEFINES
SAMPLE_RATE = (500_000 / 3)
START_FROM = 30_000
MAX_SPECTRUM = 1000

# PRE PROCESSING
df = df - df.mean()
df_trimmed = df.iloc[START_FROM:len(df) - START_FROM]
x = np.arange(len(df_trimmed))

# VISUALIZE 
plt.figure(figsize=(10, 6))
plt.plot(x, df_trimmed['RIGHT'], label='RIGHT', marker='o')
plt.plot(x, df_trimmed['MID'], label='MID', marker='o')
plt.plot(x, df_trimmed['LEFT'], label='LEFT', marker='o')

plt.xlabel('Sample Number(n)')
plt.ylabel('Value')
plt.title('Plot of RIGHT, MID, and LEFT Values (DC Removed)')
plt.xlim(0, START_FROM)
plt.grid(True)
plt.legend()
plt.show()

# Frequency Spectrum Analysis
y1 = df_trimmed['RIGHT'].values
y2 = df_trimmed['MID'].values
y3 = df_trimmed['LEFT'].values

n = len(y1)
xf = fftfreq(n, 1 / SAMPLE_RATE)[:n // 2]

yf1 = fft(y1)
yf2 = fft(y2)
yf3 = fft(y3)

plt.figure(figsize=(10, 6))
plt.plot(xf, 2.0/n * np.abs(yf1[:n // 2]), label = "R")
plt.plot(xf, 2.0/n * np.abs(yf2[:n // 2]), label = "M")
plt.plot(xf, 2.0/n * np.abs(yf3[:n // 2]), label = "L")

plt.title(f'Frequency Spectrum (DC Removed)')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude')
plt.xlim(5, MAX_SPECTRUM)
plt.legend()
plt.grid(True)
plt.show()