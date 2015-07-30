#include "preprocess.h"
RNG rng(12345);

Preprocess::Preprocess(const Mat & im){
  source_image = im;
  result_image = im;
  invert_image = im;
}

Preprocess::~Preprocess(){
}

Mat Preprocess::get_source() const{
  return source_image;
}

Mat Preprocess::get_result() const{
  return result_image;
}

Mat Preprocess::get_invert() const{
  return invert_image;
}

void Preprocess::rgb2gray(){
  rgb2gray(result_image);
  rgb2gray(invert_image);
  //cvtColor(result_image, result_image, COLOR_RGB2GRAY);
  //cvtColor(invert_image, invert_image, COLOR_RGB2GRAY);
}

void Preprocess::rgb2gray(Mat& image, int const thresh){
  cvtColor(image, image, COLOR_RGB2GRAY);
}

void Preprocess::binarization(Mat& image, int const thresh){
  threshold(image, image, 180, 255, 3);
}

/*
Binarization: Choose the suitable threshold. Important
*/
void Preprocess::binarization(int const thresh){
  binarization(result_image, thresh);
  binarization(invert_image,thresh-60);
  /*
  threshold(result_image, result_image, thresh, 255, CV_THRESH_BINARY);
  threshold(invert_image, invert_image, thresh - 60, 255, CV_THRESH_BINARY);
  */
}

void Preprocess::gaussion_blur(int const size){
  gaussion_blur(result_image, size); 
  gaussion_blur(invert_image, size);
}

void Preprocess::gaussion_blur(Mat &image, int const size){
  GaussianBlur(image, image, Size(size, size), 0, 0);
}


void Preprocess::erosion(int const erosion_type, int const erosion_size){
  Mat element = getStructuringElement(erosion_type,
    Size(2 * erosion_size + 1, 2 * erosion_size + 1),
    Point(erosion_size, erosion_size));
  erode(result_image, result_image, element);
}

void Preprocess::dilation(int const dilation_type, int const dilation_size){
  Mat element = getStructuringElement(dilation_type,
    Size(2 * dilation_size + 1, 2 * dilation_size + 1),
    Point(dilation_size, dilation_size));
  dilate(result_image, result_image, element);
}

void Preprocess::generate_invert(){
  bitwise_not(result_image, invert_image);
}

void Preprocess::morphology(Mat& image, int const size){
  Mat element = getStructuringElement(0, Size(2 * size + 1, 2 * size + 1), Point(size, size));
  morphologyEx(image, image, 5, element);
}

bool Preprocess::background(){
  const int channels[1] = { 0 };
  const int histSize[1] = { 2 };
  float hranges[2] = { 0, 255 };
  const float* ranges[1] = { hranges };
  Mat hist;
  calcHist(&source_image, 1, channels, Mat(), hist, 1, histSize, ranges);
  if (hist.at<float>(0) > hist.at<float>(1)){
    return true;
  }
  return false;
}

/*
Set the thresholds for mophology. Important
Decide by the image color (code container color pattern)
*/
void Preprocess::process(){
  gaussion_blur(result_image);
  rgb2gray(result_image);
  generate_invert();

  //binarization();
  //imwrite("result.jpg", result_image);
  //imwrite("invert.jpg", invert_image);
  morphology(result_image, 15);
  imwrite("result.jpg",result_image);
  morphology(invert_image, 14);
  imwrite("invert.jpg", invert_image);

}

