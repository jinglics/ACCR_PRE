#include "preprocess.h"

int thresh = 100;
int max_thresh = 255;

Mat src, src_gray;
/// Function header
void thresh_callback(int, void*);

int main(){
  Mat image;
  int num = 5;
  for (int i = 0; i < num; i++){
    cout << i << endl;
    image = imread("b" + to_string(i + 1) + ".JPG");
    Preprocess inst(image);
    inst.process();
    vector<Mat> candidates = inst.text_area_candidates(inst.get_result());
    int j = 0;
    for (auto it : candidates){
      //cout << "B" + to_string(i + 1) + "\\" + to_string(++j) + ".JPG"<<endl;
      imwrite("B" + to_string(i + 1) + "\\" + to_string(++j) + ".JPG", it);
    }
    candidates = inst.text_area_candidates(inst.get_invert());
    j = 0;
    for (auto it : candidates){
      imwrite("B" + to_string(i + 1) + "\\" + to_string(++j) + "in.JPG", it);
    }
  }
  waitKey(0);
}