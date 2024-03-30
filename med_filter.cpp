#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>

// Used by std::sort for determining order
bool comparePixelPairs(const std::pair<int, int> &a,
                       const std::pair<int, int> &b) {
  return a.first < b.first;
}

// Apply weighted median filter
void weightedMedianFilter(cv::Mat &inputImage, cv::Mat &outputImage) {
  int rows = inputImage.rows;
  int cols = inputImage.cols;

  // Create copy of input image
  inputImage.copyTo(outputImage);

  // Define the weighted median filter weights
  const int weights[5][5] = {{1, 1, 1, 1, 1},
                             {1, 2, 2, 2, 1},
                             {1, 2, 2, 2, 1},
                             {1, 2, 2, 2, 1},
                             {1, 1, 1, 1, 1}};

  // Calculate total weight of the mask
  int totalWeight = 0;
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      totalWeight += weights[i][j];
    }
  }

  // Iterate through each pixel of the image
  for (int i = 2; i < rows - 2; i++) {
    for (int j = 2; j < cols - 2; j++) {
      std::vector<std::pair<int, int>> pixelPairs;

      // Gather pixels and their corresponding weights in the 5x5 window
      for (int m = -2; m <= 2; m++) {
        for (int n = -2; n <= 2; n++) {
          pixelPairs.push_back(std::make_pair(
              inputImage.at<uchar>(i + m, j + n), weights[m + 2][n + 2]));
        }
      }

      // Sort pixels based on their values not including weights
      std::sort(pixelPairs.begin(), pixelPairs.end(), comparePixelPairs);

      // Find the weighted median
      int sumWeights = 0;
      int medianIndex = 0;
      for (size_t p = 0; p < pixelPairs.size(); p++) {
        sumWeights += pixelPairs[p].second;
        if (sumWeights * 2 >= totalWeight) { // Median index calculation
          medianIndex = p;
          break;
        }
      }

      // Assign the median value to the output pixel
      outputImage.at<uchar>(i, j) = pixelPairs[medianIndex].first;
    }
  }
}

int main() {
  // Load input images
  cv::Mat originalImage = cv::imread("images/img14g.tif", cv::IMREAD_GRAYSCALE);
  cv::Mat imgGn = cv::imread("images/img14gn.tif", cv::IMREAD_GRAYSCALE);
  cv::Mat imgSp = cv::imread("images/img14sp.tif", cv::IMREAD_GRAYSCALE);

  // Check if the image has been loaded successfully
  if (originalImage.empty() || imgGn.empty() || imgSp.empty()) {
    std::cout << "Error: Unable to load image(s)" << std::endl;
    return -1;
  }

  // Create output images for filtered results
  cv::Mat filteredGn(imgGn.size(), CV_8U);
  cv::Mat filteredSp(imgSp.size(), CV_8U);

  // Apply weighted median filter to the noisy images
  weightedMedianFilter(imgGn, filteredGn);
  weightedMedianFilter(imgSp, filteredSp);

  // Save the filtered images
  cv::imwrite("images/med_filter_img_gn.tif", filteredGn);
  cv::imwrite("images/med_filter_img_gn.png", filteredGn);
  cv::imwrite("images/med_filter_img_sp.tif", filteredSp);
  cv::imwrite("images/med_filter_img_sp.png", filteredSp);

  return 0;
}
