#include "nvhls_types.h"
#include "Arbiter.h"

#include <map>
#include <set>

using namespace std;

class CoverageModel {
public:
  map<string, int> eventCounts;
  set<string> requiredEvents;
};

CoverageModel* covModel;

void testNoInputs() {
  Arbiter<4> arb;
  Arbiter<4>::Mask m = 0;
  Arbiter<4>::Mask res = arb.pick(m);

  assert(res == 0);
}

int main() {
  testNoInputs();
  cout << "All tests passed" << endl;
}