/*
All the Rectangle Area are cluster
*/
vector<vector<Rect> > Preprocess::ClusterRectOnY(vector<Rect> rects){

  //combine rects with same level asix y (vertical)
  //condition: 1. threshold diff on y,
  //           2. two far on axis x. distance large than 3 times of the rect width
  //           3. 
  vector<vector<Rect> > box_rects;
  vector<Rect> box;
  int compy1, compy2;
  int distance;
  std::sort(rects.begin(), rects.end(), [](const Rect& lhs, const Rect& rhs)
  {
    return lhs.y < rhs.y;
  });

  for (int i = 0; i < rects.size(); i++)
  {
    if (box.size() == 0){
      box.push_back(rects[i]);
      compy1 = rects[i].y;
      distance = rects[i].height;
    }
    else{
      compy2 = rects[i].y;
      /*
      threshold for the y difference (上下交错程度，对齐程度)？？？ 1/2 height
      */
      if (close(compy1, compy2, distance / 4)){
        box.push_back(rects[i]);
        compy1 = rects[i].y;
        distance = rects[i].height;
      }
      else{
        vector<Rect> cand_box = FilterRectsOnY(box);
        vector<vector<Rect> > cand_boxes = SplitRectsOnY(cand_box);
        box_rects.insert(box_rects.end(), cand_boxes.begin(), cand_boxes.end());
        box.clear();
        i--;
      }
    }
  }
  if (box.size() >= 0)
    box_rects.push_back(box);

  return box_rects;
}

vector<vector<Rect> > Preprocess::ClusterRectOnX(vector<Rect> rects){
  vector<vector<Rect> > box_rects;
  vector<Rect> box;
  int compx1, compx2;
  int distance;
  sort(rects.begin(), rects.end(), [](const Rect& lhs, const Rect& rhs)
  {
    return lhs.x < rhs.x;
  });
  for (int i = 0; i < rects.size(); i++){
    if (box.size() == 0){
      box.push_back(rects[i]);
      compx1 = rects[i].x;
      distance = rects[i].width;
    }
    else{
      compx2 = rects[i].x;
      if (close(compx1, compx2, distance / 2)){
        box.push_back(rects[i]);
        compx1 = rects[i].x;
        distance = rects[i].width;
      }
      else{
        vector<Rect> cand_box = FilterRectsOnX(box);
        vector<vector<Rect> > cand_boxes = SplitRectsOnX(cand_box);
        box_rects.insert(box_rects.end(), cand_boxes.begin(), cand_boxes.end());
        box.clear();
        i--;
      }
    }
  }
  if (box.size() >= 0)
    box_rects.push_back(box);
  return box_rects;
}

vector<Rect> Preprocess::FilterRectsOnY(vector<Rect> box){
  int height = 0;
  for (const auto rect : box){
    height += rect.height;
  }
  height /= box.size();
  vector<Rect> filter_box;
  for (const auto rect : box){
    /*
    threshold for the height consistency in a group (y-axis) 0.25-1.75 average
    */
    if (rect.height > 0.75*height && rect.height < 1.25*height)
      filter_box.push_back(rect);
  }
  return filter_box;
}

vector<Rect> Preprocess::FilterRectsOnX(vector<Rect> box){
  int width = 0;
  for (const auto rect : box){
    width += rect.width;
  }
  width /= box.size();
  vector<Rect> filter_box;
  for (const auto rect : box){
    /*
    threshold for the width consistency in a group (x-axis) 0.25-1.75 average
    */
    if (rect.height > 0.75*width && rect.height < 1.25*width)
      filter_box.push_back(rect);
  }
  return filter_box;
}

vector<vector<Rect> > Preprocess::SplitRectsOnY(vector<Rect> rects){
  vector<vector<Rect> > box_rects;
  vector<Rect> box;
  int compx1, compx2;
  int width;

  sort(rects.begin(), rects.end(), [](const Rect& lhs, const Rect& rhs)
  {
    return lhs.x < rhs.x;
  });

  for (int i = 0; i < rects.size(); i++)
  {
    if (box.size() == 0){
      box.push_back(rects[i]);
      compx1 = rects[i].x;
      width = rects[i].width;
    }
    else{
      compx2 = rects[i].x;
      /*
      Threshold for the distance between objects in x-axis
      */
      int max_width = max(width, rects[i].width);
      if (close(compx1, compx2, 5 * max_width)){
        box.push_back(rects[i]);
        compx1 = rects[i].x;
        width = rects[i].width;
      }
      else{
        box_rects.push_back(box);
        box.clear();
        i--;
      }
    }
  }
  if (box.size() >= 0)
    box_rects.push_back(box);
  return box_rects;
}

