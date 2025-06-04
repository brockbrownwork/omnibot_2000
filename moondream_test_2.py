import time
from transformers import AutoModelForCausalLM, AutoTokenizer
from PIL import Image, ImageDraw

# Load the model
model = AutoModelForCausalLM.from_pretrained(
    "vikhyatk/moondream2",
    revision="2025-04-14",
    trust_remote_code=True,
    # Uncomment to run on GPU.
    # device_map={"": "cuda"}
)

# Load the image
image = Image.open("C:\\Users\\Brock\\Documents\\code\\omnibot_2000\\test_image.png")

# --- Captioning with timing ---

# Number of inferences to perform for averaging (after the first one)
num_inferences_for_average = 5 
inference_times = []

print("Starting captioning inference timing...")

# First inference
start_time = time.time()
_ = model.caption(image, length="normal")
end_time = time.time()
first_inference_time = end_time - start_time
print(f"First inference time: {first_inference_time:.4f} seconds")

# Subsequent inferences for averaging
print(f"Running {num_inferences_for_average} more inferences for averaging...")
for i in range(num_inferences_for_average):
    start_time = time.time()
    _ = model.caption(image, length="normal")
    end_time = time.time()
    inference_times.append(end_time - start_time)
    print(f"  Inference {i+1}: {inference_times[-1]:.4f} seconds")

average_inference_time = sum(inference_times) / len(inference_times)
print(f"Average inference time (excluding the first): {average_inference_time:.4f} seconds")

print("\n--- Other model functionalities (no timing) ---")

# Short caption
print("\nShort caption:")
print(model.caption(image, length="short")["caption"])

# Streaming generation example (no timing for streaming output)
print("\nNormal caption (streaming):")
for t in model.caption(image, length="normal", stream=True)["caption"]:
    print(t, end="", flush=True)
print() # New line after streaming

# Visual Querying
print("\nVisual query: 'How many people are in the image?'")
print(model.query(image, "How many people are in the image?")["answer"])

# Object Detection
print("\nObject detection: 'face'")
objects = model.detect(image, "face")["objects"]
print(f"Found {len(objects)} face(s)")

# Pointing
print("\nPointing: 'person'")
points = model.point(image, "person")["points"]
print(f"Found {len(points)} person(s)")

# Create the image with the points on them
# Draw the points on a copy of the image to avoid modifying the original if it's used elsewhere
image_with_points = image.copy()
for point in points:
    # point.x and point.y are normalized between 0 and 1, so we need to convert them to the image size
    x = int(point['x'] * image_with_points.width)
    y = int(point['y'] * image_with_points.height)
    # Draw a large green circle at the point
    draw = ImageDraw.Draw(image_with_points)
    draw.ellipse((x-10, y-10, x+10, y+10), fill=(0, 255, 0), outline=(0, 255, 0))


# Show the image with points
image_with_points.show()