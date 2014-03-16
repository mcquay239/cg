#include <QColor>
#include <QApplication>

#include <map>
#include <set>

#include <boost/optional.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/random.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm.hpp>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>

#include <cg/io/point.h>

#include <cg/primitives/point.h>

using cg::point_2f;
using cg::point_2;

float infinite(float *)
{
   return std::numeric_limits<float>::infinity();
}

size_t infinite(size_t *)
{
   return std::numeric_limits<size_t>::max();
}

template <class Data, class Time>
struct persistent_set_t
{
   typedef Time time_t;
   typedef Data data_t;

   persistent_set_t()
      : current_time_(0)
      , nodes_(0)
   {}

   data_t const * lower_bound(data_t const & d, time_t t) const
   {
      node_ptr r = root(t);
      if (!r)
         return NULL;

      data_t const * res = NULL;
      while (r && r->data() <= d)
      {
         res = &r->data();
         r = r->next(t);
      }

      return res;
   }

   data_t const * lower_bound(data_t const & d) const
   {
      return lower_bound(d, current_time_);
   }

   time_t current_time() const { return current_time_; }

   std::list<data_t> slice(time_t t) const
   {
      std::list<data_t> res;
      node_ptr r = root(t);
      while (r)
      {
         res.push_back(r->data());
         r = r->next(t);
      }

      return res;
   }

   void insert(data_t const & d)
   {
      if (roots_.empty())
      {
         node_ptr new_node(new node_t(d, current_time_, nodes_));
         roots_[current_time_] = new_node;
      }
      else
      {
         std::list<node_ptr> nodes;
         node_ptr r = roots_.rbegin()->second;
         while (r && r->data() < d)
         {
            nodes.push_back(r);
            r = r->next(current_time_);
         }

         if (r && r->data() == d)
            return;

         if (nodes.empty())
         {
            node_ptr new_node(new node_t(d, current_time_, nodes_, roots_.rbegin()->second));
            roots_[current_time_] = new_node;
         }
         else
         {
            node_ptr new_node(new node_t(d, current_time_, nodes_, nodes.back()->next(current_time_)));
            while (!nodes.empty() && !nodes.back()->set_next(current_time_, new_node))
            {
               node_ptr fork_node = nodes.back()->fork(current_time_, new_node);
               new_node = fork_node;
               nodes.pop_back();
            }

            if (nodes.empty())
               roots_[current_time_] = new_node;
         }
      }

      current_time_ += 1;
   }

   bool erase(data_t const & d)
   {
      if (roots_.empty())
         return false;

      std::list<node_ptr> nodes;
      node_ptr r = roots_.rbegin()->second;
      while (r && r->data() < d)
      {
         nodes.push_back(r);
         r = r->next(current_time_);
      }

      if (!r || r->data() != d)
         return false;

      node_ptr next = r->next(current_time_);
      while (!nodes.empty() && !nodes.back()->set_next(current_time_, next))
      {
         node_ptr fork = nodes.back()->fork(current_time_, next);
         next = fork;
         nodes.pop_back();
      }

      if (nodes.empty())
         roots_[current_time_] = next ? next->fork(current_time_) : node_ptr();

      current_time_ += 1;

      return true;
   }

   typedef boost::function<bool (data_t, time_t, data_t, time_t)> visitor_f;
   void visit(visitor_f const & v) const
   {
      BOOST_FOREACH(node_ptr node, roots_ | boost::adaptors::map_values)
         if (node)
            node->visit(v);
   }

   size_t nodes() const { return nodes_; }

protected:
   static time_t infinite_time()
   {
      return infinite((time_t *)0);
   }

private:
   struct node_t;
   typedef boost::intrusive_ptr<node_t> node_ptr;

   struct node_t : boost::intrusive_ref_counter<node_t, boost::thread_unsafe_counter>
   {
      node_t(data_t const & data, time_t init_time, size_t & nodes, node_ptr next = node_ptr())
         : data_(data)
         , init_time_(init_time)
         , t_(infinite_time())
         , nodes_(++nodes)
      {
         next_[0] = next;
      }

      ~node_t()
      {
         --nodes_;
      }

      void visit(const visitor_f &v) const
      {
         for (size_t l = 0; l != 2; ++l)
         {
            if (!next_[l])
               continue;

            if (v(data_, init_time_, next_[l]->data_, next_[l]->init_time_))
               next_[l]->visit(v);
         }
      }

      node_ptr next(time_t t) const
      {
         return t < this->t_ ? next_[0] : next_[1];
      }

      bool set_next(time_t t, node_ptr next)
      {
         if (t_ != infinite_time())
            return false;

         t_ = t;
         next_[1] = next;
         return true;
      }

      data_t const & data() const { return data_; }
      time_t init_time() const { return init_time_; }

