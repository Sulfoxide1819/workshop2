#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

// Вычисление чувствительности
double sensitivity(double L, double Ueff) {
    return L / (2.0 * sqrt(2.0) * Ueff);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input.csv>\n";
        return 1;
    }

    string input_filename = argv[1];
    ifstream infile(input_filename);
    if (!infile.is_open()) {
        cerr << "Cannot open file: " << input_filename << endl;
        return 1;
    }

    // Создаём имя выходного файла: input.csv -> input_output.csv
    string output_filename = input_filename;
    size_t dot = output_filename.find_last_of('.');
    if (dot != string::npos) {
        output_filename.insert(dot, "_output");
    } else {
        output_filename += "_output.csv";
    }

    ofstream outfile(output_filename);
    if (!outfile.is_open()) {
        cerr << "Cannot create output file: " << output_filename << endl;
        return 1;
    }

    // Заголовок выходного файла
    outfile << "L_mm,Ueff_V,S_mm_per_V\n";

    string line;
    int line_num = 0;
    while (getline(infile, line)) {
        line_num++;
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        string L_str, U_str;

        if (!getline(ss, L_str, ',')) {
            cerr << "Skipping invalid line " << line_num << ": missing L\n";
            continue;
        }
        if (!getline(ss, U_str, ',')) {
            cerr << "Skipping invalid line " << line_num << ": missing U_eff\n";
            continue;
        }

        try {
            double L = stod(L_str);
            double U = stod(U_str);
            double S = sensitivity(L, U);

            outfile << fixed << setprecision(4);
            outfile << L << "," << U << "," << S << "\n";
        } catch (...) {
            cerr << "Skipping invalid line " << line_num << ": conversion error\n";
        }
    }
    

    infile.close();
    outfile.close();

    cout << "Processed " << input_filename << " -> " << output_filename << endl;
    return 0;
}
