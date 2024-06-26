## Image Restoration

### MMSE Filters

#### Original Images

![alt text](../images/img_g.png)

![alt text](../images/img_bl.png)

![alt text](../images/img_gn.png)

![alt text](../images/img_sp.png)

#### Output Images

##### Filtered Blurred (img14bl.tif)

![alt text](../images/filtered_img_bl.png)

##### Filtered Grain (img14gn.tif)

![alt text](../images/filtered_img_gn.png)

##### Filtered Sparse Grain (img14sp.tif)

![alt text](../images/filtered_img_sp.png)

#### Filters

##### Blur Filter (img14bl.tif)

```py
theta_star =
[[ 1.58662937 -1.77636037 -0.18195097  0.8099888  -0.32429833  0.17281192 0.51186547]
 [-0.07757358 -0.99571168  0.09407374 -0.13784249 -0.79670869 -0.1422887 -0.15685742]
 [ 0.89242222 -1.30843815  0.50707461  1.86034897  0.03737642 -1.72423461 -0.15164415]
 [-1.04077975  0.19386813  1.45227105  2.31036607  0.80673293 -0.16954558 0.04773358]
 [ 0.22989286 -0.87149869 -0.08213675  1.06257072  0.34656016 -1.07352948 0.69357394]
 [-0.50389355 -0.59276408  0.19579165  0.34823317 -0.73971568 -2.19760302 1.17881683]
 [ 1.11450306 -0.67595211 -0.08352578 -0.60618441  0.88782229  0.02420487 0.04463162]]
```

##### Grain Filter (img14gn.tif)

```py
theta_star =
[[-2.10237736e-02 -1.39100597e-02 -3.44508180e-02  6.12177355e-02 4.69732119e-02  1.25872454e-02 -1.82431520e-02]
 [ 1.69015765e-02 -5.01770354e-03 -1.98525872e-02  5.57943402e-02 4.24371855e-02 -3.32136857e-02 -2.85983262e-02]
 [-5.09105494e-02 -9.93752502e-03  2.28328809e-02  1.02835716e-01 1.47605180e-02 -4.40270798e-02 -5.02643095e-02]
 [ 1.64276196e-02  1.54823570e-02  1.11956179e-01  2.43735586e-01 1.04173127e-01  5.62519884e-02  2.59118175e-02]
 [ 1.78111518e-02  4.92957561e-02  3.00778552e-02  1.15035973e-01 5.98409985e-02  1.68146474e-02  3.10760887e-02]
 [-1.57724679e-02  1.51601656e-02  1.23035464e-03  4.36985068e-02 9.03948088e-03  3.59433669e-02 -7.26633472e-03]
 [-2.82745471e-02 -6.86251154e-03 -1.22155586e-02  3.79296269e-02 -2.10285184e-05  7.07343996e-03 -1.35792671e-02]]
```

##### Sparse Grain Filter (img14sp.tif)

```py
theta_star =
[[ 0.02127461 -0.03503006 -0.00114477  0.07965957  0.06473001  0.00451991 -0.0205594 ]
 [ 0.00175518 -0.00769925 -0.04709879  0.03675556  0.0243313  -0.02385908 -0.03043562]
 [-0.04181806 -0.04186489  0.02982256  0.12449481 -0.01044216 -0.03448667 -0.05109824]
 [ 0.04987909  0.01340941  0.0855091   0.23468021  0.12306044  0.02810357 0.06205186]
 [ 0.00042518  0.02139996  0.04749334  0.13224705  0.06676584  0.0107997 0.01479232]
 [-0.01650404  0.01800627  0.0109205   0.06352337  0.0283775   0.00900784 -0.02739613]
 [-0.01678836 -0.00223707 -0.01461535  0.00374868 -0.00163233  0.01241363 0.00550534]]
```

### Weighted Median Filtering

#### Grain Filtered (img14gn.tif)

![alt text](../images/med_filter_img_gn.png)

#### Sparse Grain Filtered (img14sp.tif)

![alt text](../images/med_filter_img_sp.png)

#### C code

```cpp
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
```