      node_ptr fork(time_t t, node_ptr next) const
      {
         return new node_t(data_, t, nodes_, next);
      }

      node_ptr fork(time_t t) const
      {
         node_ptr res = new node_t(*this);
         nodes_++;
         res->init_time_ = t;
         return res;
      }

   private:
      data_t data_;
      time_t init_time_;

      node_ptr next_[2];
      time_t t_;

      size_t & nodes_;
   };

private:
   node_ptr root(time_t t) const
   {
      typename std::map<time_t, node_ptr>::const_iterator it = roots_.upper_bound(t);
      if (it == roots_.begin())
         return node_ptr();

      return boost::prior(it)->second;
   }

private:
   time_t current_time_;
   size_t nodes_;
   std::map<time_t, node_ptr> roots_;
};

struct skip_list_viewer : cg::visualization::viewer_adapter
{
   skip_list_viewer(persistent_set_t<float, size_t> & set)
      : set_(set)
   {}

   void draw(cg::visualization::drawer_type & drawer) const
   {
      drawer.set_color(Qt::green);
      set_.visit(boost::bind(&skip_list_viewer::draw, this, boost::ref(drawer), _1, _2, _3, _4));

      drawer.set_color(Qt::white);
      for (float y : set_.slice(mouse_time_))
         drawer.draw_point(point_2f(mouse_time_, y), 5);

      if (float const * l = set_.lower_bound(mouse_data_, mouse_time_))
      {
         drawer.set_color(*l == mouse_data_ ? Qt::blue : Qt::red);
         drawer.draw_point(point_2f(mouse_time_, *l), 5);
      }

      if (float const * l = set_.lower_bound(mouse_data_))
      {
         drawer.set_color(*l == mouse_data_ ? Qt::red : Qt::blue);
         drawer.draw_point(point_2f(set_.current_time(), mouse_data_), 5);
      }
   }

   void print(cg::visualization::printer_type & p) const
   {
      p.corner_stream() << "insertions: " << insertions_ << cg::visualization::endl;
      p.corner_stream() << "erasures: " << erasures_ << cg::visualization::endl;
      p.corner_stream() << "nodes: " << set_.nodes() << cg::visualization::endl;
   }

   bool on_move(const cg::point_2f & pt)
   {
      mouse_time_ = pt.x;
      mouse_data_ = 20 * (int)(pt.y / 20);
      return true;
   }

   bool on_release(const point_2f & p)
   {
      if (float const * l = set_.lower_bound(mouse_data_))
      {
         if (*l == mouse_data_)
         {
            if (set_.erase(*l))
               ++erasures_;
            return true;
         }
      }

      set_.insert(mouse_data_);
      ++insertions_;

      return true;
   }

private:
   bool draw(cg::visualization::drawer_type & drawer, float start_data, float start_time,
             float finish_data, float finish_time) const
   {
      point_2f p[] = {
         {start_time, start_data},
         {finish_time, finish_data}
      };

      drawer.draw_line(p[0], p[1]);
      for (point_2f const & pt : p)
         drawer.draw_point(pt, 3);

      return start_time <= finish_time;
   }

private:
   float mouse_data_;
   float mouse_time_;
   persistent_set_t<float, size_t> & set_;

   size_t insertions_;
   size_t erasures_;
};

struct segment_t
{
   float val;
   size_t t1;
   size_t t2;
};

double test()
{
   const size_t operations = 2000;

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
            test_set.insert(e);
            break;
         }
         while (true);
      }
      else
      {
         size_t max_idx = data[l - 1].size() - 1;
         size_t idx = ufi(eng, boost::random::uniform_int_distribution<>::param_type(0, max_idx));
         float val = *boost::next(data[l].begin(), idx);

         segments.push_back({val, status[val], l});

         data[l].erase(val);
         test_set.erase(val);
      }
   }

   for (size_t l = 0; l != operations; ++l)
   {
      if (!boost::equal(test_set.slice(l), data[l]))
      {
         std::list<float> r1 = test_set.slice(l);
         std::cout << "test_set: " << r1.size() << std::endl;
//         for (float f: test_set.slice(l))
//            std::cout << " " << f;
//         std::cout << std::endl;

         std::set<float> r2 = data[l];
         std::cout << "data[l]: " << r2.size() << std::endl;

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

//         for (float f: data[l])
//            std::cout << " " << f;
//         std::cout << std::endl;

//         skip_list_viewer viewer(test_set);
//         cg::visualization::run_viewer(&viewer, "skip list");

         assert(0);
      }
   }

   return test_set.nodes() * 1. / operations;
}

int main(int argc, char ** argv)
{
   //QApplication app(argc, argv);

   double res = test();
   std::cout << "nodes / operations == " << res << std::endl;

//   skip_list_viewer viewer;
//   cg::visualization::run_viewer(&viewer, "skip list");
}

