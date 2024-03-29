import numpy as np
import matplotlib.pyplot as plt
from PIL import Image, ImageFilter

window_size = 7
sampling_rate = 20

# compute zs and ys for a given pixel location (x, y)
def compute_zs_ys(image, x, y, window_size):
    half_window = window_size // 2
    padded_image = np.pad(image, half_window, mode='constant', constant_values=0)
    xs = padded_image[y:y+window_size, x:x+window_size].flatten()
    ys = image.getpixel((x, y))
    return xs, ys

def filter_image(desired_img, input_img, output_img_name):
    # Iterate through selected pixels to find zs and ys
    selected_pixels = [(x, y) for x in range(0, desired_img.width, sampling_rate) for y in range(0, desired_img.height, sampling_rate)]
    zs_list = []
    ys_list = []
    for x, y in selected_pixels:
        zs, ys = compute_zs_ys(input_img, x, y, window_size)
        zs_list.append(zs)
        ys_list.append(desired_img.getpixel((x, y)))

    # Compute Rzz and rzy using the selected zs and ys
    zs_array = np.array(zs_list).astype(np.float64)
    ys_array = np.array(ys_list).astype(np.float64)
    Rzz = np.dot(zs_array.T, zs_array) / len(selected_pixels)
    rzy = np.dot(zs_array.T, ys_array) / len(selected_pixels)

    # Compute filter coefficients
    theta_star = np.linalg.inv(Rzz).dot(rzy)
    print(theta_star.reshape(7, 7))

    # # Apply the optimal filter
    # print("applying filter to generate " + output_img_name)
    # filtered_input_img_data = np.zeros_like(np.array(input_img))
    # for y in range(input_img.height):
    #     for x in range(input_img.width):
    #         zs, _ = compute_zs_ys(input_img, x, y, window_size)
    #         filtered_value = np.dot(zs.astype(np.float64), theta_star)
    #         filtered_input_img_data[y, x] = np.clip(filtered_value, 0, 255)  # Clip to ensure valid pixel values
    
    # filtered_input_img = Image.fromarray(filtered_input_img_data.astype(np.uint8))
    # filtered_input_img.save('images/' + output_img_name + '.tif')
    # filtered_input_img.save('images/' + output_img_name + '.png')

def main():
    img_g = Image.open('images/img14g.tif').convert('L')
    img_bl = Image.open('images/img14bl.tif').convert('L')
    img_gn = Image.open('images/img14gn.tif').convert('L')
    img_sp = Image.open('images/img14sp.tif').convert('L')

    filter_image(img_g, img_bl, "filtered_img_bl")
    filter_image(img_g, img_gn, "filtered_img_gn")
    filter_image(img_g, img_sp, "filtered_img_sp")

    return 0

if __name__ == "__main__":
    main()