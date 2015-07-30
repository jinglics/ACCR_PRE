#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
using namespace cv;

class Preprocess{

public:
  Preprocess(const Mat & im);

  ~Preprocess();

  Mat get_source() const;

  Mat get_result() const;

  Mat get_invert() const;

  void rgb2gray();

  void rgb2gray(Mat& image, int const thresh = 180);

  void binarization(int const thresh = 180);

  void binarization(Mat& image, int const thresh = 180);

  void gaussion_blur(int const size = 5);

  void gaussion_blur(Mat &image, int const size = 5);

  void erosion(int const erosion_type = 0, int const erosion_size = 3);

  void dilation(int const dilation_type, int const dilation_size);

  void generate_invert();

  void morphology(Mat& image, int const size = 5);

  bool background();

  void process();

  vector<vector<Rect> > ClusterRectOnY(vector<Rect> rects);
  vector<vector<Rect> > ClusterRectOnX(vector<Rect> rects);

  vector<Rect> FilterRectsOnY(vector<Rect> box);
  vector<Rect> FilterRectsOnX(vector<Rect> box);

  vector<vector<Rect> > SplitRectsOnY(vector<Rect> rects);
  vector<vector<Rect> > SplitRectsOnX(vector<Rect> rects);

  Rect CombineRects(vector<Rect> rects);

  Rect CombineRectsOpencv(vector<Rect> rects);

  vector<Mat> TextAreaCandidateOnY(Mat image, int thresh = 170);
  vector<Mat> TextAreaCandidateOnX(Mat image, int thresh = 170);

  void TextAreaCandidateOnY_draw(Mat image, char* filename, int thresh = 170);

  bool close(int v1, int v2, int distance);


private:
  Mat source_image, result_image, invert_image;
};