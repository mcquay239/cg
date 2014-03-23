#include <gtest/gtest.h>

#include <cg/psl/psl.h>

#include <map>
#include <set>

using cg::profiling_t;
using cg::persistent_set_t;

struct segment_t
{
   float val;
   size_t t1;
   size_t t2;
};

typedef std::vector<segment_t> segments_t;

bool fill(segments_t const & segments)
{
   try
   {
      persistent_set_t<float, size_t> test_set;
      for (segment_t const & s: segments)
         test_set.insert(s.val, s.t1, s.t2);
   }
   catch (...)
   {
      return false;
   }

   return true;
}

boost::tuple<segments_t, segments_t> segm_partition(segments_t const & in, size_t n)
{
   Verify(n < in.size());

   segments_t pos, neg;
   for (size_t l = 0; l != in.size(); ++l)
   {
      double v1 = std::rand() * 1. / RAND_MAX;
      double v2 = (n - pos.size()) * 1. / (in.size() - l);

      if (v1 < v2)
         pos.push_back(in[l]);
      else
         neg.push_back(in[l]);
   }

   return boost::make_tuple(pos, neg);
}

bool iterate(segments_t & segments, size_t & cn)
{
   size_t tries = 10;
   for (size_t l = 0; l != tries; ++l)
   {
      std::cout << "try " << l << " cn " << cn << " segments size " << segments.size() << std::endl;

      segments_t p, n;
      boost::tie(p, n) = segm_partition(segments, cn);

      if (!fill(p))
      {
         std::cout << "pos" << std::endl;

         segments = p;
         cn = segments.size() / 2;

         return true;
      }

      if (!fill(n))
      {
         std::cout << "neg" << std::endl;

         segments = n;
         cn = segments.size() / 2;

         return true;
      }
   }

   return false;
}

void shrink(segments_t & segments)
{
   Verify(!fill(segments));

   size_t cn = segments.size() / 2;
   while (cn)
   {
      if (!iterate(segments, cn))
         cn /= 2;
   }

   std::cout << "end, segments.size() == " << segments.size() << std::endl;
}


profiling_t test(size_t const operations, bool test_result)
{
   boost::random::bernoulli_distribution<> coin(0.75);
   boost::random::uniform_01<float> uf;
   boost::random::uniform_int_distribution<> ufi;
   boost::random::mt19937 eng;

   std::vector<segment_t> segments;

   std::map<float, size_t> status;
   std::map<size_t, std::set<float> > data;
   persistent_set_t<float, size_t> test_set;

   for (size_t l = 0; l != operations; ++l)
   {
      if (!data.empty())
         data[l] = data[l - 1];

      if (data.empty() || data[l - 1].empty() || coin(eng))
      {
         do
         {
            float e = uf(eng);
            if (data[l].count(e))
               continue;

            status[e] = l;
            data[l].insert(e);
            //test_set.insert(e);
            break;
         }
         while (true);
      }
      else
      {
         size_t max_idx = data[l].size() - 1;
         size_t idx = ufi(eng, boost::random::uniform_int_distribution<>::param_type(0, max_idx));
         float val = *boost::next(data[l].begin(), idx);

         segments.push_back({val, status[val], l});

         data[l].erase(val);
         //test_set.erase(val);
      }
   }

   for (size_t l = operations; !data.rbegin()->second.empty(); ++l)
   {
      data[l] = data[l - 1];

      size_t max_idx = data[l].size() - 1;
      size_t idx = ufi(eng, boost::random::uniform_int_distribution<>::param_type(0, max_idx));
      float val = *boost::next(data[l].begin(), idx);

      segments.push_back({val, status[val], l});

      data[l].erase(val);
   }

   boost::random_shuffle(segments);
   if (test_result)
      std::cout << "start fill" << std::endl;
   size_t l = 0;
   try
   {
      for (; l != segments.size(); ++l)
      {
         segment_t const & s = segments[l];
         //std::cout << s.val << " " << s.t1 << " " << s.t2 << std::endl;
         test_set.insert(s.val, s.t1, s.t2);
      }
   }
   catch (...)
   {
      if (l + 1 != segments.size())
         segments.erase(segments.begin() + l + 1, segments.end());

      shrink(segments);

      persistent_set_t<float, size_t> ps;
      for (segment_t const & s: segments)
      {
         std::cout << s.val << " " << s.t1 << " " << s.t2 << std::endl;
         ps.insert(s.val, s.t1, s.t2);
      }

      std::cout << "test end" << std::endl;

      return profiling_t();
   }

   profiling_t res = test_set.profiling();

   if (test_result)
   {
      std::cout << "levels: " << test_set.levels() << std::endl;
      for (size_t l = 0; l != data.size(); ++l)
      {
         if (!boost::equal(test_set.slice(l), data[l]))
         {
            std::list<float> r1 = test_set.slice(l);
            std::cout << "test_set: " << r1.size() << std::endl;
            for (float f: test_set.slice(l))
               std::cout << " " << f;
            std::cout << std::endl;

            std::set<float> r2 = data[l];
            std::cout << "data[l]: " << r2.size() << std::endl;
            for (float f: data[l])
               std::cout << " " << f;
            std::cout << std::endl;

            std::set<float> res;
            boost::set_difference(r1, r2, std::inserter(res, res.end()));

            std::cout << "test_set - data:";
            for (float f: res)
               std::cout << " " << f;
            std::cout << std::endl;

            res.clear();
            boost::set_difference(r2, r1, std::inserter(res, res.end()));

            std::cout << "data - test_set:";
            for (float f: res)
               std::cout << " " << f;
            std::cout << std::endl;


   //         skip_list_viewer viewer(test_set);
   //         cg::visualization::run_viewer(&viewer, "skip list");

            Verify(0);
         }
      }
   }

   return res;
}

