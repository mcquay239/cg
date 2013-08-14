#include <vector>
#include <iostream>
#include <gtest/gtest.h>

#include "cg/triangulation/triangulation.h"
#include "cg/operations/contains/triangle_point.h"
#include "cg/operations/contains/segment_point.h"
#include "cg/operations/contains/contour_point.h"
#include <gmpxx.h>

using namespace std;
using namespace cg;

typedef vector<contour_2> polygon;

bool inner_intersection(const segment_2 & a, const segment_2 &b) {
   if (a[0] == a[1])
      return false;

   orientation_t ab[2];
   for (size_t l = 0; l != 2; ++l)
      ab[l] = orientation(a[0], a[1], b[l]);

   if (ab[0] == CG_COLLINEAR || ab[1] == CG_COLLINEAR)
      return false;
   if (ab[0] == ab[1]) return false;
   for (size_t l = 0; l != 2; l++)
      ab[l] = orientation(b[0], b[1], a[l]);
   if (ab[0] == CG_COLLINEAR || ab[1] == CG_COLLINEAR)
      return false;
   return ab[0] != ab[1];
}

bool inner_intersection(const segment_2 & a, const triangle_2 &t) {
   for (size_t l = 0; l != 3; l++)
      if (inner_intersection(a, t.side(l))) return true;
   for (size_t i = 0; i < 2; i++) {
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

bool contains(const point_2 &p, polygon &poly) {
   if (!contains(poly[0], p)) return false;
   for (size_t i = 1; i < poly.size(); i++) {
      if (contains(poly[i], p)) return false;
   }
   return true;
}

mpq_class S(polygon &poly) {
   mpq_class res = 0;
   for (auto cont : poly) {
      auto c = cont.circulator();
      auto st = c;
      do {
         point_2 p1(*c), p2(*(c + 1));
         mpq_class x1(p1.x), x2(p2.x), y1(p1.y), y2(p2.y);
         res += (x1 * y2 - x2 * y1) / 2;
      } while (++c != st);
   }
   return res;
}

mpq_class S(triangle_2 &t) {
   mpq_class res = 0;
   point_2 p0(t[0]), p1(t[1]), p2(t[2]);
   mpq_class x0(p0.x), y0(p0.y),x1(p1.x), x2(p2.x), y1(p1.y), y2(p2.y);
   x1 -= x0; y1 -= y0;
   x2 -= x0; y2 -= y0;
   res = (x1 * y2 - x2 * y1) / 2;
   return abs(res);
}

void check_triangulation(polygon poly, vector<triangle_2> t) {
   //normal triangles
   size_t count_v = 0;
   for (auto cont : poly) count_v += cont.vertices_num();
   EXPECT_TRUE(t.size() == count_v + 2 * (poly.size() - 2));
   for (auto tr : t) {
      EXPECT_FALSE(orientation(tr[0], tr[1], tr[2]) == CG_COLLINEAR);
   }
   //which not intersect
   //(intersections with touches allowed
   for (size_t i = 0; i < t.size(); i++) {
      for (size_t j = i + 1; j < t.size(); j++) {
         auto t1 = t[i], t2 = t[j];
         for (int k = 0; k < 3; k++) {
            auto s = t1.side(k);
            EXPECT_FALSE(inner_intersection(s, t2));
         }
      }
   }
   //and not intersect border
   for (auto cont : poly) {
      auto c = cont.circulator();
      auto st = c;
      do {
         for (auto tr : t)
            EXPECT_FALSE(inner_intersection(segment_2(*c, *(c + 1)), tr));
      } while (++c != st);
   }
   //and fully contains in polygon
   for (auto tr : t) {
      point_2 p((tr[0].x + tr[1].x + tr[2].x) / 3, (tr[0].y + tr[1].y + tr[2].y) / 3);
      EXPECT_TRUE(contains(p, poly));
   }
   //and feel it completely
   mpq_class Spoly = S(poly);
   mpq_class Striangles = 0;
   for (auto tr : t) Striangles += S(tr);
   EXPECT_TRUE(Spoly == Striangles);
}

TEST(triangulation, custom_00) {
   vector<contour_2> poly;
   contour_2 cur0({point_2(-728, 359), point_2(-828, -211), point_2(-574, -46), point_2(-376, -285), point_2(-328, -95), point_2(-358, -403), point_2(-48, 247), point_2(-707, -47)});
   poly.push_back(cur0);
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, custom_1) {
   vector<contour_2> poly;
   contour_2 cur0({point_2(-494, 326), point_2(-839, -381), point_2(-534, 47), point_2(-243, 48), point_2(-237, -49), point_2(-102, -41), point_2(-111, 201), point_2(-343, 187), point_2(-342, 114), point_2(-538, 118)});
   poly.push_back(cur0);
   contour_2 cur1({point_2(-278, 160), point_2(-153, 166), point_2(-144, 0), point_2(-193, 135), point_2(-316, 71)});
   poly.push_back(cur1);
   contour_2 cur2({point_2(-561, 143), point_2(-601, 37), point_2(-450, 75), point_2(-602, 31), point_2(-621, 40)});
   poly.push_back(cur2);
   contour_2 cur3({point_2(-672, -52), point_2(-631, -11), point_2(-622, -46)});
   poly.push_back(cur3);
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, custom_2) {
   vector<contour_2> poly;
   contour_2 cur0({point_2(-246, 303), point_2(-778, 151), point_2(-782, 45), point_2(-471, 55), point_2(-528, 170), point_2(-246, 138), point_2(-432, -85), point_2(-802, -67), point_2(-571, -11), point_2(-858, 19), point_2(-933, -275), point_2(-492, -397), point_2(-67, -83), point_2(-189, 60), point_2(342, 137), point_2(403, -144), point_2(-73, -301), point_2(-115, -206), point_2(-266, -317), point_2(-289, -407), point_2(41, -458), point_2(219, -339), point_2(241, -258), point_2(384, -352), point_2(382, -447), point_2(691, -366), point_2(692, -192), point_2(835, 103), point_2(601, 236), point_2(335, 310), point_2(167, 372), point_2(-67, 226)});
   poly.push_back(cur0);
   contour_2 cur1({point_2(166, 256), point_2(323, 283), point_2(469, 183), point_2(-111, 167), point_2(17, 219), point_2(78, 210)});
   poly.push_back(cur1);
   contour_2 cur2({point_2(497, -140), point_2(565, 132), point_2(728, 58), point_2(584, -95), point_2(616, -283), point_2(349, -257)});
   poly.push_back(cur2);
   contour_2 cur3({point_2(-784, -272), point_2(-723, -208), point_2(-809, -117), point_2(-527, -130), point_2(-514, -277), point_2(-626, -195)});
   poly.push_back(cur3);
   contour_2 cur4({point_2(-363, -151), point_2(-279, 70), point_2(-228, -100), point_2(-286, -35), point_2(-356, -241), point_2(-458, -208), point_2(-426, -128), point_2(-411, -190)});
   poly.push_back(cur4);
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, custom_3) {
   vector<contour_2> poly;
   contour_2 cur0({point_2(-656, 401), point_2(-651, -399), point_2(193, -450), point_2(213, 396)});
   poly.push_back(cur0);
   contour_2 cur1({point_2(-460, -288), point_2(-422, 235), point_2(-97, 258), point_2(-125, 24), point_2(-327, 32), point_2(-333, -55), point_2(-42, -98), point_2(-45, -260)});
   poly.push_back(cur1);
   contour_2 cur2({point_2(-101, -49), point_2(-299, -30), point_2(-301, 8), point_2(-103, 3)});
   poly.push_back(cur2);
}

TEST(triangulation, custom_0) {
   vector<contour_2> poly;
   contour_2 cur({point_2(-109, 42), point_2(-151, -87), point_2(104, -114), point_2(133, 25)});
   poly.push_back(cur);
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, simple_test) {
   contour_2 outer({ point_2(-1, -1), point_2(1, -1), point_2(0, 1), point_2(-1, 1) });
   auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}


TEST(triangulation, init_test) {
   contour_2 outer({ point_2(-2, -2), point_2(2, -2), point_2(2, 2), point_2(-2, 2) });
   contour_2 hole({ point_2(1, 1), point_2(1, -1), point_2(-1, -1), point_2(-1, 1) });
   auto poly = {outer, hole};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_01) {
   contour_2 outer ({ point_2(0, 0), point_2(-1, -1), point_2(1, 0), point_2(-1, 1)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_02) {
   contour_2 outer ({ point_2(0, 0), point_2(1, 0), point_2(0, 1)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_03) {
   contour_2 outer ({ point_2(-1e+06, -1e+06), point_2(1e+06, 42), point_2(-999999, 1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_04) {
   contour_2 outer ({ point_2(1e+06, 1e+06), point_2(999999, -1e+06), point_2(1e+06, 999999)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_05) {
   contour_2 outer ({ point_2(-1e+06, 0), point_2(0, -1), point_2(1e+06, 0), point_2(0, 1)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_06) {
   contour_2 outer ({ point_2(1, 0), point_2(0, 1e+06), point_2(-1, 0), point_2(0, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_07) {
   contour_2 outer ({ point_2(-1e+06, 0), point_2(0, -1e+06), point_2(1e+06, 0), point_2(0, 1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_08) {
   contour_2 outer ({ point_2(-1e+06, 1e+06), point_2(-999999, -999998), point_2(-999998, -999999), point_2(1e+06, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_09) {
   contour_2 outer ({ point_2(0, 0), point_2(1, 0), point_2(1, 1), point_2(3, 1), point_2(3, 2), point_2(0, 2)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_10) {
   contour_2 outer ({ point_2(1, 0), point_2(1, 999999), point_2(-1, 999999), point_2(-1, 0), point_2(-1e+06, -999999), point_2(-999999, -1e+06), point_2(0, -1), point_2(999999, -1e+06), point_2(1e+06, -999999)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_11) {
   contour_2 outer ({ point_2(-999999, 1e+06), point_2(-1e+06, 999999), point_2(-1, 0), point_2(-1e+06, -999999), point_2(-999999, -1e+06), point_2(0, -1), point_2(999999, -1), point_2(999999, 1), point_2(0, 1)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_12) {
   contour_2 outer ({ point_2(1e+06, 1e+06), point_2(-1e+06, 1e+06), point_2(-1e+06, 999996), point_2(999994, 999996), point_2(-1e+06, -999998), point_2(-999998, -1e+06), point_2(999996, 999994), point_2(999996, -1e+06), point_2(1e+06, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_13) {
   contour_2 outer ({ point_2(-1e+06, 1e+06), point_2(-1e+06, 999996), point_2(999994, 999996), point_2(-1e+06, -999998), point_2(-999998, -1e+06), point_2(999996, 999994), point_2(999996, -1e+06), point_2(1e+06, -1e+06), point_2(1e+06, 1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_14) {
   contour_2 outer ({ point_2(0, 0), point_2(1, 1), point_2(2, 0), point_2(1, 2)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_15) {
   contour_2 outer ({ point_2(2, 1), point_2(0, 2), point_2(1, 1), point_2(0, 0)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_16) {
   contour_2 outer ({ point_2(0, -1), point_2(1, 1), point_2(2, 0), point_2(1, 2)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_17) {
   contour_2 outer ({ point_2(2, 1), point_2(0, 2), point_2(1, 1), point_2(-1, 0)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_18) {
   contour_2 outer ({ point_2(-1, -1e+06), point_2(0, 0), point_2(1, -1e+06), point_2(1, 1e+06), point_2(0, 1), point_2(-1, 1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_19) {
   contour_2 outer ({ point_2(1e+06, -1), point_2(1, 0), point_2(1e+06, 1), point_2(-1e+06, 1), point_2(0, 0), point_2(-1e+06, -1)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_20) {
   contour_2 outer ({ point_2(0, 999999), point_2(1, -1e+06), point_2(2, 999999), point_2(2, 1e+06), point_2(1, -999999), point_2(0, 1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_21) {
   contour_2 outer ({ point_2(1e+06, 0), point_2(-999999, 1), point_2(1e+06, 2), point_2(999999, 2), point_2(-1e+06, 1), point_2(999999, 0)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_22) {
   contour_2 outer ({ point_2(1e+06, 1e+06), point_2(-1275, 49), point_2(-1225, 48), point_2(-1176, 47), point_2(-1128, 46), point_2(-1081, 45), point_2(-1035, 44), point_2(-990, 43), point_2(-946, 42), point_2(-903, 41), point_2(-861, 40), point_2(-820, 39), point_2(-780, 38), point_2(-741, 37), point_2(-703, 36), point_2(-666, 35), point_2(-630, 34), point_2(-595, 33), point_2(-561, 32), point_2(-528, 31), point_2(-496, 30), point_2(-465, 29), point_2(-435, 28), point_2(-406, 27), point_2(-378, 26), point_2(-351, 25), point_2(-325, 24), point_2(-300, 23), point_2(-276, 22), point_2(-253, 21), point_2(-231, 20), point_2(-210, 19), point_2(-190, 18), point_2(-171, 17), point_2(-153, 16), point_2(-136, 15), point_2(-120, 14), point_2(-105, 13), point_2(-91, 12), point_2(-78, 11), point_2(-66, 10), point_2(-55, 9), point_2(-45, 8), point_2(-36, 7), point_2(-28, 6), point_2(-21, 5), point_2(-15, 4), point_2(-10, 3), point_2(-6, 2), point_2(-3, 1), point_2(-1, 0), point_2(0, -1), point_2(1, -3), point_2(2, -6), point_2(3, -10), point_2(4, -15), point_2(5, -21), point_2(6, -28), point_2(7, -36), point_2(8, -45), point_2(9, -55), point_2(10, -66), point_2(11, -78), point_2(12, -91), point_2(13, -105), point_2(14, -120), point_2(15, -136), point_2(16, -153), point_2(17, -171), point_2(18, -190), point_2(19, -210), point_2(20, -231), point_2(21, -253), point_2(22, -276), point_2(23, -300), point_2(24, -325), point_2(25, -351), point_2(26, -378), point_2(27, -406), point_2(28, -435), point_2(29, -465), point_2(30, -496), point_2(31, -528), point_2(32, -561), point_2(33, -595), point_2(34, -630), point_2(35, -666), point_2(36, -703), point_2(37, -741), point_2(38, -780), point_2(39, -820), point_2(40, -861), point_2(41, -903), point_2(42, -946), point_2(43, -990), point_2(44, -1035), point_2(45, -1081), point_2(46, -1128), point_2(47, -1176), point_2(48, -1225), point_2(49, -1275)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_23) {
   contour_2 outer ({ point_2(-1275, 49), point_2(-1225, 48), point_2(-1176, 47), point_2(-1128, 46), point_2(-1081, 45), point_2(-1035, 44), point_2(-990, 43), point_2(-946, 42), point_2(-903, 41), point_2(-861, 40), point_2(-820, 39), point_2(-780, 38), point_2(-741, 37), point_2(-703, 36), point_2(-666, 35), point_2(-630, 34), point_2(-595, 33), point_2(-561, 32), point_2(-528, 31), point_2(-496, 30), point_2(-465, 29), point_2(-435, 28), point_2(-406, 27), point_2(-378, 26), point_2(-351, 25), point_2(-325, 24), point_2(-300, 23), point_2(-276, 22), point_2(-253, 21), point_2(-231, 20), point_2(-210, 19), point_2(-190, 18), point_2(-171, 17), point_2(-153, 16), point_2(-136, 15), point_2(-120, 14), point_2(-105, 13), point_2(-91, 12), point_2(-78, 11), point_2(-66, 10), point_2(-55, 9), point_2(-45, 8), point_2(-36, 7), point_2(-28, 6), point_2(-21, 5), point_2(-15, 4), point_2(-10, 3), point_2(-6, 2), point_2(-3, 1), point_2(-1, 0), point_2(0, -1), point_2(1, -3), point_2(2, -6), point_2(3, -10), point_2(4, -15), point_2(5, -21), point_2(6, -28), point_2(7, -36), point_2(8, -45), point_2(9, -55), point_2(10, -66), point_2(11, -78), point_2(12, -91), point_2(13, -105), point_2(14, -120), point_2(15, -136), point_2(16, -153), point_2(17, -171), point_2(18, -190), point_2(19, -210), point_2(20, -231), point_2(21, -253), point_2(22, -276), point_2(23, -300), point_2(24, -325), point_2(25, -351), point_2(26, -378), point_2(27, -406), point_2(28, -435), point_2(29, -465), point_2(30, -496), point_2(31, -528), point_2(32, -561), point_2(33, -595), point_2(34, -630), point_2(35, -666), point_2(36, -703), point_2(37, -741), point_2(38, -780), point_2(39, -820), point_2(40, -861), point_2(41, -903), point_2(42, -946), point_2(43, -990), point_2(44, -1035), point_2(45, -1081), point_2(46, -1128), point_2(47, -1176), point_2(48, -1225), point_2(49, -1275), point_2(1e+06, 1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_30) {
   contour_2 outer ({ point_2(-1e+06, -1e+06), point_2(-999999, 999999), point_2(-999999, 1e+06), point_2(-1e+06, -999999)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_31) {
   contour_2 outer ({ point_2(-999999, -1e+06), point_2(1e+06, -999999), point_2(999999, -999999), point_2(-1e+06, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_32) {
   contour_2 outer ({ point_2(-1e+06, -1e+06), point_2(-999999, 999999), point_2(-999998, -1e+06), point_2(-999998, -999999), point_2(-999999, 1e+06), point_2(-1e+06, -999999)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_33) {
   contour_2 outer ({ point_2(-999999, -1e+06), point_2(1e+06, -999999), point_2(-999999, -999998), point_2(-1e+06, -999998), point_2(999999, -999999), point_2(-1e+06, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_34) {
   contour_2 outer ({ point_2(-1e+06, -1e+06), point_2(-999999, 999999), point_2(-999998, -1e+06), point_2(-999997, 999999), point_2(-999996, -1e+06), point_2(-999996, -999999), point_2(-999997, 1e+06), point_2(-999998, -999999), point_2(-999999, 1e+06), point_2(-1e+06, -999999)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_35) {
   contour_2 outer ({ point_2(-999999, -1e+06), point_2(1e+06, -999999), point_2(-999999, -999998), point_2(1e+06, -999997), point_2(-999999, -999996), point_2(-1e+06, -999996), point_2(999999, -999997), point_2(-1e+06, -999998), point_2(999999, -999999), point_2(-1e+06, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_36) {
   contour_2 outer ({ point_2(-1e+06, -1e+06), point_2(-999999, 999999), point_2(-999998, -1e+06), point_2(-999997, 999999), point_2(-999996, -1e+06), point_2(-999995, 999999), point_2(-999994, -1e+06), point_2(-999994, -999999), point_2(-999995, 1e+06), point_2(-999996, -999999), point_2(-999997, 1e+06), point_2(-999998, -999999), point_2(-999999, 1e+06), point_2(-1e+06, -999999)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_37) {
   contour_2 outer ({ point_2(-999999, -1e+06), point_2(1e+06, -999999), point_2(-999999, -999998), point_2(1e+06, -999997), point_2(-999999, -999996), point_2(1e+06, -999995), point_2(-999999, -999994), point_2(-1e+06, -999994), point_2(999999, -999995), point_2(-1e+06, -999996), point_2(999999, -999997), point_2(-1e+06, -999998), point_2(999999, -999999), point_2(-1e+06, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_38) {
   contour_2 outer ({ point_2(-1e+06, -1e+06), point_2(-999999, 999999), point_2(-999998, -1e+06), point_2(-999997, 999999), point_2(-999996, -1e+06), point_2(-999995, 999999), point_2(-999994, -1e+06), point_2(-999993, 999999), point_2(-999992, -1e+06), point_2(-999992, -999999), point_2(-999993, 1e+06), point_2(-999994, -999999), point_2(-999995, 1e+06), point_2(-999996, -999999), point_2(-999997, 1e+06), point_2(-999998, -999999), point_2(-999999, 1e+06), point_2(-1e+06, -999999)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_39) {
   contour_2 outer ({ point_2(-999999, -1e+06), point_2(1e+06, -999999), point_2(-999999, -999998), point_2(1e+06, -999997), point_2(-999999, -999996), point_2(1e+06, -999995), point_2(-999999, -999994), point_2(1e+06, -999993), point_2(-999999, -999992), point_2(-1e+06, -999992), point_2(999999, -999993), point_2(-1e+06, -999994), point_2(999999, -999995), point_2(-1e+06, -999996), point_2(999999, -999997), point_2(-1e+06, -999998), point_2(999999, -999999), point_2(-1e+06, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_40) {
   contour_2 outer ({ point_2(-1e+06, -1e+06), point_2(-999999, 999999), point_2(-999998, -1e+06), point_2(-999997, 999999), point_2(-999996, -1e+06), point_2(-999995, 999999), point_2(-999994, -1e+06), point_2(-999993, 999999), point_2(-999992, -1e+06), point_2(-999991, 999999), point_2(-999990, -1e+06), point_2(-999990, -999999), point_2(-999991, 1e+06), point_2(-999992, -999999), point_2(-999993, 1e+06), point_2(-999994, -999999), point_2(-999995, 1e+06), point_2(-999996, -999999), point_2(-999997, 1e+06), point_2(-999998, -999999), point_2(-999999, 1e+06), point_2(-1e+06, -999999)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_41) {
   contour_2 outer ({ point_2(-999999, -1e+06), point_2(1e+06, -999999), point_2(-999999, -999998), point_2(1e+06, -999997), point_2(-999999, -999996), point_2(1e+06, -999995), point_2(-999999, -999994), point_2(1e+06, -999993), point_2(-999999, -999992), point_2(1e+06, -999991), point_2(-999999, -999990), point_2(-1e+06, -999990), point_2(999999, -999991), point_2(-1e+06, -999992), point_2(999999, -999993), point_2(-1e+06, -999994), point_2(999999, -999995), point_2(-1e+06, -999996), point_2(999999, -999997), point_2(-1e+06, -999998), point_2(999999, -999999), point_2(-1e+06, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_42) {
   contour_2 outer ({ point_2(-1e+06, -1e+06), point_2(-999999, 999999), point_2(-999998, -1e+06), point_2(-999997, 999999), point_2(-999996, -1e+06), point_2(-999995, 999999), point_2(-999994, -1e+06), point_2(-999993, 999999), point_2(-999992, -1e+06), point_2(-999991, 999999), point_2(-999990, -1e+06), point_2(-999989, 999999), point_2(-999988, -1e+06), point_2(-999987, 999999), point_2(-999986, -1e+06), point_2(-999985, 999999), point_2(-999984, -1e+06), point_2(-999983, 999999), point_2(-999982, -1e+06), point_2(-999981, 999999), point_2(-999980, -1e+06), point_2(-999980, -999999), point_2(-999981, 1e+06), point_2(-999982, -999999), point_2(-999983, 1e+06), point_2(-999984, -999999), point_2(-999985, 1e+06), point_2(-999986, -999999), point_2(-999987, 1e+06), point_2(-999988, -999999), point_2(-999989, 1e+06), point_2(-999990, -999999), point_2(-999991, 1e+06), point_2(-999992, -999999), point_2(-999993, 1e+06), point_2(-999994, -999999), point_2(-999995, 1e+06), point_2(-999996, -999999), point_2(-999997, 1e+06), point_2(-999998, -999999), point_2(-999999, 1e+06), point_2(-1e+06, -999999)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_43) {
   contour_2 outer ({ point_2(-999999, -1e+06), point_2(1e+06, -999999), point_2(-999999, -999998), point_2(1e+06, -999997), point_2(-999999, -999996), point_2(1e+06, -999995), point_2(-999999, -999994), point_2(1e+06, -999993), point_2(-999999, -999992), point_2(1e+06, -999991), point_2(-999999, -999990), point_2(1e+06, -999989), point_2(-999999, -999988), point_2(1e+06, -999987), point_2(-999999, -999986), point_2(1e+06, -999985), point_2(-999999, -999984), point_2(1e+06, -999983), point_2(-999999, -999982), point_2(1e+06, -999981), point_2(-999999, -999980), point_2(-1e+06, -999980), point_2(999999, -999981), point_2(-1e+06, -999982), point_2(999999, -999983), point_2(-1e+06, -999984), point_2(999999, -999985), point_2(-1e+06, -999986), point_2(999999, -999987), point_2(-1e+06, -999988), point_2(999999, -999989), point_2(-1e+06, -999990), point_2(999999, -999991), point_2(-1e+06, -999992), point_2(999999, -999993), point_2(-1e+06, -999994), point_2(999999, -999995), point_2(-1e+06, -999996), point_2(999999, -999997), point_2(-1e+06, -999998), point_2(999999, -999999), point_2(-1e+06, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_44) {
   contour_2 outer ({ point_2(-1e+06, -1e+06), point_2(-999999, 999999), point_2(-999998, -1e+06), point_2(-999997, 999999), point_2(-999996, -1e+06), point_2(-999995, 999999), point_2(-999994, -1e+06), point_2(-999993, 999999), point_2(-999992, -1e+06), point_2(-999991, 999999), point_2(-999990, -1e+06), point_2(-999989, 999999), point_2(-999988, -1e+06), point_2(-999987, 999999), point_2(-999986, -1e+06), point_2(-999985, 999999), point_2(-999984, -1e+06), point_2(-999983, 999999), point_2(-999982, -1e+06), point_2(-999981, 999999), point_2(-999980, -1e+06), point_2(-999979, 999999), point_2(-999978, -1e+06), point_2(-999977, 999999), point_2(-999976, -1e+06), point_2(-999975, 999999), point_2(-999974, -1e+06), point_2(-999973, 999999), point_2(-999972, -1e+06), point_2(-999971, 999999), point_2(-999970, -1e+06), point_2(-999969, 999999), point_2(-999968, -1e+06), point_2(-999967, 999999), point_2(-999966, -1e+06), point_2(-999965, 999999), point_2(-999964, -1e+06), point_2(-999963, 999999), point_2(-999962, -1e+06), point_2(-999961, 999999), point_2(-999960, -1e+06), point_2(-999960, -999999), point_2(-999961, 1e+06), point_2(-999962, -999999), point_2(-999963, 1e+06), point_2(-999964, -999999), point_2(-999965, 1e+06), point_2(-999966, -999999), point_2(-999967, 1e+06), point_2(-999968, -999999), point_2(-999969, 1e+06), point_2(-999970, -999999), point_2(-999971, 1e+06), point_2(-999972, -999999), point_2(-999973, 1e+06), point_2(-999974, -999999), point_2(-999975, 1e+06), point_2(-999976, -999999), point_2(-999977, 1e+06), point_2(-999978, -999999), point_2(-999979, 1e+06), point_2(-999980, -999999), point_2(-999981, 1e+06), point_2(-999982, -999999), point_2(-999983, 1e+06), point_2(-999984, -999999), point_2(-999985, 1e+06), point_2(-999986, -999999), point_2(-999987, 1e+06), point_2(-999988, -999999), point_2(-999989, 1e+06), point_2(-999990, -999999), point_2(-999991, 1e+06), point_2(-999992, -999999), point_2(-999993, 1e+06), point_2(-999994, -999999), point_2(-999995, 1e+06), point_2(-999996, -999999), point_2(-999997, 1e+06), point_2(-999998, -999999), point_2(-999999, 1e+06), point_2(-1e+06, -999999)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}

TEST(triangulation, from_kingdom_subdivision_45) {
   contour_2 outer ({ point_2(-999999, -1e+06), point_2(1e+06, -999999), point_2(-999999, -999998), point_2(1e+06, -999997), point_2(-999999, -999996), point_2(1e+06, -999995), point_2(-999999, -999994), point_2(1e+06, -999993), point_2(-999999, -999992), point_2(1e+06, -999991), point_2(-999999, -999990), point_2(1e+06, -999989), point_2(-999999, -999988), point_2(1e+06, -999987), point_2(-999999, -999986), point_2(1e+06, -999985), point_2(-999999, -999984), point_2(1e+06, -999983), point_2(-999999, -999982), point_2(1e+06, -999981), point_2(-999999, -999980), point_2(1e+06, -999979), point_2(-999999, -999978), point_2(1e+06, -999977), point_2(-999999, -999976), point_2(1e+06, -999975), point_2(-999999, -999974), point_2(1e+06, -999973), point_2(-999999, -999972), point_2(1e+06, -999971), point_2(-999999, -999970), point_2(1e+06, -999969), point_2(-999999, -999968), point_2(1e+06, -999967), point_2(-999999, -999966), point_2(1e+06, -999965), point_2(-999999, -999964), point_2(1e+06, -999963), point_2(-999999, -999962), point_2(1e+06, -999961), point_2(-999999, -999960), point_2(-1e+06, -999960), point_2(999999, -999961), point_2(-1e+06, -999962), point_2(999999, -999963), point_2(-1e+06, -999964), point_2(999999, -999965), point_2(-1e+06, -999966), point_2(999999, -999967), point_2(-1e+06, -999968), point_2(999999, -999969), point_2(-1e+06, -999970), point_2(999999, -999971), point_2(-1e+06, -999972), point_2(999999, -999973), point_2(-1e+06, -999974), point_2(999999, -999975), point_2(-1e+06, -999976), point_2(999999, -999977), point_2(-1e+06, -999978), point_2(999999, -999979), point_2(-1e+06, -999980), point_2(999999, -999981), point_2(-1e+06, -999982), point_2(999999, -999983), point_2(-1e+06, -999984), point_2(999999, -999985), point_2(-1e+06, -999986), point_2(999999, -999987), point_2(-1e+06, -999988), point_2(999999, -999989), point_2(-1e+06, -999990), point_2(999999, -999991), point_2(-1e+06, -999992), point_2(999999, -999993), point_2(-1e+06, -999994), point_2(999999, -999995), point_2(-1e+06, -999996), point_2(999999, -999997), point_2(-1e+06, -999998), point_2(999999, -999999), point_2(-1e+06, -1e+06)});    auto poly = {outer};
   vector<triangle_2> v = triangulate(poly);
   check_triangulation(poly, v);
}
