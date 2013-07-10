#include <gtest/gtest.h>

#include "cg/operations/my_operation.h"
#include <boost/chrono.hpp>
#include "random_utils.h"

TEST(my_operation, just_test)
{
   boost::random::mt19937 gen;
   boost::random::uniform_real_distribution<> distr(-(1LL << 53), (1LL << 53));

   cg::point_2 a(1, 2), b(2, 3), c(3, 5);
   cg::point_2 res = my_operation(a, b);
   EXPECT_TRUE(res.x == a.x + b.x && res.y == a.y + b.y);
}

TEST(my_operation, just_another_test)
{
   boost::random::mt19937 gen;
   boost::random::mt19937 gen2;
   boost::random::uniform_real_distribution<> distr(-(1LL << 53), (1LL << 53));

   boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();

   cg::point_2 a(2, 2), b(2, 4), c(4, 6);
   cg::point_2 res = my_operation(a, b);

   EXPECT_TRUE(res.x == c.x && res.y == c.y);
}

//TEST(intersect_test, intersect_accuracy)
//{
//    std::default_random_engine generator(::rand());
//    std::uniform_int_distribution<int> size_distribution(10, 30);
//    std::vector<int> v(size_distribution(generator));
//    std::uniform_int_distribution<int> number_distribution(0, 100);
//    boost::sort(boost::generate(
//                    v, [&number_distribution, &generator] () { return number_distribution(generator); }));
//    std::uniform_int_distribution<int> test_distribution(-150, 150);
//    int a = test_distribution(generator);
//    int b = test_distribution(generator);
//    std::vector<int> res;
//    intersect(v.begin(), v.end(), a, b, std::back_inserter(res));
//    std::vector<int> res_slow;
//    intersect_slow(v.begin(), v.end(), a, b, std::back_inserter(res_slow));
//    EXPECT_EQ(res, res_slow);
//}

//TEST(intersect_test, intersect_speed)
//{
//    std::default_random_engine generator(::rand());
//    std::uniform_int_distribution<int> size_distribution(10000, 30000);
//    std::vector<int> v(size_distribution(generator));
//    std::uniform_int_distribution<int> number_distribution(0, 1000000);
//    boost::sort(boost::generate(
//                    v, [&number_distribution, &generator] () { return number_distribution(generator); }));
//    std::uniform_int_distribution<int> test_distribution(-1500000, 1500000);
//    int a = test_distribution(generator);
//    int b = test_distribution(generator);
//    std::vector<int> res;
//    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
//    intersect(v.begin(), v.end(), a, b, std::back_inserter(res));
//    std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
//    std::vector<int> res_slow;
//    start = std::chrono::system_clock::now();
//    intersect_slow(v.begin(), v.end(), a, b, std::back_inserter(res_slow));
//    std::chrono::duration<double> sec_slow = std::chrono::system_clock::now() - start;
//    EXPECT_LE(sec, sec_slow);
//}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}




