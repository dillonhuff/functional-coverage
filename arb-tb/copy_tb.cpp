#include "nvhls_types.h"
#include "Arbiter.h"

#include <map>
#include <set>

using namespace std;

class CoverageModel {
public:
  map<string, int> eventCounts;

  void print(std::ostream& out) {
    out << "--- Coverage model results" << endl;
    for (auto e : eventCounts) {
      out << "  " << e.first << " happened " << e.second << " time(s)" << endl;
    }
  }

  void sample(bool shouldIncrement, const std::string& eventName) {
    if (eventCounts.find(eventName) == eventCounts.end()) {
      eventCounts[eventName] = 0;
    }

    if (shouldIncrement) {
      eventCounts[eventName] = eventCounts[eventName] + 1;
    }
  }
};

CoverageModel* covModel;

template<typename MaskType>
bool arb_mask_nonzero(const MaskType& val) {
  return val != 0;
}

#define SAMPLE(test, eventName, call) (call); (covModel->sample((test), (eventName)));

void testNoInputs() {
  Arbiter<4> arb;
  Arbiter<4>::Mask m = 0;
  Arbiter<4>::Mask res = SAMPLE(arb_mask_nonzero(m), "arb_mask_nonzero_tb.cpp:43:26", arb.pick(m));

  assert(res == 0);
}

int main() {
  CoverageModel cm;
  covModel = &cm;
  
  testNoInputs();
  cout << "All tests passed" << endl;

  covModel->print(cout);
}
