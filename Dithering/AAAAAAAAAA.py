from PIL import Image, ImageFont
from pathlib import Path
import os
import sys
import subprocess
import DitheringAlgorithms

from tkinter import Tk
from tkinter.filedialog import askopenfilename

# --- SETTINGS ---
BRAILLE_WIDTH = 300   # number of braille characters horizontally
VERTICAL_CORRECTION = 1.1  # compensates for braille cell aspect ratio


# --- FILE SELECTION ---
def select_image() -> Path:
    Tk().withdraw()
    path = askopenfilename(
        title="Select an image",
        filetypes=[
            ("Image files", "*.png *.jpg *.jpeg"),
            ("PNG files", "*.png"),
            ("JPEG files", "*.jpg *.jpeg"),
        ]
    )
    if not path:
        print("No file selected.")
        sys.exit(0)
    return Path(path)


# --- IMAGE LOADING & RESIZING ---
def load_and_resize(image_path: Path, target_width: int, vertical_correction: float) -> Image.Image:
    img = Image.open(image_path).convert("L")  # grayscale

    orig_width, orig_height = img.size
    new_width = target_width * 2  # 2 pixels per braille dot horizontally
    scale = new_width / orig_width
    new_height = int(orig_height * scale * vertical_correction)

    return img.resize((new_width, new_height), Image.Resampling.LANCZOS)


# --- BRAILLE CONVERSION ---
def pixels_to_braille(block: list[int]) -> str:
    """
    Convert a 2x4 pixel block to a braille unicode character.

    Braille dot layout (index → position):
      0 3
      1 4
      2 5
      6 7
    """
    dots = 0
    for i, val in enumerate(block):
        if val > 0:
            dots |= 1 << i
    return chr(0x2800 + dots)


def image_to_braille(img: Image.Image) -> str:
    """Convert a dithered grayscale image to a braille string."""
    pixels = img.load()
    width, height = img.size
    lines = []

    for y in range(0, height, 4):
        line = ""
        for x in range(0, width, 2):
            block = []
            for dy in range(4):
                for dx in range(2):
                    px = 0
                    if y + dy < height and x + dx < width:
                        px = pixels[x + dx, y + dy]
                    block.append(px)
            line += pixels_to_braille(block)
        lines.append(line)

    return "\n".join(lines)


# --- OUTPUT ---
def to_html_table(text: str) -> str:
    lines = text.split("\n")
    rows = ""
    for line in lines:
        cells = "".join(f"<td>{char}</td>" for char in line)
        rows += f"<tr>{cells}</tr>"

    return f""" 
<!DOCTYPE html>
<html>
<head>
    <style>
        body {{
            background-color: #1e1e1e;
            color: #ffffff;
        }}
        table {{
            border-collapse: collapse;
            line-height: 1;
        }}
        td {{
            font-family: 'Segoe UI Symbol', monospace;
            font-size: 12px;
            width: 8px;
            min-width: 8px;  /* ADD THIS */
            height: 15px;
            padding: 0;
            text-align: center;
            overflow: hidden;
        }}
    </style>
</head>
<body>
    <table>{rows}</table>
</body>
</html>
            """

def save_and_open_html(text: str, output_path: Path):
    html = to_html_table(text)

    html_path = output_path.with_suffix(".html")
    html_path.write_text(html, encoding="utf-8")
    print(f"Saved to: {html_path}")

    chrome_paths = [
        r"C:\Program Files\Google\Chrome\Application\chrome.exe",
        r"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe",
        "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome",
        "google-chrome",
        "chromium-browser"
    ]

    for chrome in chrome_paths:
        try:
            subprocess.run([chrome, html_path.as_uri()])
            return
        except (FileNotFoundError, OSError):
            continue

    if sys.platform.startswith("win"):
        os.startfile(html_path)
    elif sys.platform == "darwin":
        subprocess.run(["open", html_path])
    else:
        subprocess.run(["xdg-open", html_path])

def save_and_open_txt(text: str, output_path: Path):
    output_path.write_text(text, encoding="utf-8")
    print(f"Saved to: {output_path}")

    chrome_paths = [
        r"C:\Program Files\Google\Chrome\Application\chrome.exe",
        r"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe",
        "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome",  # macOS
        "google-chrome",   # Linux
        "chromium-browser" # Linux fallback
    ]

    for chrome in chrome_paths:
        try:
            subprocess.run([chrome, output_path.as_uri()])
            return
        except (FileNotFoundError, OSError):
            continue

    # Fall back to default opener
    if sys.platform.startswith("win"):
        os.startfile(output_path)
    elif sys.platform == "darwin":
        subprocess.run(["open", output_path])
    else:
        subprocess.run(["xdg-open", output_path])

# --- MAIN ---
def main():
    image_path = select_image()
    print(f"Selected: {image_path}")

    img = load_and_resize(image_path, BRAILLE_WIDTH, VERTICAL_CORRECTION)

    # Swap dithering function here to try different algorithms
    img = DitheringAlgorithms.dither_atkinson(img)
    # img = dither_threshold(img)

    braille = image_to_braille(img)

    # output_path = image_path.with_suffix(".txt")
    output_path = Path(__file__).parent / "output.txt"
    save_and_open_txt(braille, output_path)

if __name__ == "__main__":
    main()