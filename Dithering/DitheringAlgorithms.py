from PIL import Image
import numpy as np

# --- DITHERING ALGORITHMS (NumPy-accelerated) ---


def dither_floyd_steinberg(img: Image.Image) -> Image.Image:
    """Floyd-Steinberg dithering with NumPy array operations."""
    arr = np.array(img, dtype=np.float32)
    height, width = arr.shape

    for y in range(height):
        for x in range(width):
            old = arr[y, x]
            new = 0.0 if old < 128 else 255.0
            arr[y, x] = new
            error = old - new

            if x + 1 < width:
                arr[y, x + 1] += error * 7 / 16
            if y + 1 < height:
                if x - 1 >= 0:
                    arr[y + 1, x - 1] += error * 3 / 16
                arr[y + 1, x] += error * 5 / 16
                if x + 1 < width:
                    arr[y + 1, x + 1] += error * 1 / 16

    return Image.fromarray(np.clip(arr, 0, 255).astype(np.uint8))


def dither_threshold(img: Image.Image, threshold: int = 128) -> Image.Image:
    """Simple threshold dithering"""
    arr = np.array(img, dtype=np.uint8)
    result = np.where(arr < threshold, 0, 255).astype(np.uint8)
    return Image.fromarray(result)


def dither_random_noise(img: Image.Image, threshold: int = 128) -> Image.Image:
    """Random noise dithering"""
    arr = np.array(img, dtype=np.float32)
    noise = np.random.uniform(-128, 128, arr.shape).astype(np.float32)
    result = np.where(arr + noise < threshold, 0, 255).astype(np.uint8)
    return Image.fromarray(result)


def dither_atkinson(img: Image.Image) -> Image.Image:
    """Atkinson dithering with NumPy array operations."""
    arr = np.array(img, dtype=np.float32)
    height, width = arr.shape

    for y in range(height):
        for x in range(width):
            old = arr[y, x]
            new = 0.0 if old < 128 else 255.0
            arr[y, x] = new
            error = (old - new) / 8.0

            if x + 1 < width:
                arr[y, x + 1] += error
            if x + 2 < width:
                arr[y, x + 2] += error
            if y + 1 < height:
                if x - 1 >= 0:
                    arr[y + 1, x - 1] += error
                arr[y + 1, x] += error  # always runs when y+1 is valid
                if x + 1 < width:
                    arr[y + 1, x + 1] += error
            if y + 2 < height:
                arr[y + 2, x] += error

    return Image.fromarray(np.clip(arr, 0, 255).astype(np.uint8))


def dither_bayer(img: Image.Image) -> Image.Image:
    """Ordered dithering using a 4x4 Bayer matrix"""
    BAYER_4x4 = (
        np.array(
            [[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]],
            dtype=np.float32,
        )
        / 16.0
    )

    arr = np.array(img, dtype=np.float32)
    height, width = arr.shape

    # Tile the Bayer matrix to cover the full image size
    tiled = np.tile(BAYER_4x4, (height // 4 + 1, width // 4 + 1))[:height, :width]

    result = np.where(arr / 255.0 < tiled, 0, 255).astype(np.uint8)
    return Image.fromarray(result)
