import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from scipy.signal import firwin, lfilter
from scipy.fft import fft, fftfreq

# DATASET 
data_file = '17125_measurements/FM_BF600/BUZ-P20x20.csv'
df = pd.read_csv(data_file)

# DEFINES
NUM_TAPS = 51 
PASS_BAND = 3995 
BW = 5
SAMPLE_RATE = (500_000 / 3)

# PRE PROCESSING
x = np.arange(len(df))

bandpass_coefficients = firwin(
    NUM_TAPS,
    [
        (PASS_BAND - BW) / (1/2 * SAMPLE_RATE),
        (PASS_BAND + BW) / (1/2 * SAMPLE_RATE)
    ],
    pass_zero = False
)

df_filtered = df.copy()
for column in ['RIGHT', 'MID', 'LEFT']:
    df_filtered[column] = lfilter(bandpass_coefficients, 1.0, df[column])

# VISUALIZE 
plt.figure(figsize=(10, 6))
plt.plot(x, df_filtered['RIGHT'], label='RIGHT', marker='o')
plt.plot(x, df_filtered['MID'], label='MID', marker='o')
plt.plot(x, df_filtered['LEFT'], label='LEFT', marker='o')

plt.xlabel('Sample Number(n)')
plt.ylabel('Value')
plt.title('Plot of RIGHT, MID, and LEFT Values (DC Removed)')
# plt.xlim(0,1000)
plt.grid(True)
plt.legend()
plt.show()

# Frequency Spectrum Analysis
y1 = df_filtered['RIGHT'].values
y2 = df_filtered['MID'].values
y3 = df_filtered['LEFT'].values

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
# plt.xlim(5, MAX_SPECTRUM)
plt.legend()
plt.grid(True)
plt.show()