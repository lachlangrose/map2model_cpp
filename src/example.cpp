#include <iostream>
#include <stdexcept>

#include "file_utils.h"
#include "parameters_reader.h"
#include "topology_analyzer.h"

#include <pybind11/pybind11.h>

std::string runMap2Model() {

  std::string result = "";

  try {

    // Reading the parameters file.
    ConverterLib::Parameters par;
    const std::string parameter_lines = par.directRead(
        "Turner_Syncline/output", "Turner_Syncline/tmp/hams2_geol.csv",
        "Turner_Syncline/tmp/GEOS_GEOLOGY_LINEARSTRUCTURE_500K_GSD.csv",
        "Turner_Syncline/tmp/mindeps_2018.csv");

    result += "Successful parameter load\n";

    // // Create the output data folder.
    FileUtils::CreateDirectory(par.path_output.c_str());
    result += "Output directory created.\n";

    // Main calculations start here.
    ConverterLib::TopologyAnalyzer topo_analyzer;
    topo_analyzer.Initialize(par);

    if (par.subregion_size_x > 0 && par.subregion_size_y > 0) {
      topo_analyzer.AnalyzeLocalTopology(par);
    } else {
      topo_analyzer.AnalyzeGlobalTopology(par, parameter_lines);
    }

    result += "Calculations done.\n";

  } catch (const std::exception &e) {
    std::cerr << "Unexpected exception in main caught: " << e.what()
              << std::endl;
    result += e.what();
  }

  return result;
}

// std::string run() {
//   std::string result = runMap2Model(
//       "Turner_Syncline/output", "Turner_Syncline/tmp/hams2_geol.csv",
//       "Turner_Syncline/tmp/GEOS_GEOLOGY_LINEARSTRUCTURE_500K_GSD.csv",
//       "Turner_Syncline/tmp/mindeps_2018.csv");

//   return result;
// }

int add(int i, int j) { return i + j; }

// namespace py = pybind11;

// using CLP = ConverterLib::Parameters;

PYBIND11_MODULE(BOBS, m) {
  m.doc() = "pybind11 example plugin"; // optional module docstring

  m.def("add", &add, "A function which adds two numbers");
  m.def("run", &runMap2Model, "Test function that does nuthin.");
  // py::class_<TT>(m, "Test").def(py::init()).def("test", &TT::test);

  // py::class_<CLP>(m, "ConverterLibParameters")
  //     .def(py::init())
  //     .def("directRead", &CLP::directRead)
  //     .def_property_readonly("path_output", &CLP::path_output);

  // m.def("run", &run, "Run map2model.");
}