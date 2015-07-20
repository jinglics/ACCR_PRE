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
    source_image = im;
    result_image = im;
    invert_image = im;
  }
  ~Preprocess(){

  }

  Mat get_source() const{
    return source_image;
  }

  Mat get_result() const{
    return result_image;
  }

  Mat get_invert() const{
    return invert_image;
  }

  void rgb2gray(){
    cvtColor(result_image, result_image, COLOR_RGB2GRAY);
    cvtColor(invert_image, invert_image, COLOR_RGB2GRAY);
  }
  void rgb2gray(Mat& image){
    cvtColor(image, image, COLOR_RGB2GRAY);
  }

  void binarization(Mat& image){
    threshold(image, image, 180, 255, 3);
  }

  /*
  threshold for binarization
  */
  void binarization(int thresh = 180){
    threshold(result_image, result_image, thresh, 255, CV_THRESH_BINARY);
    threshold(invert_image, invert_image, thresh - 60, 255, CV_THRESH_BINARY);
  }

  void gaussion_blur(int size = 5){
    GaussianBlur(result_image, result_image, Size(size, size), 0, 0);
    GaussianBlur(invert_image, invert_image, Size(size, size), 0, 0);
  }
  void gaussion_blur(Mat &image, int size = 5){
    GaussianBlur(image, image, Size(size, size), 0, 0);
  }

  // 0, 3 for B
  void erosion(int erosion_type = 0, int erosion_size = 3){
    Mat element = getStructuringElement(erosion_type,
      Size(2 * erosion_size + 1, 2 * erosion_size + 1),
      Point(erosion_size, erosion_size));
    erode(result_image, result_image, element);
  }

  void dilation(int dilation_type, int dilation_size){
    Mat element = getStructuringElement(dilation_type,
      Size(2 * dilation_size + 1, 2 * dilation_size + 1),
      Point(dilation_size, dilation_size));
    dilate(result_image, result_image, element);
  }
  void generate_invert(){
    bitwise_not(result_image, invert_image);
    //binarization(invert_image);
  }

  void morphology(Mat& image, int size = 5){
    Mat element = getStructuringElement(0, Size(2 * size + 1, 2 * size + 1), Point(size, size));
    morphologyEx(image, image, 5, element);
  }

  bool background(){
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

  //void eliminate_small_contour(Mat image, int thresh = 170){
  //  Mat canny_output;
  //  vector<vector<Point> > contours;
  //  vector<Vec4i> hierarchy;
  //  // Detect edges using canny
  //  //threshold(result_image, canny_output, thresh, 255, THRESH_BINARY);
  //  Canny(image, canny_output, thresh, thresh * 2, 3);
  //  /// Find contours
  //  findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
  //
  //  /// Approximate contours to polygons + get bounding rects and circles
  //  vector<vector<Point> > contours_poly(contours.size());
  //  vector<Rect> boundRect;
  //  vector<int> boundRectArea;
  //  for (int i = 0; i < contours.size(); i++)
  //  {
  //    approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
  //    Rect rect = boundingRect(Mat(contours_poly[i]));
  //    cout << rect.height << endl;
  //    if (rect.height>40 && rect.height < 150){
  //      boundRect.push_back(rect);
  //    }
  //  }
  //  std::sort(boundRect.begin(), boundRect.end(), [](const Rect& lhs, const Rect& rhs)
  //  {
  //    return lhs.y < rhs.y;
  //  });
  //  vector<vector<Rect> > box_points;
  //  vector<Rect> box;
  //  int compy1, compy2;
  //  //Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
  //  for (int i = 0; i< boundRect.size(); i++)
  //  {
  //    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
  //    drawContours(source_image, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
  //    rectangle(source_image, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
  //    //circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
  //    if (box.size()==0){
  //      //box.push_back(boundRect[i].tl()); 
  //      //box.push_back(boundRect[i].br());
  //      box.push_back(boundRect[i]);
  //      compy1 = boundRect[i].y;
  //    }
  //    else{
  //      compy2 = boundRect[i].y;
  //      if (close(compy1, compy2, boundRect[i].height/2)){ ////threshold
  //        /*box.push_back(boundRect[i].tl());
  //        box.push_back(boundRect[i].br());*/
  //        box.push_back(boundRect[i]);
  //      }
  //      else{
  //        box_points.push_back(box);
  //        box.clear();
  //        i--;
  //      }
  //    }
  //  }
  //  if (box.size()>=0) 
  //    box_points.push_back(box);
  //  namedWindow("Contour", CV_WINDOW_AUTOSIZE);
  //  imshow("Contour", source_image);
  //  vector<Mat> text_religon;
  //  for (int i = 0; i < box_points.size(); i++){
  //    //Rect religion = combine(box_points[i]);
  //    Rect religion = syscombine(box_points[i]);
  //    //RotatedRect rectbox = minAreaRect(box_points[i]);
  //    //Point2f vertices[4];
  //    //rectbox.points(vertices);
  //    //text_religon.push_back(Mat(result_image, Rect(vertices[0], vertices[2])));
  //    if (religion.height>source_image.rows || religion.width>source_image.cols || religion.x<1 || religion.y<1 || religion.x>source_image.cols || religion.y>source_image.rows) continue;
  //    text_religon.push_back(Mat(result_image, religion));
  //    //namedWindow("Religion" + i, CV_WINDOW_AUTOSIZE);
  //    //imshow("Religion" + i, text_religon[i]);
  //    //for (int i = 0; i < 4; ++i)
  //    //{
  //    //  line(result_image, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 0, 0), 1, CV_AA);
  //    //}
  //  }

  /*
  important thresholds for mophology
  how to set in different cases????
  */
  void process(){
    gaussion_blur();
    rgb2gray();
    generate_invert();
    //rgb2gray(invert_image);
    //rgb2gray();

    binarization();
    morphology(result_image, 15);
    morphology(invert_image, 14);
    ////gaussion_blur();
    //if (background()){
    //  morphology(result_image, 14);
    //  morphology(invert_image, 15);
    //}
    //else{
    //  cout << "Here" << endl;
    //  morphology(result_image, 15);
    //  morphology(invert_image, 5);
    //}
    imwrite("bw1.jpg", result_image);
    imwrite("bw2.jpg", invert_image);
  }

  vector<vector<Rect> > combine_rect(vector<Rect> rects){
    //sort the rects with axis: y
    std::sort(rects.begin(), rects.end(), [](const Rect& lhs, const Rect& rhs)
    {
      return lhs.y < rhs.y;
    });

    //combine rects with same level asix y (vertical)
    //condition: 1. threshold diff on y,
    //           2. two far on axis x. distance large than 3 times of the rect width
    //           3. 
    vector<vector<Rect> > box_rects;
    vector<Rect> box;
    int compy1, compy2;// , compx1, compx2;

    Mat drawing = Mat::zeros(source_image.size(), CV_8UC3);

    for (int i = 0; i < rects.size(); i++) //rects.size()
    {
      //cout << rects[i].y << endl;
      //if (rects[i].y <= 260 && rects[i].y >= 240){
      Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
      rectangle(drawing, rects[i].tl(), rects[i].br(), color, 2, 8, 0);
      //}
    }
    namedWindow("Contour", CV_WINDOW_AUTOSIZE);// NORMAL);
    imshow("Contour", drawing);
    for (int i = 0; i < rects.size(); i++)
    {
      if (i == 13){
        i = i;
      }
      if (box.size() == 0){
        box.push_back(rects[i]);
        compy1 = rects[i].y;
      }
      else{
        compy2 = rects[i].y;
        /*
        threshold for the y difference (上下交错程度，对齐程度)？？？ 1/2 height
        */
        if (close(compy1, compy2, rects[i].height / 4)){
          box.push_back(rects[i]);
          compy1 = rects[i].y;
        }
        else{
          vector<Rect> cand_box = filter_rects(box);
          vector<vector<Rect> > cand_boxes = split_rects(cand_box);
          box_rects.insert(box_rects.end(), cand_boxes.begin(), cand_boxes.end());
          //box_rects.push_back(filter_rects(box));
          box.clear();
          i--;
        }
      }
    }
    if (box.size() >= 0)
      box_rects.push_back(box);

    return box_rects;
  }
  vector<Rect> filter_rects(vector<Rect> box){
    int height = 0;
    for (const auto rect : box){
      height += rect.height;
    }
    height /= box.size();
    vector<Rect> filt_box;
    for (const auto rect : box){
      /*
      threshold for the height consistency in a group (y-axis) 0.25-1.75 average
      */
      if (rect.height>0.75*height && rect.height<1.25*height)
        filt_box.push_back(rect);
    }
    return filt_box;
  }

  vector<vector<Rect> > split_rects(vector<Rect> rects){
    sort(rects.begin(), rects.end(), [](const Rect& lhs, const Rect& rhs)
    {
      return lhs.x < rhs.x;
    });
    vector<vector<Rect> > box_rects;
    vector<Rect> box;
    int compx1, compx2;
    int width;
    for (int i = 0; i< rects.size(); i++)
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



  void religion_filter(Rect text_religon){


  }
  void position_filter(Rect text_religon){

  }

  vector<Mat> text_area_candidates(Mat image, int thresh = 170){
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    // Detect edges using canny
    //threshold(result_image, canny_output, thresh, 255, THRESH_BINARY);
    Canny(image, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect;
    vector<int> boundRectArea;

    for (int i = 0; i < contours.size(); i++)
    {
      approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
      Rect rect = boundingRect(Mat(contours_poly[i]));
      //cout << rect.height << endl;
      /*
      threshold for size of the objects important !!!!!!!!!!
      */
      if (rect.height>40 && rect.height < 150){
        //if (rect.height>10){
        boundRect.push_back(rect);
      }
    }
    //std::sort(boundRect.begin(), boundRect.end(), [](const Rect& lhs, const Rect& rhs)
    //{
    //  return lhs.y < rhs.y;
    //});
    //vector<vector<Rect> > box_points;
    //vector<Rect> box;
    //int compy1, compy2;
    ////Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
    //for (int i = 0; i< boundRect.size(); i++)
    //{
    //  Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    //  drawContours(source_image, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
    //  rectangle(source_image, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
    //  //circle(drawing, center[i], (int)radius[i], color, 2, 8, 0);
    //  if (box.size() == 0){
    //    //box.push_back(boundRect[i].tl()); 
    //    //box.push_back(boundRect[i].br());
    //    box.push_back(boundRect[i]);
    //    compy1 = boundRect[i].y;
    //  }
    //  else{
    //    compy2 = boundRect[i].y;
    //    if (close(compy1, compy2, boundRect[i].height / 2)){ ////threshold
    //      /*box.push_back(boundRect[i].tl());
    //      box.push_back(boundRect[i].br());*/
    //      box.push_back(boundRect[i]);
    //    }
    //    else{
    //      box_points.push_back(box);
    //      box.clear();
    //      i--;
    //    }
    //  }
    //}
    //if (box.size() >= 0)
    //  box_points.push_back(box);
    //namedWindow("Contour", CV_WINDOW_AUTOSIZE);
    //imshow("Contour", source_image);

    vector<vector<Rect> > box_rects = combine_rect(boundRect);
    vector<Mat> text_religon;
    for (int i = 0; i < box_rects.size(); i++){
      Rect religion = combinex(box_rects[i]);
      //Rect religion = syscombine(box_rects[i]);
      if (religion.height>source_image.rows || religion.width>source_image.cols ||
        religion.x<1 || religion.y<1 || religion.x>source_image.cols || religion.y>source_image.rows)
        continue;
      text_religon.push_back(Mat(source_image, religion));
    }

    sort(text_religon.begin(), text_religon.end(), [](const Mat& lhs, const Mat& rhs)
    {
      return lhs.cols > rhs.cols;
    });
    vector<Mat> candidates;
    int min = (3 < text_religon.size() ? 3 : text_religon.size());
    for (int i = 0; i < min; i++){
      candidates.push_back(text_religon[i]);
      //namedWindow("Religion" + to_string(i), CV_WINDOW_AUTOSIZE);
      //imshow("Religion" + to_string(i), text_religon[i]);
    }
    return candidates;
  }

  bool close(int v1, int v2, int thresh){
    if (abs(v1 - v2) < thresh)
      return true;
    return false;
  }

  Rect combinex(vector<Rect> rects){
    int minx = INT_MAX, miny = INT_MAX, maxx = -1, maxy = -1;
    for (const auto& rect : rects){
      if (minx>rect.tl().x)
        minx = rect.tl().x;
      if (miny>rect.tl().y)
        miny = rect.tl().y;
      if (maxx<rect.br().x)
        maxx = rect.br().x;
      if (maxy<rect.br().y)
        maxy = rect.br().y;
    }
    return Rect(Point(minx, miny), Point(maxx, maxy));
  }

  Rect syscombine(vector<Rect> rects){
    vector<Point> points;
    for (auto rect : rects){
      points.push_back(rect.tl());
      points.push_back(rect.br());
    }
    RotatedRect rectbox = minAreaRect(points);
    return rectbox.boundingRect();
  }

};