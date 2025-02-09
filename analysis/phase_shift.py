import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import correlate

# Script analysis setup # 
SAMPLE_RATE = (500_000 / 3)     
csv_file = "./analysis/24125-Atlas/P55x-5.csv"

def remove_dc_offset(signal):
    """Remove DC offset from a signal."""
    return signal - np.mean(signal)

def normalize_signal(signal):
    """Normalize a signal to the range [-1, 1]."""
    return (signal - np.min(signal)) / (np.max(signal) - np.min(signal)) * 2 - 1

def compute_phase_shift(signal1, signal2, sampling_rate):
    """Compute the phase shift between two signals in seconds."""
    correlation = correlate(signal1, signal2, mode='full')
    lag = np.argmax(correlation) - (len(signal1) - 1)
    time_shift = lag / sampling_rate
    return time_shift

def main():
    data = pd.read_csv(csv_file)

    mid_signal = data['MID'].values
    left_signal = data['LEFT'].values
    right_signal = data['RIGHT'].values
    
    # Pre Processing # 
    mid_signal = remove_dc_offset(mid_signal)
    left_signal = remove_dc_offset(left_signal)
    right_signal = remove_dc_offset(right_signal)

    mid_signal = normalize_signal(mid_signal)
    left_signal = normalize_signal(left_signal)
    right_signal = normalize_signal(right_signal)

    # Feature Computation #  
    phase_shift_mid_left = compute_phase_shift(left_signal, mid_signal, SAMPLE_RATE)
    phase_shift_mid_right = compute_phase_shift(left_signal, right_signal, SAMPLE_RATE)

    # Visualization #
    print(f"Phase shift between LEFT and MID: {phase_shift_mid_left:.6f} seconds")
    print(f"Phase shift between LEFT and RIGHT: {phase_shift_mid_right:.6f} seconds")
    
    time = np.arange(len(mid_signal)) / SAMPLE_RATE
    plt.figure(figsize=(10, 6))
    plt.plot(time, mid_signal, label='MID')
    plt.plot(time, left_signal, label='LEFT')
    plt.plot(time, right_signal, label='RIGHT')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.title('Normalized Signals')
    plt.legend()
    plt.grid()
    plt.show()

if __name__ == "__main__":
    main()