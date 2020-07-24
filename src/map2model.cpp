/******************************************************************************
 * A program for geological map topology analysis.
 *
 * Author: Vitaliy Ogarko, vogarko@gmail.com
 *******************************************************************************/

#include <iostream>
#include <stdexcept>

#include "file_utils.h"
#include "parameters_reader.h"
#include "tester.h"
#include "topology_analyzer.h"

#include <pybind11/pybind11.h>

const bool RUN_TESTS = false;

// Main entry point.
int main(int argc, char *argv[]) {
  if (RUN_TESTS) {
    ConverterLib::Test_FaultAndPolygonIntersecting();
    ConverterLib::Test_FaultsAreIntersecting();
    return 0;
  }

  try {
    // Retrieve parameters filename from a command line.
    std::string parfile_path;
    if (argc < 2) {
      std::cout << "Error: parameter file is not specified!" << std::endl;
      std::cout << "Usage: map2model <Parfile>" << std::endl;
      exit(0);
    } else {
      parfile_path = argv[1];
    }

    // Reading the parameters file.
    ConverterLib::Parameters par;
    const std::string parameter_lines = par.Read(parfile_path);

    // Create the output data folder.
    FileUtils::CreateDirectory(par.path_output.c_str());

    // Copy parameters file to the output folder (for reference).
    FileUtils::CopyFile(parfile_path.c_str(),
                        (par.path_output + "/Parfile").c_str());

    //----------------------------------------------------------------------------------
    // Main calculations start here.
    ConverterLib::TopologyAnalyzer topo_analyzer;
    topo_analyzer.Initialize(par);

    if (par.subregion_size_x > 0 && par.subregion_size_y > 0) {
      topo_analyzer.AnalyzeLocalTopology(par);
    } else {
      topo_analyzer.AnalyzeGlobalTopology(par, parameter_lines);
    }
  } catch (const std::exception &e) {
    std::cerr << "Unexpected exception in main caught: " << e.what()
              << std::endl;
  }

  std::cout << "End." << std::endl;
  return 0;
}

namespace py = pybind11;

std::string runMap2Model(const std::string output, const std::string geology,
                         const std::string faults, const std::string points,
                         py::dict bbox, py::dict config,
                         const std::string commodities) {

  std::string result = "";

  // for (auto item : config) {
  //   py::print(std::string(py::str(item.first)), ",",
  //             std::string(py::str(item.second)));
  // }

  // py::print(py::str(config["o"]));

  try {

    // Reading the parameters file.
    ConverterLib::Parameters par;
    const std::string parameter_lines = par.directRead(
        output, geology, faults, points, bbox, config, commodities);

    result += "Successful parameter load\n";

    // // Create the output data folder.
    FileUtils::CreateDirectory(par.path_output.c_str());
    result += "Output directory created at: " + output + "\n";

    // Main calculations start here.
    ConverterLib::TopologyAnalyzer topo_analyzer;
    topo_analyzer.Initialize(par);

    if (par.subregion_size_x > 0 && par.subregion_size_y > 0) {
      topo_analyzer.AnalyzeLocalTopology(par);
    } else {
      topo_analyzer.AnalyzeGlobalTopology(par, parameter_lines);
    }

    result += "Topology analysis complete.\n";

  } catch (const std::exception &e) {
    std::cerr << "Unexpected exception in main caught: " << e.what()
              << std::endl;
    result += e.what();
  }

  return result;
}

int add(int i, int j) { return i + j; }

// using CLP = ConverterLib::Parameters;

PYBIND11_MODULE(map2model, m) {
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
