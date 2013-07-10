#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>

#include <boost/filesystem.hpp>
#include <cg/primitives/contour.h>

using namespace boost::filesystem;
using namespace std;

int main() {
    ofstream out("formated");
    for (int i = 0; i < 90; i++) {
        string s = to_string(i);
        path p("./tests/" + s);
        if (exists(p)) {
            cout << p << " " << file_size(p) << endl;
            cg::contour_2 cont;
            ifstream in(p.string());
            int n;
            in >> n;
            out << "TEST(triangulation, " << "custom_" << s << ") {" << endl;
            out << "    vector<contour_2> poly;" << endl;
            for (int i = 0; i < n; i++) {
                out << "    contour_2 cur" + to_string(i) + "({";
                int m;
                in >> m;
                for (int j = 0; j < m; j++) {
                    double x, y;
                    in >> x >> y;
                    if (j != 0) out << ", ";
                    out << "point_2(" << x << ", " << y << ")";
                }
                    out << "});" << endl;
                    out << "    poly.push_back(cur" + to_string(i) + ");" << endl;
            }
            out << "    vector<triangle_2> v = triangulate(poly);" << endl;
            out << "    check_triangulation(poly, v); " << endl;
            out << "}" << endl << endl;
        }


        }
}