vector<vector<Rect> > Preprocess::SplitRectsOnX(vector<Rect> rects){
  vector<vector<Rect> > box_rects;
  vector<Rect> box;
  int compy1, compy2;
  int height;

  sort(rects.begin(), rects.end(), [](const Rect& lhs, const Rect& rhs)
  {
    return lhs.y < rhs.y;
  });

  for (int i = 0; i < rects.size(); i++)
  {
    if (box.size() == 0){
      box.push_back(rects[i]);
      compy1 = rects[i].y;
      height = rects[i].height;
    }
    else{
      compy2 = rects[i].y;
      /*
      Threshold for the distance between objects in x-axis
      */
      int max_height = max(height, rects[i].height);
      if (close(compy1, compy2, 3 * max_height)){
        box.push_back(rects[i]);
        compy1 = rects[i].y;
        height = rects[i].height;
      }
      else{
        box_rects.push_back(box);
        box.clear();
        i--;
      }
    }
  }
  if (box.size() >= 0)
    box_rects.push_back(box);
  return box_rects;
}

vector<Mat> Preprocess::TextAreaCandidateOnY(Mat image, int thresh){
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  // Detect edges using canny
  //threshold(result_image, canny_output, thresh, 255, THRESH_BINARY);
  Canny(image, canny_output, thresh, thresh * 2, 3);
  /// Find contours
  findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
  /// Approximate contours to polygons + get bounding rects and circles
  vector<Point> contours_poly;
  vector<Rect> boundRect;

  for (int i = 0; i < contours.size(); i++)
  {
    approxPolyDP(Mat(contours[i]), contours_poly, 3, true);
    Rect rect = boundingRect(Mat(contours_poly));
    //cout << rect.height << endl;
    /*
    threshold for size of the objects important !!!!!!!!!!
    */
    if (rect.height>40 && rect.height < 150){
      //if (rect.height>10){
      boundRect.push_back(rect);
    }
  }
  vector<vector<Rect> > box_rects = ClusterRectOnY(boundRect);
  vector<Mat> text_region;
  for (int i = 0; i < box_rects.size(); i++){
    Rect religion = CombineRects(box_rects[i]);
    //Rect religion = syscombine(box_rects[i]);
    if (religion.height>source_image.rows || religion.width>source_image.cols ||
      religion.x<1 || religion.y<1 || religion.x>source_image.cols || religion.y>source_image.rows)
      continue;
    text_region.push_back(Mat(source_image, religion));
  }

  sort(text_region.begin(), text_region.end(), [](const Mat& lhs, const Mat& rhs)
  {
    return lhs.cols > rhs.cols;
  });
  vector<Mat> candidates;
  int min = (3 < text_region.size() ? 3 : text_region.size());
  for (int i = 0; i < min; i++){
    candidates.push_back(text_region[i]);
  }
  return candidates;
}


