// Copyright 2015-2017 Hans Dembinski
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <array>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <boost/histogram/axis/integer.hpp>
#include <boost/histogram/axis/regular.hpp>
#include <boost/histogram/axis/variant.hpp>
#include <boost/histogram/detail/meta.hpp>
#include <boost/histogram/histogram.hpp>
#include <boost/histogram/unlimited_storage.hpp>
#include <deque>
#include <iterator>
#include <map>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "utility_allocator.hpp"
#include "utility_meta.hpp"

using namespace boost::histogram;
using namespace boost::histogram::detail;

struct VisitorTestFunctor {
  template <typename T>
  T operator()(T&&);
};

int main() {
  // has_method_value*
  {
    struct A {};
    struct B {
      A value(int) const { return {}; }
    };
    struct C {
      char value(int) const { return 0; }
    };

    BOOST_TEST_TRAIT_FALSE((has_method_value<A>));
    BOOST_TEST_TRAIT_TRUE((has_method_value<B>));
    BOOST_TEST_TRAIT_TRUE((has_method_value<C>));

    BOOST_TEST_TRAIT_FALSE((has_method_value_with_convertible_return_type<A, double>));
    BOOST_TEST_TRAIT_TRUE((has_method_value_with_convertible_return_type<B, A>));
    BOOST_TEST_TRAIT_FALSE((has_method_value_with_convertible_return_type<B, char>));
    BOOST_TEST_TRAIT_TRUE((has_method_value_with_convertible_return_type<C, int>));
    BOOST_TEST_TRAIT_FALSE((has_method_value_with_convertible_return_type<C, A>));
  }

  // has_method_options
  {
    struct A {};
    struct B {
      void options() {}
    };

    BOOST_TEST_TRAIT_FALSE((has_method_options<A>));
    BOOST_TEST_TRAIT_TRUE((has_method_options<B>));
  }

  // has_method_metadata
  {
    struct A {};
    struct B {
      void metadata();
    };

    BOOST_TEST_TRAIT_FALSE((has_method_metadata<A>));
    BOOST_TEST_TRAIT_TRUE((has_method_metadata<B>));
  }

  // has_method_update
  {
    struct A {};
    struct B {
      void update(int) {}
    };
    using C = axis::integer<int, axis::null_type, use_default>;

    BOOST_TEST_TRAIT_FALSE((has_method_update<A>));
    BOOST_TEST_TRAIT_TRUE((has_method_update<B>));
    BOOST_TEST_TRAIT_TRUE((has_method_update<C>));
  }

  // has_method_resize
  {
    struct A {};
    using B = std::vector<int>;
    using C = std::map<int, int>;

    BOOST_TEST_TRAIT_FALSE((has_method_resize<A>));
    BOOST_TEST_TRAIT_TRUE((has_method_resize<B>));
    BOOST_TEST_TRAIT_FALSE((has_method_resize<C>));
  }

  // has_method_size
  {
    struct A {};
    using B = std::vector<int>;
    using C = std::map<int, int>;

    BOOST_TEST_TRAIT_FALSE((has_method_size<A>));
    BOOST_TEST_TRAIT_TRUE((has_method_size<B>));
    BOOST_TEST_TRAIT_TRUE((has_method_size<C>));
  }

  // has_method_clear
  {
    struct A {};
    using B = std::vector<int>;
    using C = std::map<int, int>;
    using D = std::array<int, 10>;

    BOOST_TEST_TRAIT_FALSE((has_method_clear<A>));
    BOOST_TEST_TRAIT_TRUE((has_method_clear<B>));
    BOOST_TEST_TRAIT_TRUE((has_method_clear<C>));
    BOOST_TEST_TRAIT_FALSE((has_method_clear<D>));
  }

  // has_allocator
  {
    struct A {};
    using B = std::vector<int>;
    using C = std::map<int, int>;
    using D = std::array<int, 10>;

    BOOST_TEST_TRAIT_FALSE((has_method_clear<A>));
    BOOST_TEST_TRAIT_TRUE((has_method_clear<B>));
    BOOST_TEST_TRAIT_TRUE((has_method_clear<C>));
    BOOST_TEST_TRAIT_FALSE((has_method_clear<D>));
  }

  // is_storage
  {
    struct A {};
    using B = std::vector<int>;
    using C = unlimited_storage<>;

    BOOST_TEST_TRAIT_FALSE((is_storage<A>));
    BOOST_TEST_TRAIT_FALSE((is_storage<B>));
    BOOST_TEST_TRAIT_TRUE((is_storage<C>));
  }

  // is_indexable
  {
    struct A {};
    using B = std::vector<int>;
    using C = std::map<int, int>;
    using D = std::map<A, int>;

    BOOST_TEST_TRAIT_FALSE((is_indexable<A>));
    BOOST_TEST_TRAIT_TRUE((is_indexable<B>));
    BOOST_TEST_TRAIT_TRUE((is_indexable<C>));
    BOOST_TEST_TRAIT_FALSE((is_indexable<D>));
  }

  // is_transform
  {
    struct A {};
    struct B {
      double forward(double);
      double inverse(double);
    };

    BOOST_TEST_TRAIT_FALSE((is_transform<A>));
    BOOST_TEST_TRAIT_TRUE((is_transform<B>));
  }

  // is_vector_like
  {
    struct A {};
    using B = std::vector<int>;
    using C = std::array<int, 10>;
    using D = std::map<unsigned, int>;
    using E = std::deque<int>;
    BOOST_TEST_TRAIT_FALSE((is_vector_like<A>));
    BOOST_TEST_TRAIT_TRUE((is_vector_like<B>));
    BOOST_TEST_TRAIT_FALSE((is_vector_like<C>));
    BOOST_TEST_TRAIT_FALSE((is_vector_like<D>));
    BOOST_TEST_TRAIT_TRUE((is_vector_like<E>));
  }

  // is_array_like
  {
    struct A {};
    using B = std::vector<int>;
    using C = std::array<int, 10>;
    using D = std::map<unsigned, int>;
    BOOST_TEST_TRAIT_FALSE((is_array_like<A>));
    BOOST_TEST_TRAIT_FALSE((is_array_like<B>));
    BOOST_TEST_TRAIT_TRUE((is_array_like<C>));
    BOOST_TEST_TRAIT_FALSE((is_array_like<D>));
  }

  // is_map_like
  {
    struct A {};
    using B = std::vector<int>;
    using C = std::array<int, 10>;
    using D = std::map<unsigned, int>;
    using E = std::unordered_map<unsigned, int>;
    BOOST_TEST_TRAIT_FALSE((is_map_like<A>));
    BOOST_TEST_TRAIT_FALSE((is_map_like<B>));
    BOOST_TEST_TRAIT_FALSE((is_map_like<C>));
    BOOST_TEST_TRAIT_TRUE((is_map_like<D>));
    BOOST_TEST_TRAIT_TRUE((is_map_like<E>));
  }

  // is_axis
  {
    struct A {};
    struct B {
      int index(double);
      int size() const;
    };
    struct C {
      int index(double);
    };
    struct D {
      int size();
    };
    using E = axis::variant<axis::regular<>>;

    BOOST_TEST_TRAIT_FALSE((is_axis<A>));
    BOOST_TEST_TRAIT_TRUE((is_axis<B>));
    BOOST_TEST_TRAIT_FALSE((is_axis<C>));
    BOOST_TEST_TRAIT_FALSE((is_axis<D>));
    BOOST_TEST_TRAIT_FALSE((is_axis<E>));
  }

  // is_iterable
  {
    using A = std::vector<int>;
    using B = int[3];
    using C = std::initializer_list<int>;
    BOOST_TEST_TRAIT_FALSE((is_iterable<int>));
    BOOST_TEST_TRAIT_TRUE((is_iterable<A>));
    BOOST_TEST_TRAIT_TRUE((is_iterable<B>));
    BOOST_TEST_TRAIT_TRUE((is_iterable<C>));
  }

  // is_streamable
  {
    struct Foo {};
    BOOST_TEST_TRAIT_TRUE((is_streamable<int>));
    BOOST_TEST_TRAIT_TRUE((is_streamable<std::string>));
    BOOST_TEST_TRAIT_FALSE((is_streamable<Foo>));
  }

  // is_axis_variant
  {
    struct A {};
    BOOST_TEST_TRAIT_FALSE((is_axis_variant<A>));
    BOOST_TEST_TRAIT_TRUE((is_axis_variant<axis::variant<>>));
    BOOST_TEST_TRAIT_TRUE((is_axis_variant<axis::variant<axis::regular<>>>));
  }

  // remove_cvref_t
  {
    using T1 = int;
    using T2 = int&&;
    using T3 = const int;
    using T4 = const int&;
    using T5 = volatile int;
    using T6 = volatile int&&;
    using T7 = volatile const int;
    using T8 = volatile const int&;
    BOOST_TEST_TRAIT_SAME(remove_cvref_t<T1>, int);
    BOOST_TEST_TRAIT_SAME(remove_cvref_t<T2>, int);
    BOOST_TEST_TRAIT_SAME(remove_cvref_t<T3>, int);
    BOOST_TEST_TRAIT_SAME(remove_cvref_t<T4>, int);
    BOOST_TEST_TRAIT_SAME(remove_cvref_t<T5>, int);
    BOOST_TEST_TRAIT_SAME(remove_cvref_t<T6>, int);
    BOOST_TEST_TRAIT_SAME(remove_cvref_t<T7>, int);
    BOOST_TEST_TRAIT_SAME(remove_cvref_t<T8>, int);
  }

  // copy_qualifiers
  {
    BOOST_TEST_TRAIT_SAME(copy_qualifiers<int, long>, long);
    BOOST_TEST_TRAIT_SAME(copy_qualifiers<const int, long>, const long);
    BOOST_TEST_TRAIT_SAME(copy_qualifiers<int&, long>, long&);
    BOOST_TEST_TRAIT_SAME(copy_qualifiers<const int&, long>, const long&);
    BOOST_TEST_TRAIT_SAME(copy_qualifiers<int&&, long>, long&&);
  }

  // mp_last
  {
    using L = std::tuple<int, char, long>;
    BOOST_TEST_TRAIT_SAME(mp_last<L>, long);
  }

  // args_type, arg_type
  {
    struct Foo {
      static int f1(char);
      int f2(long) const;
    };

    BOOST_TEST_TRAIT_SAME(args_type<decltype(&Foo::f1)>, std::tuple<char>);
    BOOST_TEST_TRAIT_SAME(args_type<decltype(&Foo::f2)>, std::tuple<long>);
    BOOST_TEST_TRAIT_SAME(arg_type<decltype(&Foo::f1)>, char);
    BOOST_TEST_TRAIT_SAME(arg_type<decltype(&Foo::f2)>, long);
  }

  // visitor_return_type
  {
    using V1 = axis::variant<char>;
    using V2 = axis::variant<int>&;
    using V3 = const axis::variant<long>&;
    using V4 = axis::variant<const char&>;
    using V5 = axis::variant<const char&>&;
    using V6 = const axis::variant<const char&>&;
    BOOST_TEST_TRAIT_SAME(visitor_return_type<VisitorTestFunctor, V1>, char);
    BOOST_TEST_TRAIT_SAME(visitor_return_type<VisitorTestFunctor, V2>, int&);
    BOOST_TEST_TRAIT_SAME(visitor_return_type<VisitorTestFunctor, V3>, const long&);
    BOOST_TEST_TRAIT_SAME(visitor_return_type<VisitorTestFunctor, V4>, const char&);
    BOOST_TEST_TRAIT_SAME(visitor_return_type<VisitorTestFunctor, V5>, const char&);
    BOOST_TEST_TRAIT_SAME(visitor_return_type<VisitorTestFunctor, V6>, const char&);
  }

  // static_if
  {
    struct callable {
      int operator()() { return 1; };
    };
    struct not_callable {};
    auto fcn = [](auto b, auto x) {
      return static_if<decltype(b)>([](auto x) { return x(); }, [](auto) { return 2; },
                                    x);
    };
    BOOST_TEST_EQ(fcn(std::true_type(), callable()), 1);
    BOOST_TEST_EQ(fcn(std::false_type(), not_callable()), 2);
  }

  // tuple_slice
  {
    auto a = std::make_tuple(1, 2, 3, 4);
    auto b = tuple_slice<1, 2>(a);
    BOOST_TEST_EQ(b, std::make_tuple(2, 3));
  }

  // is_sequence_of_axis
  {
    using A = std::vector<axis::regular<>>;
    using B = std::vector<axis::variant<axis::regular<>>>;
    using C = std::vector<int>;
    auto v = std::vector<axis::variant<axis::regular<>, axis::integer<>>>();
    BOOST_TEST_TRAIT_TRUE((is_sequence_of_any_axis<A>));
    BOOST_TEST_TRAIT_TRUE((is_sequence_of_axis<A>));
    BOOST_TEST_TRAIT_FALSE((is_sequence_of_axis_variant<A>));
    BOOST_TEST_TRAIT_TRUE((is_sequence_of_any_axis<B>));
    BOOST_TEST_TRAIT_TRUE((is_sequence_of_axis_variant<B>));
    BOOST_TEST_TRAIT_FALSE((is_sequence_of_axis<B>));
    BOOST_TEST_TRAIT_FALSE((is_sequence_of_any_axis<C>));
    BOOST_TEST_TRAIT_TRUE((is_sequence_of_any_axis<decltype(v)>));
  }

  // is_weight
  {
    struct A {};
    using B = int;
    using C = decltype(weight(1));
    BOOST_TEST_TRAIT_FALSE((is_weight<A>));
    BOOST_TEST_TRAIT_FALSE((is_weight<B>));
    BOOST_TEST_TRAIT_TRUE((is_weight<C>));
  }

  // is_sample
  {
    struct A {};
    using B = int;
    using C = decltype(sample(1));
    using D = decltype(sample(1, 2.0));
    BOOST_TEST_TRAIT_FALSE((is_sample<A>));
    BOOST_TEST_TRAIT_FALSE((is_sample<B>));
    BOOST_TEST_TRAIT_TRUE((is_sample<C>));
    BOOST_TEST_TRAIT_TRUE((is_sample<D>));
  }

  // make_default
  {
    struct A {};
    auto a = make_default(A());
    BOOST_TEST_TRAIT_SAME(decltype(a), A);
    tracing_allocator_db db;
    using B = std::vector<int, tracing_allocator<int>>;
    B b = make_default(B(tracing_allocator<int>(db)));
    b.resize(100);
    BOOST_TEST_EQ(db[&BOOST_CORE_TYPEID(int)].first, 100);
  }

  // has_operator_equal
  {
    struct A {};
    struct B {
      bool operator==(const B&) const { return true; }
    };

    BOOST_TEST_TRAIT_FALSE((has_operator_equal<A, A>));
    BOOST_TEST_TRAIT_FALSE((has_operator_equal<B, A>));
    BOOST_TEST_TRAIT_TRUE((has_operator_equal<B, B>));
    BOOST_TEST_TRAIT_TRUE((has_operator_equal<const B&, const B&>));
  }

  // has_operator_radd
  {
    struct A {};
    struct B {
      B& operator+=(const B&) { return *this; }
    };

    BOOST_TEST_TRAIT_FALSE((has_operator_radd<A, A>));
    BOOST_TEST_TRAIT_FALSE((has_operator_radd<B, A>));
    BOOST_TEST_TRAIT_TRUE((has_operator_radd<B, B>));
    BOOST_TEST_TRAIT_TRUE((has_operator_radd<B&, const B&>));
  }

  // get_size
  {
    std::tuple<int, int> a;
    std::vector<int> b(3);
    std::array<int, 4> c;
    const std::tuple<int> d;
    BOOST_TEST_EQ(get_size(a), 2);
    BOOST_TEST_EQ(get_size(b), 3);
    BOOST_TEST_EQ(get_size(c), 4);
    BOOST_TEST_EQ(get_size(d), 1);
  }

  return boost::report_errors();
}
