import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import Normalize
from matplotlib.patches import Circle
import pandas as pd

# DATASET 
data_file = './analysis/24125-Accuracy/computed_results_with_ground_truth.csv'
df = pd.read_csv(data_file)

x_actual = df['True_X']
y_actual = df['True_Y']
x_predicted = df['Computed_X']
y_predicted = df['Computed_Y']

# COMPUTATION, euclidean accuracy-method
errors = np.sqrt((x_actual - x_predicted)**2 + (y_actual - y_predicted)**2)
errors_percentage = (errors / np.max(errors)) * 100

# VISUALIZATION 
fig, ax = plt.subplots(figsize=(8, 7))

for x, y, error in zip(x_actual, y_actual, errors):
    circle = Circle((x, y), radius=error, color='gray', alpha=0.25)
    ax.add_patch(circle)

mic_positions = [(0, 0, 'MIC-L'), (50, 0, 'MIC-M'), (50, 50, 'MIC-R')]
for mic_x, mic_y, label in mic_positions:
    ax.plot(mic_x, mic_y, 'or', label=label)  # Plot static point
    ax.annotate(label, (mic_x, mic_y), textcoords="offset points", 
                xytext=(10, 10), ha='center', fontsize=10, color='red')
    
sc = ax.scatter(x_actual, y_actual, c=errors_percentage, cmap='plasma', 
                norm=Normalize(vmin=0, vmax=100), s=70, alpha=1, 
                edgecolor='k', label="Actual Points")


cbar = plt.colorbar(sc, ax=ax)
cbar.set_label('Error Magnitude (%)', fontsize=12)

major_ticks = np.arange(-10, 70, 10)
minor_ticks = np.arange(-10, 70, 1)

ax.set_xticks(major_ticks)
ax.set_xticks(minor_ticks, minor=True)
ax.set_yticks(major_ticks)
ax.set_yticks(minor_ticks, minor=True)

x_min, x_max = -10, 70
y_min, y_max = -10, 70
ax.set_xlim(x_min, x_max)
ax.set_ylim(y_min, y_max)

ax.set_title('Geometric Accuracy Plot', fontsize=14)
ax.set_xlabel('X Actual', fontsize=12)
ax.set_ylabel('Y Actual', fontsize=12)
ax.grid(which='both')
ax.grid(which='minor', alpha=0.2)
ax.grid(which='major', alpha=0.5)
ax.legend()

plt.tight_layout()
plt.show()