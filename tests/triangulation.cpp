#include <vector>
#include <iostream>
#include <gtest/gtest.h>

#include "cg/triangulation/triangulation.h"


using namespace std;

TEST(triangulation, init_test) {
    cg::contour_2 outer({ cg::point_2(-2, -2), cg::point_2(2, -2), cg::point_2(2, 2), cg::point_2(-2, 2) });
    cg::contour_2 hole({ cg::point_2(-1, -1), cg::point_2(1, -1), cg::point_2(1, 1), cg::point_2(-1, 1) });
    vector<cg::triangle_2> v = cg::triangulate({ outer, hole });
    EXPECT_TRUE(v.size() == 1);
}

int main(int argc, char **argv) {
    cout << "Start testing [monotone triangulation]" << endl;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
