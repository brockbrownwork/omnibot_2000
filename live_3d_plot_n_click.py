import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.animation as animation
import time

# --- Global variables to store and update data ---
x_data = []
y_data = []
z_data = []
labels = []
current_index = 0

# --- Setup the initial plot ---
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.set_title('Real-time 3D Scatter Plot with Clickable Points (Auto-scaling)')

# Create an empty scatter plot initially.
scatter = ax.scatter(x_data, y_data, z_data, picker=5) # picker=5 allows clicking within 5 points

# --- Define the callback function for point clicks ---
def on_pick(event):
    if event.artist == scatter:
        indices = event.ind
        print(f'Picked points with indices: {indices}')
        for i in indices:
            if 0 <= i < len(x_data):
                 # Use .item() to get scalar values from numpy arrays if needed,
                 # though with lists direct indexing works fine.
                 print(f'  - Data for index {i}: x={x_data[i]:.2f}, y={y_data[i]:.2f}, z={z_data[i]:.2f}, label={labels[i]}')
            else:
                 print(f'  - Picked index {i} is out of current data bounds.')

# Connect the callback function to the 'pick_event'
fig.canvas.mpl_connect('pick_event', on_pick)

# --- Define the animation update function ---
def update(frame):
    global x_data, y_data, z_data, labels, current_index

    # --- Simulate receiving new data points ---
    num_new_points = 2
    # Generate random data between 0 and 10 for better visualization of scaling
    new_x = np.random.rand(num_new_points) * 10
    new_y = np.random.rand(num_new_points) * 10
    new_z = np.random.rand(num_new_points) * 10
    new_labels = [f'Point {current_index + i}' for i in range(num_new_points)]

    x_data.extend(new_x)
    y_data.extend(new_y)
    z_data.extend(new_z)
    labels.extend(new_labels)

    # Update the data of the existing scatter artist
    scatter._offsets3d = (x_data, y_data, z_data)

    # --- Dynamically adjust axis limits and aspect ratio ---
    if x_data: # Ensure data is not empty
        x_min, x_max = min(x_data), max(x_data)
        y_min, y_max = min(y_data), max(y_data)
        z_min, z_max = min(z_data), max(z_data)

        # Add a small padding
        x_range = x_max - x_min
        y_range = y_max - y_min
        z_range = z_max - z_min

        # Handle cases where range is zero (e.g., only one point)
        padding = 1.0 # Base padding
        x_padding = max(padding, x_range * 0.1) # 10% padding or base padding
        y_padding = max(padding, y_range * 0.1)
        z_padding = max(padding, z_range * 0.1)


        # Set limits with padding
        ax.set_xlim([x_min - x_padding/2, x_max + x_padding/2])
        ax.set_ylim([y_min - y_padding/2, y_max + y_padding/2])
        ax.set_zlim([z_min - z_padding/2, z_max + z_padding/2])

        # Set box aspect ratio to maintain correct scales
        # The aspect ratio should be proportional to the data ranges
        # Avoid division by zero if ranges are zero
        aspect_x = max(x_range, padding) # Use padding if range is zero
        aspect_y = max(y_range, padding)
        aspect_z = max(z_range, padding)

        ax.set_box_aspect([aspect_x, aspect_y, aspect_z])


    current_index += num_new_points

    # Return the artist(s) that have been updated
    return scatter,


# --- Create the animation ---
# Using blit=False for compatibility with dynamic axis limit changes
ani = animation.FuncAnimation(fig, update, frames=None, interval=100, blit=False, cache_frame_data=False)

# --- Show the plot ---
plt.show()
