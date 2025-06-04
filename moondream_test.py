from transformers import AutoModelForCausalLM, AutoTokenizer
from PIL import Image, ImageDraw
from IPython.display import Image, display

model = AutoModelForCausalLM.from_pretrained(
    "vikhyatk/moondream2",
    revision="2025-04-14",
    trust_remote_code=True,
    device_map={"": "cuda"}
)

image = Image.open("C:\\Users\\Brock\\Documents\\code\\omnibot_2000\\test_image.png")

# Captioning
print("Short caption:")
print(model.caption(image, length="short")["caption"])

print("\nNormal caption:")
for t in model.caption(image, length="normal", stream=True)["caption"]:
    # Streaming generation example, supported for caption() and detect()
    print(t, end="", flush=True)
print(model.caption(image, length="normal"))

# Visual Querying
print("\nVisual query: 'How many people are in the image?'")
print(model.query(image, "How many people are in the image?")["answer"])

# Object Detection
print("\nObject detection: 'face'")
objects = model.detect(image, "face")["objects"]
print(f"Found {len(objects)} face(s)")

# Pointing
thing_to_find = "person holding a cup of coffee"
print(f"\nPointing: '{thing_to_find}'")
points = model.point(image, thing_to_find)["points"]
print(f"Found {len(points)} {thing_to_find}(s)")

# create the image with the points on them

# draw the points on the image
for point in points:
    # point.x and point.y are normalized between 0 and 1, so we need to convert them to the image size
    x = int(point['x'] * image.width)
    y = int(point['y'] * image.height)
    # Draw a large green circle at the point
    draw = ImageDraw.Draw(image)
    draw.ellipse((x-10, y-10, x+10, y+10), fill=(0, 255, 0), outline=(0, 255, 0))

# show the image
image.show()
