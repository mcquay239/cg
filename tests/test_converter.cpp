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
    for (int i = 1; i < 90; i++) {
        string s = to_string(i);
        if (s.size() == 1) s = "0" + s;
        path p("./tests/" + s);
        if (exists(p)) {
            cout << p << " " << file_size(p) << endl;
            cg::contour_2 cont;
            ifstream in(p.string());
            int n;
            in >> n;
            out << "TEST(triangulation, " << "from_kingdom_subdivision_" << s << ") {" << endl;
            out << "    contour_2 outer ({ ";

            for (int i = 0; i < n; i++) {
                double x, y;
                in >> x >> y;
                cont.add_point(cg::point_2(x, y));
                if (i != 0) out << ", ";
                out << "point_2(" << x << ", " << y << ")";
            }
            out << "});";
            out << "    auto poly = {outer};" << endl;
            out << "    vector<triangle_2> v = triangulate(poly);" << endl;
            out << "    check_triangulation(poly, v); " << endl;
            out << "}" << endl << endl;
        }


        }
}
