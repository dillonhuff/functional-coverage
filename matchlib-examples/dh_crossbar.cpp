#include "crossbar.h"

// To run: clang++ -std=c++11 dh_crossbar.cpp -I/Users/dillon/CppWorkspace/systemc-2.3/include/ -L/Users/dillon/CppWorkspace/systemc-2.3/lib-macosx386 -lsystemc -lm -I.

void dh_crossbar(int inputs[4],
                 bool valid_in[4],
                 NVUINTW(nvhls::index_width<4>::val) source[4],
                 bool valid_source[4],
                 int data_out[4],
                 bool valid_out[4]) {
  crossbar<int, 4, 4>(inputs, valid_in, source, valid_source, data_out, valid_out);
}

int main() {
  int inputs[4];
  bool valid_in[4];
  NVUINTW(nvhls::index_width<4>::val) source[4];
  NVUINTW(nvhls::index_width<245>::val) rand[4];
  bool valid_source[4];
  int data_out[4];
  bool valid_out[4];

  dh_crossbar(inputs, valid_in, source, valid_source, data_out, valid_out);
  //crossbar<int, 4, 4>(inputs, valid_in, source, valid_source, data_out, valid_out);

  for (int i = 0; i < 10; i++) {
    rand[i] = rand[i] + 1;
  }
}
