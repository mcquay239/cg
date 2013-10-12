#include <gtest/gtest.h>
#include <cg/primitives/contour.h>
#include <cg/primitives/point.h>
#include <cg/primitives/segment.h>
#include <cg/io/segment.h>
#include <cg/visibility/shortest_path.h>
#include <vector>
#include <iostream>
#include <boost/assign/list_of.hpp>

using namespace std;
using cg::point_2;
using cg::contour_2;
using cg::segment_2;

TEST(shortest_path, sample)
{
	point_2 start(0, 0), finish(4, 4);
	vector<contour_2> poly;
	std::vector<point_2> pts = boost::assign::list_of(point_2(1, 1))
	(point_2(2, 1))
	(point_2(2, 2))
	(point_2(1, 2));

	poly.push_back(pts);
	vector<point_2> ans = cg::get_shortest_path(start, finish, poly);
	for (auto p : ans) {
		cout << p << endl;
	}

	EXPECT_TRUE(true);
}

TEST(shortest_path, hard)
{

	point_2 start(0, 0), finish(10, 6);
	vector<contour_2> poly;
	std::vector<point_2> first = boost::assign::list_of(point_2(5, 1))
	(point_2(5, 5))
	(point_2(3, 3));

	poly.push_back(first);

	std::vector<point_2> second = boost::assign::list_of(point_2(5, -1))
	(point_2(8, -1))
	(point_2(7, 3));

	poly.push_back(second);

	std::vector<point_2> third = boost::assign::list_of(point_2(6, 4))
	(point_2(8, 4))
	(point_2(6, 5));

	poly.push_back(third);

	vector<point_2> ans = cg::get_shortest_path(start, finish, poly);
	for (auto p : ans) {
		cout << p << endl;
	}

	EXPECT_TRUE(true);
}

TEST(shortest_path, unbelievable)
{
	point_2 start(0, 0), finish(4, 3);
	vector<contour_2> poly;
	std::vector<point_2> first = boost::assign::list_of(point_2(1, 1))
	(point_2(1, -2))
	(point_2(2, 0))
	(point_2(3, -2))
	(point_2(3, 1));

	poly.push_back(first);
	vector<point_2> ans = cg::get_shortest_path(start, finish, poly);
	for (auto p : ans) {
		cout << p << endl;
	}

	EXPECT_TRUE(true);
}

TEST(shortest_path, gangsta)
{
	point_2 start(0, 0), finish(4, 5);
	vector<contour_2> poly;
	vector<point_2> ans = cg::get_shortest_path(start, finish, poly);
	for (auto p : ans) {
		cout << p << endl;
	}

	EXPECT_TRUE(true);
}
