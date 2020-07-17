/******************************************************************************
 * Parameter reader.
 *
 * Author: Vitaliy Ogarko, vogarko@gmail.com
 *******************************************************************************/

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include "parameters_reader.h"

namespace ConverterLib {

//! Extract a value after '=' symbol.
static std::string GetValue(std::string str) {
  return str.substr(str.find('=') + 1, str.length());
}

//! Extract a value before '=' symbol.
static std::string GetDescription(std::string str) {
  return str.substr(0, str.find('=') + 1);
}

//! Split string into substrings: split("1,2,3",",") ==> ["1","2","3"]
std::vector<std::string> split(std::string str, std::string token) {
  std::vector<std::string> result;
  while (str.size()) {
    size_t index = str.find(token);
    if (index != std::string::npos) {
      result.push_back(str.substr(0, index));
      str = str.substr(index + token.size());
      if (str.size() == 0)
        result.push_back(str);
    } else {
      result.push_back(str);
      str = "";
    }
  }
  return result;
}

namespace py = pybind11;
// ? Reading input params directly from main
std::string Parameters::directRead(const std::string output,
                                   const std::string geology,
                                   const std::string faults,
                                   const std::string points, py::dict bbox,
                                   py::dict config,
                                   const std::string commodities) {

  //--- COLUMN NAMES IN CSV DATA FILES:
  //-------------------------------------------------------------
  constNames["FIELD_COORDINATES"] = "WKT";

  constNames["FIELD_FAULT_ID"] = py::str(config["o"]);

  constNames["FIELD_FAULT_FEATURE"] = py::str(config["f"]);

  constNames["FIELD_POLYGON_ID"] = py::str(config["o"]);

  constNames["FIELD_POLYGON_LEVEL1_NAME"] = py::str(config["u"]);

  constNames["FIELD_POLYGON_LEVEL2_NAME"] = py::str(config["g"]);

  constNames["FIELD_POLYGON_MIN_AGE"] = py::str(config["min"]);

  constNames["FIELD_POLYGON_MAX_AGE"] = py::str(config["max"]);

  constNames["FIELD_POLYGON_CODE"] = py::str(config["c"]);

  constNames["FIELD_POLYGON_DESCRIPTION"] = py::str(config["ds"]);

  constNames["FIELD_POLYGON_ROCKTYPE1"] = py::str(config["r1"]);

  constNames["FIELD_POLYGON_ROCKTYPE2"] = py::str(config["r2"]);

  constNames["FIELD_SITE_CODE"] = py::str(config["msc"]);

  constNames["FIELD_SITE_TYPE"] = py::str(config["mst"]);

  constNames["FIELD_SITE_COMMO"] = py::str(config["mscm"]);

  //--- SOME CONSTANTS:
  //----------------------------------------------------------------------------
  constNames["FAULT_AXIAL_FEATURE_NAME"] = py::str(config["fold"]);

  constNames["SILL_STRING"] = py::str(config["sill"]);

  constNames["IGNEOUS_STRING"] = py::str(config["intrusive"]);

  constNames["VOLCANIC_STRING"] = py::str(config["volcanic"]);

  constNames["IGNORE_DEPOSITS_SITE_TYPE"] = "Infrastructure";

  angleEpsilon = 1.0;
  distanceEpsilon = 15.0;
  faultFaultDistanceBuffer = 20.0;
  pointToContactDistanceBuffer = std::stod(py::str(config["deposit_dist"]));
  intersectPolygonsDistanceBuffer = 3.0;

  //--- PATHS:
  //-------------------------------------------------------------------------------------
  path_output = output;
  path_geology = geology;
  path_faults = faults;
  path_points = points;

  //------------------------------------------------------------------------------------------------
  clipping_window[0] = std::stod(py::str(bbox["minx"]));
  clipping_window[1] = std::stod(py::str(bbox["miny"]));
  clipping_window[2] = std::stod(py::str(bbox["maxx"]));
  clipping_window[3] = std::stod(py::str(bbox["maxy"]));

  graph_edge_width_categories.push_back(2000.0);
  graph_edge_width_categories.push_back(20000.0);
  graph_edge_width_categories.push_back(200000.0);

  graph_edge_direction_type = 2;

  depositListForGraphInfo = split(commodities, ",");

  partial_graph_polygon_id = 32;

  partial_graph_depth = 4;

  subregion_size_x = 0.0;
  subregion_size_y = 0.0;

  //-------------------------------------------------------------------------------
  // Return to the beginning of the file.
  // infile.clear();
  // infile.seekg(0, std::ios::beg);

  // Store all input parameter lines.
  std::string parameter_lines = "";
  // while (std::getline(infile, line)) {
  //   parameter_lines += "# " + line + '\n';
  // }

  std::cerr << parameter_lines << '\n';
  return parameter_lines;
}

//! Reading input parameters from a file.
std::string Parameters::Read(const std::string &filename) {
  std::ifstream infile(filename.c_str());

  if (!infile.good()) {
    throw std::runtime_error("Error in opening the file: " + filename);
  } else {
    std::cout << "Reading data from the file: " << filename << std::endl;
  }

  std::string line;
  char buf[200], c;

  std::getline(infile, line);
  std::cout << line << std::endl;

  //--- COLUMN NAMES IN CSV DATA FILES:
  //-------------------------------------------------------------
  std::getline(infile, line);
  constNames["FIELD_COORDINATES"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_COORDINATES"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_FAULT_ID"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_FAULT_ID"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_FAULT_FEATURE"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_FAULT_FEATURE"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_POLYGON_ID"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_POLYGON_ID"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_POLYGON_LEVEL1_NAME"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_POLYGON_LEVEL1_NAME"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_POLYGON_LEVEL2_NAME"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_POLYGON_LEVEL2_NAME"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_POLYGON_MIN_AGE"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_POLYGON_MIN_AGE"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_POLYGON_MAX_AGE"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_POLYGON_MAX_AGE"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_POLYGON_CODE"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_POLYGON_CODE"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_POLYGON_DESCRIPTION"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_POLYGON_DESCRIPTION"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_POLYGON_ROCKTYPE1"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_POLYGON_ROCKTYPE1"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_POLYGON_ROCKTYPE2"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_POLYGON_ROCKTYPE2"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_SITE_CODE"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_SITE_CODE"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_SITE_TYPE"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_SITE_TYPE"]
            << std::endl;

  std::getline(infile, line);
  constNames["FIELD_SITE_COMMO"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FIELD_SITE_COMMO"]
            << std::endl;

  //--- SOME CONSTANTS:
  //----------------------------------------------------------------------------
  std::getline(infile, line);
  std::cout << line << std::endl;

  std::getline(infile, line);
  constNames["FAULT_AXIAL_FEATURE_NAME"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["FAULT_AXIAL_FEATURE_NAME"]
            << std::endl;

  std::getline(infile, line);
  constNames["SILL_STRING"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["SILL_STRING"] << std::endl;

  std::getline(infile, line);
  constNames["IGNEOUS_STRING"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["IGNEOUS_STRING"]
            << std::endl;

  std::getline(infile, line);
  constNames["VOLCANIC_STRING"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["VOLCANIC_STRING"]
            << std::endl;

  std::getline(infile, line);
  constNames["IGNORE_DEPOSITS_SITE_TYPE"] = GetValue(line);
  std::cout << GetDescription(line) << constNames["IGNORE_DEPOSITS_SITE_TYPE"]
            << std::endl;

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> angleEpsilon;
  std::cout << buf << c << angleEpsilon << std::endl;
  infile.get(c);

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> distanceEpsilon;
  std::cout << buf << c << distanceEpsilon << std::endl;
  infile.get(c);

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> faultFaultDistanceBuffer;
  std::cout << buf << c << faultFaultDistanceBuffer << std::endl;
  infile.get(c);

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> pointToContactDistanceBuffer;
  std::cout << buf << c << pointToContactDistanceBuffer << std::endl;
  infile.get(c);

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> intersectPolygonsDistanceBuffer;
  std::cout << buf << c << intersectPolygonsDistanceBuffer << std::endl;
  infile.get(c);

  //--- PATHS:
  //-------------------------------------------------------------------------------------
  std::getline(infile, line);
  std::cout << line << std::endl;

  std::getline(infile, line);
  path_output = GetValue(line);
  std::cout << GetDescription(line) << path_output << std::endl;

  std::getline(infile, line);
  path_geology = GetValue(line);
  std::cout << GetDescription(line) << path_geology << std::endl;

  std::getline(infile, line);
  path_faults = GetValue(line);
  std::cout << GetDescription(line) << path_faults << std::endl;

  std::getline(infile, line);
  path_points = GetValue(line);
  std::cout << GetDescription(line) << path_points << std::endl;

  //------------------------------------------------------------------------------------------------
  std::getline(infile, line);
  std::cout << line << std::endl;

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> clipping_window[0];
  infile >> clipping_window[1];
  infile >> clipping_window[2];
  infile >> clipping_window[3];
  std::cout << buf << c << clipping_window[0] << " " << clipping_window[1]
            << " " << clipping_window[2] << " " << clipping_window[3]
            << std::endl;
  infile.get(c);

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> minFractionInMixedContact;
  std::cout << buf << c << minFractionInMixedContact << std::endl;
  infile.get(c);

  double w[3];
  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> w[0];
  infile >> w[1];
  infile >> w[2];
  std::cout << buf << c << w[0] << " " << w[1] << " " << w[2] << std::endl;
  infile.get(c);

  graph_edge_width_categories.push_back(w[0]);
  graph_edge_width_categories.push_back(w[1]);
  graph_edge_width_categories.push_back(w[2]);

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> graph_edge_direction_type;
  std::cout << buf << c << graph_edge_direction_type << std::endl;
  infile.get(c);

  std::string depositsString;
  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> depositsString;
  std::cout << buf << c << depositsString << std::endl;
  infile.get(c);

  depositListForGraphInfo = split(depositsString, ",");

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> partial_graph_polygon_id;
  std::cout << buf << c << partial_graph_polygon_id << std::endl;
  infile.get(c);

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> partial_graph_depth;
  std::cout << buf << c << partial_graph_depth << std::endl;
  infile.get(c);

  infile.get(buf, 200, '=');
  infile.get(c);
  infile >> subregion_size_x;
  infile >> subregion_size_y;
  std::cout << buf << c << subregion_size_x << " " << subregion_size_y
            << std::endl;
  infile.get(c);

  std::getline(infile, line);
  std::cout << line << std::endl;

  //-------------------------------------------------------------------------------
  // Return to the beginning of the file.
  infile.clear();
  infile.seekg(0, std::ios::beg);

  // Store all input parameter lines.
  std::string parameter_lines = "";
  while (std::getline(infile, line)) {
    parameter_lines += "# " + line + '\n';
  }

  std::cerr << parameter_lines << '\n';
  return parameter_lines;
}

} // namespace ConverterLib
