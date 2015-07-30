#include "preprocess.h"

int thresh = 100;
int max_thresh = 255;

Mat src, src_gray;





int main(int argc, char *argv[]){
  //cout << argv[1] << endl;
  //cout << argv[3] << endl;
  Mat image = imread(argv[2]);
  Preprocess inst(image);
  inst.process();

  if (strcmp(argv[1],"0")==0){
    inst.TextAreaCandidateOnY_draw(inst.get_result(),argv[3]);
  }
  else if (strcmp(argv[1], "1") == 0){
    inst.TextAreaCandidateOnY_draw(inst.get_invert(), argv[3]);
  }
  waitKey(0);
  return 1;
}











/*

  Mat image;
  int num = 5;
  for (int i = 0; i < num; i++){
    cout << i << endl;
    image = imread("b" + to_string(i + 1) + ".JPG");
    Preprocess inst(image);
    inst.process();
    vector<Mat> candidates = inst.TextAreaCandidateOnY(inst.get_result());
    int j = 0;
    for (auto it : candidates){
      //cout << "B" + to_string(i + 1) + "\\" + to_string(++j) + ".JPG"<<endl;
      imwrite("B" + to_string(i + 1) + "\\" + to_string(++j) + ".JPG", it);
    }
    candidates = inst.TextAreaCandidateOnY(inst.get_invert());
    j = 0;
    for (auto it : candidates){
      imwrite("B" + to_string(i + 1) + "\\" + to_string(++j) + "in.JPG", it);
    }
  }
  waitKey(0);
  
}
*/