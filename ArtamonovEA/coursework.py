import numpy as np
import cv2


def detect_edges(self, pixels: np.ndarray) -> np.ndarray:
    """
    Detects edges in the image using an enhanced hybrid edge detection combining
    Difference of Gaussians, Sobel operators, non-maximum suppression, and hysteresis.

    Args:
        pixels (np.ndarray): The input image data (RGB or grayscale).

    Returns:
        np.ndarray: Array representing detected edges with directional information.
    """
    # Input validation
    gray = cv2.cvtColor(pixels, cv2.COLOR_RGB2GRAY)

    # Remove noise using Gaussian blur
    gray = cv2.GaussianBlur(gray, (3, 3), 0)

    # Canny edge detection with fine-tuned thresholds
    v = np.median(gray)
    sigma = 0.1
    lower = int(max(0, (1.0 - sigma) * v))
    upper = int(min(255, (1.0 + sigma) * v))
    edges = cv2.Canny(gray, lower, upper)

    # Sobel operators for edge orientation
    sobelx = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=3)
    sobely = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=3)

    # Calculate edge orientation
    orientation = np.arctan2(sobely, sobelx) * (180 / np.pi)
    orientation = np.mod(orientation + 180, 180)  # Normalize to 0-180°

    # Initialize output array
    output_height = gray.shape[0] // self.textures.size
    output_width = gray.shape[1] // self.textures.size
    ascii_edges = np.zeros((output_height, output_width), dtype=np.uint8)


    # Map edges to ASCII characters based on orientation
    for i in range(output_height):
        for j in range(output_width):
            # Sample region for edge detection
            region = edges[i * self.textures.size : (i + 1) * self.textures.size, j * self.textures.size : (j + 1) * self.textures.size]

            if np.any(region):  # If edges exist in region
                # Get average orientation in region
                region_orient = orientation[
                                i * self.textures.size : (i + 1) * self.textures.size, j * self.textures.size : (j + 1) * self.textures.size
                                ]
                avg_orient = np.mean(region_orient[region > 0])

                # Map orientation to ASCII characters
                # 0: no edge, 1: "-", 2: "|", 3: "\", 4: "/"
                if 22.5 <= avg_orient < 67.5:
                    ascii_edges[i, j] = 3  # "\"
                elif 67.5 <= avg_orient < 112.5:
                    ascii_edges[i, j] = 1  # "|"
                elif 112.5 <= avg_orient < 157.5:
                    ascii_edges[i, j] = 4  # "/"
                else:
                    ascii_edges[i, j] = 2  # "-"
            else:
                ascii_edges[i, j] = 0  # No edge

    return ascii_edges