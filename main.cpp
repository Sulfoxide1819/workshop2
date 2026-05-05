#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <iomanip>


double mean(const std::vector<double>& data);
std::vector<double> dev(const std::vector<double>& data);
std::vector<double> square(const std::vector<double>& data);
double stddev(const std::vector<double>& data);
double device_error(double period, double mean, double gamma_0, unsigned n);
void write_deviations(const std::vector<double>& values,
                      const std::vector<double>& devs,
                      const std::vector<double>& sq_devs,
                      const std::string& filename);
void write_histogram(const std::vector<double>& values,
                     const std::string& filename,
                     int num_bins = 0);

int main(int argc, char* argv[]){
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " as <csv-file>\n";
    return 1;
  }

  std::ifstream file(argv[1]);
    if (!file.is_open()) {
      std::cerr << "Cannot open file: " << argv[1] << "\n";
      return 1;
    } 
  std::vector<double> L_
  std::vector<double> U_;
  std::string line;

  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;

    std::stringstream ss(line);
    std::string id_str, val_str;

    if (!std::getline(ss, id_str, ',')) continue;
    if (!std::getline(ss, val_str, ',')) continue;
    try {
            double l_val = std::stod(id_str);
            L_.push_back(l_val);
            double u_val = std::stod(val_str);
            U_.push_back(u_val);
        } catch (...) {
            std::cerr << "Skipping invalid line: " << line << "\n";
        }
  }

  double mean_val = mean(values);
  double d_error = device_error(EXACT, mean_val, GAMMA_0, 0);
  std::vector<double> dev_vec = dev(values);
  std::vector<double> sq_dev_vec = square(dev_vec);
  double stddev_val = stddev(sq_dev_vec);
  double m_error = stddev_val / std::sqrt(values.size());

  write_deviations(values, dev_vec, sq_dev_vec, "deviations.csv");
  write_histogram(values, "histogram.csv", BIN_NUM);
  std::cout << "=== Result ===" << "\n";
  std::cout << "Device error: " << d_error << "\n";
  std::cout << "Measurement error: " << m_error << "\n";
  std::cout << "==============" << "\n";
  std::cout << "Mean: " << mean_val << "\n";
  std::cout << "Variance: " << stddev_val << "\n";
  std::cout << "Files created: deviations.csv, histogram.csv\n";


  return 0;
}

//arithmetic mean
double mean(const std::vector<double>& data){
  double sum = 0;
  for(const double& value : data){
    sum += value;
  }
  return sum / static_cast<double>(data.size());
}

//period: rough - 1e-4
//        exact - 1e-6
//ms
double device_error(double period, double mean, double gamma_0, unsigned n){
  return gamma_0 * mean + period * 1e3 /* / std::pow(10, n)*/;

}


//deviations
std::vector<double> dev(const std::vector<double>& data){
  std::vector<double> dev_vec; //vector of deviations
  double m = mean(data);
  for(const double& value : data){
    dev_vec.push_back((value - m));
  }
  return dev_vec;
}

//vector of square's
std::vector<double> square(const std::vector<double>& data){
  std::vector<double> sq_vec; //vector of squared elements
  for(const double& value : data){
    sq_vec.push_back((value * value));
  }
  return sq_vec;
}

double stddev(const std::vector<double>& data){
  size_t size = data.size();
  if(size <= 1) return 0;
  double sum = 0;
  for(const double& value : data){
    sum += value;
  }
  return std::sqrt(sum / (size - 1));
}

void write_deviations(const std::vector<double>& values,
                      const std::vector<double>& devs,
                      const std::vector<double>& sq_devs,
                      const std::string& filename) {
  std::ofstream out(filename);
  if (!out.is_open()) {
    std::cerr << "Cannot create file: " << filename << "\n";
    return;
  }
  out << "index,value,deviation,square_dev\n";
  for (size_t i = 0; i < values.size(); ++i) {
    out << i+1 << "," << values[i] << "," << devs[i] << "," << sq_devs[i] << "\n";
  }
  out.close();
}

void write_histogram(const std::vector<double>& values,
                     const std::string& filename,
                     int num_bins) {
  if (values.empty()) return;

  double min_val = std::floor(*std::min_element(values.begin(), values.end()) * 10)/ 10.0;
  double max_val = std::ceil(*std::max_element(values.begin(), values.end())*10)/ 10.0;

  // Если все значения одинаковы — один интервал
  if (max_val == min_val) {
    num_bins = 1;
  } else if (num_bins <= 0) {
    // Формула Стерджесса: k = 1 + log2(N)
    num_bins = static_cast<int>(std::ceil(1 + std::log2(values.size())));
  }

  double bin_width = (max_val - min_val) / num_bins;

  std::vector<int> counts(num_bins, 0);

  for (double v : values) {
    int idx = static_cast<int>((v - min_val) / bin_width);
    if (idx < 0) idx = 0;
    if (idx >= num_bins) idx = num_bins - 1; // для v == max_val
    counts[idx]++;
  }

  std::ofstream out(filename);
  if (!out.is_open()) {
    std::cerr << "Cannot create file: " << filename << "\n";
    return;
  }

  out << "left,right,frequency\n";
  for (int i = 0; i < num_bins; ++i) {
    double left = min_val + i * bin_width;
    double right = (i == num_bins - 1) ? max_val : left + bin_width;
    out << left << "," << right << "," << counts[i] << "\n";
  }
  out.close();
}
