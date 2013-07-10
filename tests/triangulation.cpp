#include <vector>
#include <iostream>
#include <gtest/gtest.h>

#include "cg/triangulation/triangulation.h"
#include "cg/operations/contains/triangle_point.h"
#include "cg/operations/contains/segment_point.h"

using namespace std;
using namespace cg;

typedef vector<contour_2> polygon;

bool inner_intersection(const segment_2 & a, const segment_2 &b) {
    if (a[0] == a[1]) 
        return false;

    orientation_t ab[2];
    for (size_t l = 0; l != 2; ++l) 
        ab[l] = orientation(a[0], a[1], b[l]);

    if (ab[0] == ab[1] && ab[0] == CG_COLLINEAR) 
        return false;
    if (ab[0] == ab[1]) return false;
    for (size_t l = 0; l != 2; l++) 
        ab[l] = orientation(b[0], b[1], a[l]);
    return ab[0] != ab[1];
}

bool inner_intersection(const segment_2 & a, const triangle_2 &t) {
    for (size_t l = 0; l != 3; l++) 
        if (inner_intersection(a, t.side(l))) return true;
    for (int i = 0; i < 2; i++) {
        auto p = a[i];
        bool in = true;
        if (!contains(t, p)) in = false;
        for (int j = 0; j < 3; j++) {
            if (contains(t.side(j), p)) in = false;
        }
        if (in) return true;
    }
    return false;
}

bool check_triangulation(polygon poly, vector<triangle_2> t) {
    //normal triangles
    size_t count_v = 0;
    for (auto cont : poly) count_v += cont.vertices_num();
    if (t.size() != count_v + 2) return false;
    for (auto tr : t) {
        if (orientation(tr[0], tr[1], tr[2]) == CG_COLLINEAR) return false;
    }
    //which not intersect
    //(intersections with touches allowed
    for (int i = 0; i < t.size(); i++) {
        for (int j = i + 1; j < t.size(); j++) {
            auto t1 = t[i], t2 = t[j];
            for (int k = 0; k < 3; k++) {
                auto s = t1.side(k);
                if (inner_intersection(s, t2)) return false;
            }
        }
    }
    //and fully contains in polygon
    //and feel it completely
    return true;
}

TEST(triangulation, simple_test) {
    contour_2 outer({ point_2(-1, -1), point_2(1, -1), point_2(0, 1), point_2(-1, 1) });
    vector<triangle_2> v = triangulate({ outer });
    EXPECT_TRUE(v.size() == 2);
}


TEST(triangulation, init_test) {
    contour_2 outer({ point_2(-2, -2), point_2(2, -2), point_2(2, 2), point_2(-2, 2) });
    contour_2 hole({ point_2(1, 1), point_2(1, -1), point_2(-1, -1), point_2(-1, 1) });
    vector<triangle_2> v = triangulate({ outer, hole });
    EXPECT_TRUE(v.size() == 8);
}

int main(int argc, char **argv) {
    cout << "Start testing [monotone triangulation]" << endl;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
