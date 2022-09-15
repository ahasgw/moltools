#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <GraphMol/MolStandardize/MolStandardize.h>
#include <GraphMol/SanitException.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <RDGeneral/RDLog.h>

namespace {

void super_parent(const std::string &fname, bool quiet) {
  using namespace std;
  using namespace RDKit;

  ifstream ifs((fname == "-") ? "/dev/stdin" : fname.c_str());
#pragma omp parallel
#pragma omp single nowait
  for (string line; getline(ifs, line);) {
#pragma omp task firstprivate(line)
    try {
      string smi;
      istringstream iss(line);
      iss >> smi;
      unique_ptr<RWMol> mol(SmilesToMol(smi));
      if (mol) {
        unique_ptr<RWMol> supmol(MolStandardize::superParent(*mol));
        smi = MolToSmiles(*supmol);
        if (!smi.empty()) {
          string rest;
          getline(iss, rest);
#pragma omp critical
          cout << smi << rest << endl;
        }
      }
      else {
        if (!quiet)
#pragma omp critical
          cerr << "Illegal SMILES: " << smi << endl;
      }
    }
    catch (ValueErrorException &e) {
      if (!quiet)
#pragma omp critical
        cerr << "ValueErrorException: " << e.what() << endl;
    }
    catch (SmilesParseException &e) {
      if (!quiet)
#pragma omp critical
        cerr << "SmilesParseException" << endl;
    }
    catch (MolSanitizeException &e) {
      if (!quiet)
#pragma omp critical
        cerr << "MolSanitizeException" << endl;
    }
  } // end of omp parallel
}

} // namespace

int main(int argc, char *argv[]) {
  using namespace std;
  using namespace RDKit;

  cin .sync_with_stdio(false);
  cout.sync_with_stdio(false);
  cerr.sync_with_stdio(false);
  clog.sync_with_stdio(false);

  bool quiet = true;

  if (quiet) boost::logging::disable_logs("rdApp.*");

  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      super_parent(argv[i], quiet);
    }
  } else {
    super_parent("/dev/stdin", quiet);
  }
  return EXIT_SUCCESS;
}