void Preprocess::TextAreaCandidateOnY_draw(Mat image, char* filename, int thresh){
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  // Detect edges using canny
  //threshold(result_image, canny_output, thresh, 255, THRESH_BINARY);
  Canny(image, canny_output, thresh, thresh * 2, 3);
  /// Find contours
  findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
  /// Approximate contours to polygons + get bounding rects and circles
  vector<Point> contours_poly;
  vector<Rect> boundRect;

  for (int i = 0; i < contours.size(); i++)
  {
    approxPolyDP(Mat(contours[i]), contours_poly, 3, true);
    Rect rect = boundingRect(Mat(contours_poly));
    //cout << rect.height << endl;
    /*
    threshold for size of the objects important !!!!!!!!!!
    */
    if (rect.height>40 && rect.height < 150){
      //if (rect.height>10){
      boundRect.push_back(rect);
    }
  }
  vector<vector<Rect> > box_rects = ClusterRectOnY(boundRect);
  vector<Rect> text_region;
  for (int i = 0; i < box_rects.size(); i++){
    Rect religion = CombineRects(box_rects[i]);
    //Rect religion = syscombine(box_rects[i]);
    if (religion.height>source_image.rows || religion.width>source_image.cols ||
      religion.x<1 || religion.y<1 || religion.x>source_image.cols || religion.y>source_image.rows)
      continue;
    text_region.push_back(religion);
  }

  sort(text_region.begin(), text_region.end(), [](const Rect& lhs, const Rect& rhs)
  {
    return lhs.width > rhs.width;
  });

  //vector<Mat> candidates;
  int min = (3 < text_region.size() ? 3 : text_region.size());
  for (int i = 0; i < min; i++){
    Scalar color = Scalar(0, 255, 255);// rng.uniform(0, 255), rng.uniform(0, 255));
    rectangle(source_image, text_region[i].tl(), text_region[i].br(), color, 5, 8, 0);
  }
  namedWindow("Contours", CV_WINDOW_NORMAL);
  imshow("Contours", source_image);
  //cout << filename << endl;
  imwrite(filename, source_image);
  
}

vector<Mat> Preprocess::TextAreaCandidateOnX(Mat image, int thresh){
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  Canny(image, canny_output, thresh, thresh * 2, 3);
  findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

  vector<Point> contours_poly;
  vector<Rect> boundRect;

  for (int i = 0; i < contours.size(); i++)
  {
    approxPolyDP(Mat(contours[i]), contours_poly, 3, true);
    Rect rect = boundingRect(Mat(contours_poly));
    /*
    threshold for size of the objects important !!!!!!!!!!
    */
    if (rect.height>40 && rect.height < 150){
      //if (rect.height>10){
      boundRect.push_back(rect);
    }
  }
  vector<vector<Rect> > box_rects = ClusterRectOnX(boundRect);
  vector<Mat> text_region;
  for (int i = 0; i < box_rects.size(); i++){
    Rect religion = CombineRects(box_rects[i]);
    //Rect religion = syscombine(box_rects[i]);
    if (religion.height>source_image.rows || religion.width>source_image.cols ||
      religion.x<1 || religion.y<1 || religion.x>source_image.cols || religion.y>source_image.rows)
      continue;
    text_region.push_back(Mat(source_image, religion));
  }

  sort(text_region.begin(), text_region.end(), [](const Mat& lhs, const Mat& rhs)
  {
    return lhs.cols > rhs.cols;
  });
  vector<Mat> candidates;
  int min = (3 < text_region.size() ? 3 : text_region.size());
  for (int i = 0; i < min; i++){
    candidates.push_back(text_region[i]);
  }
  return candidates;

}

bool Preprocess::close(int v1, int v2, int thresh){
  if (abs(v1 - v2) < thresh)
    return true;
  return false;
}

Rect Preprocess::CombineRects(vector<Rect> rects){
  int minx = INT_MAX, miny = INT_MAX, maxx = -1, maxy = -1;
  for (const auto& rect : rects){
    if (minx > rect.tl().x)
      minx = rect.tl().x;
    if (miny>rect.tl().y)
      miny = rect.tl().y;
    if (maxx < rect.br().x)
      maxx = rect.br().x;
    if (maxy < rect.br().y)
      maxy = rect.br().y;
  }
  return Rect(Point(minx, miny), Point(maxx, maxy));
}

Rect Preprocess::CombineRectsOpencv(vector<Rect> rects){
  vector<Point> points;
  for (auto rect : rects){
    points.push_back(rect.tl());
    points.push_back(rect.br());
  }
  RotatedRect rectbox = minAreaRect(points);
  return rectbox.boundingRect();
}