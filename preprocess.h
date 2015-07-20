#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
using namespace cv;
RNG rng(12345);

class Preprocess{
  Mat source_image, result_image, invert_image;
public:
  Preprocess(const Mat & im){
  }
  ~Preprocess(){

  }

  Mat get_source() const{
  }

  Mat get_result() const{
  }

  Mat get_invert() const{
  }

  void rgb2gray(){
  }

  void rgb2gray(Mat& image){
  }

  void binarization(Mat& image){
  }

  void binarization(int thresh = 180){
  }

  void gaussion_blur(int size = 5){
  }
  void gaussion_blur(Mat &image, int size = 5){
  }

  void erosion(int erosion_type = 0, int erosion_size = 3){
     }

  void dilation(int dilation_type, int dilation_size){
   }

  void generate_invert(){
     }

  void morphology(Mat& image, int size = 5){
  }

  bool background(){
  }

  void process(){
  }

  vector<vector<Rect> > combine_rect(vector<Rect> rects){
  }

  vector<Rect> filter_rects(vector<Rect> box){
  }

  vector<vector<Rect> > split_rects(vector<Rect> rects){
  }


  void position_filter(Rect text_religon){
  }

  vector<Mat> text_area_candidates(Mat image, int thresh = 170){
  }

  bool close(int v1, int v2, int thresh){
  }

  Rect combinex(vector<Rect> rects){
  }

  Rect syscombine(vector<Rect> rects){
  }

};