boost::tuple<profiling_t, double> test_speed(size_t operations, bool dmp = false)
{
   std::vector<size_t> times(2 * operations);
   for (size_t l = 0; l != operations * 2; ++l)
      times[l] = l;

   std::vector<size_t> values(operations);
   for (size_t l = 0; l != operations; ++l)
      values[l] = 1 + l;

   boost::random_shuffle(values);
   boost::random_shuffle(times);

   if (dmp)
      std::cout << "data prepared" << std::endl;

   typedef boost::chrono::steady_clock clock_t;
   boost::chrono::time_point<clock_t> start = clock_t::now();

   persistent_set_t<float, size_t> ps;
   for (size_t l = 0; l != operations; ++l)
   {
      if (times[2 * l] > times[2 * l + 1])
         std::swap(times[2 * l], times[2 * l + 1]);

      ps.insert(values[l], times[2 * l], times[2 * l + 1]);
   }

   clock_t::duration d = clock_t::now() - start;
   double dur = boost::chrono::duration_cast<boost::chrono::milliseconds>(d).count() * 1e-3;

   if (dmp)
      std::cout << "done" << std::endl;

   return boost::make_tuple(ps.profiling(), dur);
}

TEST(psl, correctness)
{
   test(10000, true);
}

TEST(psl, speed)
{
   std::cout << "N\t N\t\t N log N\t N\t\t N log N \t N log N\t sec" << std::endl;

   for (size_t i = 6; i <= 30; ++i)
   {
      int l = 1 << i;
      double r[5];
      for (size_t k = 0; k != 5; ++k)
         r[k] = 0;

      for (size_t k = 0; k != 10; ++k)
      {
         profiling_t mi;
         double dur;
         boost::tie(mi, dur) = test_speed(l);
         r[0] += mi.nodes;
         r[1] += mi.overhead;
         r[2] += mi.insertion;
         r[3] += mi.localization;
         r[4] += dur;
      }

      for (size_t k = 0; k != 5; ++k)
         r[k] /= 10 * l;

      std::cout << l << ":\t ";
      std::cout << "n " << r[0] << "\t";
      std::cout << "o " << r[1] / log(l) << "\t";
      std::cout << "i " << r[2] << "\t";
      std::cout << "l " << r[3] / log(l) << "\t";
      std::cout << "t " << r[4] / log(l) << "\t";
      std::cout << "(" << r[4] * 10 * l << ")";

      std::cout << std::endl;
   }
}
