
#ifndef CPP17
#define CPP17

#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class cCpp17
{
  public:
  // based on https://github.com/AnthonyCalandra/modern-cpp-features/blob/master/CPP17.md

//    Template argument deduction for class templates
//      Automatic template argument deduction much like how it's done for functions, but now including class constructors.
//
//      template <typename T = float>
//    struct MyContainer {
//      T val;
//      MyContainer() : val{} {}
//      MyContainer(T val) : val{ val } {}
//      // ...
//    };
//    MyContainer c1{ 1 }; // OK MyContainer<int>
//    MyContainer c2; // OK MyContainer<float>
//    Declaring non - type template parameters with auto
//      Following the deduction rules of auto, while respecting the non - type template parameter list of allowable types[*], template arguments can be deduced from the types of its arguments :
//
//    template <auto... seq>
//    struct my_integer_sequence {
//      // Implementation here ...
//    };
//
//    // Explicitly pass type `int` as template argument.
//    auto seq = std::integer_sequence<int, 0, 1, 2>();
//    // Type is deduced to be `int`.
//    auto seq2 = my_integer_sequence<0, 1, 2>();
//    *-For example, you cannot use a double as a template parameter type, which also makes this an invalid deduction using auto.
//
//      Folding expressions
//      A fold expression performs a fold of a template parameter pack over a binary operator.
//
//      An expression of the form(... op e) or (e op ...), where op is a fold - operator and e is an unexpanded parameter pack, are called unary folds.
//      An expression of the form(e1 op ... op e2), where op are fold - operators, is called a binary fold.Either e1 or e2 is an unexpanded parameter pack, but not both.
//      template <typename... Args>
//    bool logicalAnd(Args... args) {
//      // Binary folding.
//      return (true && ... && args);
//    }
//    bool b = true;
//    bool& b2 = b;
//    logicalAnd(b, b2, true); // == true
//    template <typename... Args>
//    auto sum(Args... args) {
//      // Unary folding.
//      return (... + args);
//    }
//    sum(1.0, 2.0f, 3); // == 6.0
//    New rules for auto deduction from braced - init - list
//      Changes to auto deduction when used with the uniform initialization syntax.Previously, auto x{ 3 }; deduces a std::initializer_list<int>, which now deduces to int.
//
//      auto x1{ 1, 2, 3 }; // error: not a single element
//    auto x2 = { 1, 2, 3 }; // x2 is std::initializer_list<int>
//    auto x3{ 3 }; // x3 is int
//    auto x4{ 3.0 }; // x4 is double
//    constexpr lambda
//      Compile - time lambdas using constexpr.
//
//      auto identity = [](int n) constexpr { return n; };
//    static_assert(identity(123) == 123);
//    constexpr auto add = [](int x, int y) {
//      auto L = [=] { return x; };
//      auto R = [=] { return y; };
//      return [=] { return L() + R(); };
//    };
//
//    static_assert(add(1, 2)() == 3);
//    constexpr int addOne(int n) {
//      return [n] { return n + 1; }();
//    }
//
//    static_assert(addOne(1) == 2);
//    Lambda capture this by value
//      Capturing this in a lambda's environment was previously reference-only. An example of where this is problematic is asynchronous code using callbacks that require an object to be available, potentially past its lifetime. *this (C++17) will now make a copy of the current object, while this (C++11) continues to capture by reference.
//
//      struct MyObj {
//      int value{ 123 };
//      auto getValueCopy() {
//        return[*this]{ return value; };
//      }
//      auto getValueRef() {
//        return [this] { return value; };
//      }
//    };
//    MyObj mo;
//    auto valueCopy = mo.getValueCopy();
//    auto valueRef = mo.getValueRef();
//    mo.value = 321;
//    valueCopy(); // 123
//    valueRef(); // 321
//    Inline variables
//      The inline specifier can be applied to variables as well as to functions.A variable declared inline has the same semantics as a function declared inline.
//
//      // Disassembly example using compiler explorer.
//      struct S { int x; };
//    inline S x1 = S{ 321 }; // mov esi, dword ptr [x1]
//                          // x1: .long 321
//
//    S x2 = S{ 123 };        // mov eax, dword ptr [.L_ZZ4mainE2x2]
//                          // mov dword ptr [rbp - 8], eax
//                          // .L_ZZ4mainE2x2: .long 123
//    It can also be used to declareand define a static member variable, such that it does not need to be initialized in the source file.
//
//      struct S {
//      S() : id{ count++ } {}
//      ~S() { count--; }
//      int id;
//      static inline int count{ 0 }; // declare and initialize count to 0 within the class
//    };
//    Nested namespaces
//      Using the namespace resolution operator to create nested namespace definitions.
//
//      namespace A {
//      namespace B {
//        namespace C {
//          int i;
//        }
//      }
//    }
//    The code above can be written like this:
//
//    namespace A::B::C {
//      int i;
//    }
//    Structured bindings
//      A proposal for de - structuring initialization, that would allow writing auto [x, y, z] = expr; where the type of expr was a tuple - like object, whose elements would be bound to the variables x, y, and z(which this construct declares).Tuple - like objects include std::tuple, std::pair, std::array, and aggregate structures.
//
//      using Coordinate = std::pair<int, int>;
//    Coordinate origin() {
//      return Coordinate{ 0, 0 };
//    }
//
//    const auto [x, y] = origin();
//    x; // == 0
//    y; // == 0
//    std::unordered_map<std::string, int> mapping{
//      {"a", 1},
//      {"b", 2},
//      {"c", 3}
//    };
//
//    // Destructure by reference.
//    for (const auto& [key, value] : mapping) {
//      // Do something with key and value
//    }
//    Selection statements with initializer
//      New versions of the ifand switch statements which simplify common code patternsand help users keep scopes tight.
//
//    {
//      std::lock_guard<std::mutex> lk(mx);
//      if (v.empty()) v.push_back(val);
//    }
//    // vs.
//    if (std::lock_guard<std::mutex> lk(mx); v.empty()) {
//      v.push_back(val);
//    }
//    Foo gadget(args);
//    switch (auto s = gadget.status()) {
//    case OK: gadget.zip(); break;
//    case Bad: throw BadFoo(s.message());
//    }
//    // vs.
//    switch (Foo gadget(args); auto s = gadget.status()) {
//    case OK: gadget.zip(); break;
//    case Bad: throw BadFoo(s.message());
//    }
//    constexpr if
//      Write code that is instantiated depending on a compile - time condition.
//
//      template <typename T>
//    constexpr bool isIntegral() {
//      if constexpr (std::is_integral<T>::value) {
//        return true;
//      }
//      else {
//        return false;
//      }
//    }
//    static_assert(isIntegral<int>() == true);
//    static_assert(isIntegral<char>() == true);
//    static_assert(isIntegral<double>() == false);
//    struct S {};
//    static_assert(isIntegral<S>() == false);
//    UTF - 8 character literals
//      A character literal that begins with u8 is a character literal of type char.The value of a UTF - 8 character literal is equal to its ISO 10646 code point value.
//
//      char x = u8'x';
//    Direct list initialization of enums
//      Enums can now be initialized using braced syntax.
//
//      enum byte : unsigned char {};
//    byte b{ 0 }; // OK
//    byte c{ -1 }; // ERROR
//    byte d = byte{ 1 }; // OK
//    byte e = byte{ 256 }; // ERROR
//    [[fallthrough]], [[nodiscard]], [[maybe_unused]] attributes
//      C++17 introduces three new attributes: [[fallthrough]], [[nodiscard]] and [[maybe_unused]].
//
//      [[fallthrough]] indicates to the compiler that falling through in a switch statement is intended behavior.This attribute may only be used in a switch statement, and must be placed before the next case / default label.
//      switch (n) {
//      case 1:
//        // ...
//        [[fallthrough]];
//      case 2:
//        // ...
//        break;
//      case 3:
//        // ...
//        [[fallthrough]];
//      default:
//        // ...
//      }
//    [[nodiscard]] issues a warning when either a function or class has this attribute and its return value is discarded.
//      [[nodiscard]] bool do_something() {
//      return is_success; // true for success, false for failure
//    }
//
//    do_something(); // warning: ignoring return value of 'bool do_something()',
//                    // declared with attribute 'nodiscard'
//    // Only issues a warning when `error_info` is returned by value.
//    struct [[nodiscard]] error_info {
//      // ...
//    };
//
//    error_info do_something() {
//      error_info ei;
//      // ...
//      return ei;
//    }
//
//    do_something(); // warning: ignoring returned value of type 'error_info',
//                    // declared with attribute 'nodiscard'
//    [[maybe_unused]] indicates to the compiler that a variable or parameter might be unused and is intended.
//      void my_callback(std::string msg, [[maybe_unused]] bool error) {
//      // Don't care if `msg` is an error message, just log it.
//      log(msg);
//    }
//    __has_include
//      __has_include(operand) operator may be used in #if and #elif expressions to check whether a header or source file(operand) is available for inclusion or not.
//
//      One use case of this would be using two libraries that work the same way, using the backup / experimental one if the preferred one is not found on the system.
//
//#ifdef __has_include
//#  if __has_include(<optional>)
//#    include <optional>
//#    define have_optional 1
//#  elif __has_include(<experimental/optional>)
//#    include <experimental/optional>
//#    define have_optional 1
//#    define experimental_optional
//#  else
//#    define have_optional 0
//#  endif
//#endif
//      It can also be used to include headers existing under different names or locations on various platforms, without knowing which platform the program is running on, OpenGL headers are a good example for this which are located in OpenGL\ directory on macOSand GL\ on other platforms.
//
//#ifdef __has_include
//#  if __has_include(<OpenGL/gl.h>)
//#    include <OpenGL/gl.h>
//#    include <OpenGL/glu.h>
//#  elif __has_include(<GL/gl.h>)
//#    include <GL/gl.h>
//#    include <GL/glu.h>
//#  else
//#    error No suitable OpenGL headers found.
//# endif
//#endif
//      Class template argument deduction
//      Class template argument deduction(CTAD) allows the compiler to deduce template arguments from constructor arguments.
//
//      std::vector v{ 1, 2, 3 }; // deduces std::vector<int>
//
//    std::mutex mtx;
//    auto lck = std::lock_guard{ mtx }; // deduces to std::lock_guard<std::mutex>
//
//    auto p = new std::pair{ 1.0, 2.0 }; // deduces to std::pair<double, double>*
//    For user - defined types, deduction guides can be used to guide the compiler how to deduce template arguments if applicable:
//
//    template <typename T>
//    struct container {
//      container(T t) {}
//
//      template <typename Iter>
//      container(Iter beg, Iter end);
//    };
//
//    // deduction guide
//    template <typename Iter>
//    container(Iter b, Iter e)->container<typename std::iterator_traits<Iter>::value_type>;
//
//    container a{ 7 }; // OK: deduces container<int>
//
//    std::vector<double> v{ 1.0, 2.0, 3.0 };
//    auto b = container{ v.begin(), v.end() }; // OK: deduces container<double>
//
//    container c{ 5, 6 }; // ERROR: std::iterator_traits<int>::value_type is not a type
//    C++17 Library Features
//      std::variant
//      The class template std::variant represents a type - safe union.An instance of std::variant at any given time holds a value of one of its alternative types(it's also possible for it to be valueless).
//
//        std::variant<int, double> v{ 12 };
//    std::get<int>(v); // == 12
//    std::get<0>(v); // == 12
//    v = 12.0;
//    std::get<double>(v); // == 12.0
//    std::get<1>(v); // == 12.0
//    std::optional
//      The class template std::optional manages an optional contained value, i.e.a value that may or may not be present.A common use case for optional is the return value of a function that may fail.
//
//      std::optional<std::string> create(bool b) {
//      if (b) {
//        return "Godzilla";
//      }
//      else {
//        return {};
//      }
//    }
//
//    create(false).value_or("empty"); // == "empty"
//    create(true).value(); // == "Godzilla"
//    // optional-returning factory functions are usable as conditions of while and if
//    if (auto str = create(true)) {
//      // ...
//    }
//    std::any
//      A type - safe container for single values of any type.
//
//      std::any x{ 5 };
//    x.has_value() // == true
//      std::any_cast<int>(x) // == 5
//      std::any_cast<int&>(x) = 10;
//    std::any_cast<int>(x) // == 10
//      std::string_view
//      A non - owning reference to a string.Useful for providing an abstraction on top of strings(e.g. for parsing).
//
//      // Regular strings.
//      std::string_view cppstr{ "foo" };
//    // Wide strings.
//    std::wstring_view wcstr_v{ L"baz" };
//    // Character arrays.
//    char array[3] = { 'b', 'a', 'r' };
//    std::string_view array_v(array, std::size(array));
//    std::string str{ "   trim me" };
//    std::string_view v{ str };
//    v.remove_prefix(std::min(v.find_first_not_of(" "), v.size()));
//    str; //  == "   trim me"
//    v; // == "trim me"
//    std::invoke
//      Invoke a Callable object with parameters.Examples of callable objects are std::function or lambdas; objects that can be called similarly to a regular function.
//
//      template <typename Callable>
//    class Proxy {
//      Callable c_;
//
//    public:
//      Proxy(Callable c) : c_{ std::move(c) } {}
//
//      template <typename... Args>
//      decltype(auto) operator()(Args&&... args) {
//        // ...
//        return std::invoke(c_, std::forward<Args>(args)...);
//      }
//    };
//
//    const auto add = [](int x, int y) { return x + y; };
//    Proxy p{ add };
//    p(1, 2); // == 3
//    std::apply
//      Invoke a Callable object with a tuple of arguments.
//
//      auto add = [](int x, int y) {
//      return x + y;
//    };
//    std::apply(add, std::make_tuple(1, 2)); // == 3
//    std::filesystem
//      The new std::filesystem library provides a standard way to manipulate files, directories, and paths in a filesystem.
//
//      Here, a big file is copied to a temporary path if there is available space :
//
//    const auto bigFilePath{ "bigFileToCopy" };
//    if (std::filesystem::exists(bigFilePath)) {
//      const auto bigFileSize{ std::filesystem::file_size(bigFilePath) };
//      std::filesystem::path tmpPath{ "/tmp" };
//      if (std::filesystem::space(tmpPath).available > bigFileSize) {
//        std::filesystem::create_directory(tmpPath.append("example"));
//        std::filesystem::copy_file(bigFilePath, tmpPath.append("newFile"));
//      }
//    }
//    std::byte
//      The new std::byte type provides a standard way of representing data as a byte.Benefits of using std::byte over char or unsigned char is that it is not a character type, and is also not an arithmetic type; while the only operator overloads available are bitwise operations.
//
//      std::byte a{ 0 };
//    std::byte b{ 0xFF };
//    int i = std::to_integer<int>(b); // 0xFF
//    std::byte c = a & b;
//    int j = std::to_integer<int>(c); // 0
//    Note that std::byte is simply an enum, and braced initialization of enums become possible thanks to direct - list - initialization of enums.
//
//      Splicing for mapsand sets
//      Moving nodesand merging containers without the overhead of expensive copies, moves, or heap allocations / deallocations.
//
//      Moving elements from one map to another :
//
//    std::map<int, string> src{ {1, "one"}, {2, "two"}, {3, "buckle my shoe"} };
//    std::map<int, string> dst{ {3, "three"} };
//    dst.insert(src.extract(src.find(1))); // Cheap remove and insert of { 1, "one" } from `src` to `dst`.
//    dst.insert(src.extract(2)); // Cheap remove and insert of { 2, "two" } from `src` to `dst`.
//    // dst == { { 1, "one" }, { 2, "two" }, { 3, "three" } };
//    Inserting an entire set :
//
//    std::set<int> src{ 1, 3, 5 };
//    std::set<int> dst{ 2, 4, 5 };
//    dst.merge(src);
//    // src == { 5 }
//    // dst == { 1, 2, 3, 4, 5 }
//    Inserting elements which outlive the container :
//
//    auto elementFactory() {
//      std::set<...> s;
//      s.emplace(...);
//      return s.extract(s.begin());
//    }
//    s2.insert(elementFactory());
//    Changing the key of a map element :
//
//    std::map<int, string> m{ {1, "one"}, {2, "two"}, {3, "three"} };
//    auto e = m.extract(2);
//    e.key() = 4;
//    m.insert(std::move(e));
//    // m == { { 1, "one" }, { 3, "three" }, { 4, "two" } }
//    Parallel algorithms
//      Many of the STL algorithms, such as the copy, findand sort methods, started to support the parallel execution policies : seq, parand par_unseq which translate to "sequentially", "parallel" and "parallel unsequenced".
//
//      std::vector<int> longVector;
//    // Find element using parallel execution policy
//    auto result1 = std::find(std::execution::par, std::begin(longVector), std::end(longVector), 2);
//    // Sort elements using sequential execution policy
//    auto result2 = std::sort(std::execution::seq, std::begin(longVector), std::end(longVector));
//    std::sample
//      Samples n elements in the given sequence(without replacement) where every element has an equal chance of being selected.
//
//      const std::string ALLOWED_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
//    std::string guid;
//    // Sample 5 characters from ALLOWED_CHARS.
//    std::sample(ALLOWED_CHARS.begin(), ALLOWED_CHARS.end(), std::back_inserter(guid),
//      5, std::mt19937{ std::random_device{}() });
//
//    std::cout << guid; // e.g. G1fW2
//    std::clamp
//      Clamp given value between a lowerand upper bound.
//
//      std::clamp(42, -1, 1); // == 1
//    std::clamp(-42, -1, 1); // == -1
//    std::clamp(0, -1, 1); // == 0
//
//    // `std::clamp` also accepts a custom comparator:
//    std::clamp(0, -1, 1, std::less<>{}); // == 0
//    std::reduce
//      Fold over a given range of elements.Conceptually similar to std::accumulate, but std::reduce will perform the fold in parallel.Due to the fold being done in parallel, if you specify a binary operation, it is required to be associativeand commutative.A given binary operation also should not change any element or invalidate any iterators within the given range.
//
//      The default binary operation is std::plus with an initial value of 0.
//
//      const std::array<int, 3> a{ 1, 2, 3 };
//    std::reduce(std::cbegin(a), std::cend(a)); // == 6
//    // Using a custom binary op:
//    std::reduce(std::cbegin(a), std::cend(a), 1, std::multiplies<>{}); // == 6
//    Additionally you can specify transformations for reducers:
//
//    std::transform_reduce(std::cbegin(a), std::cend(a), 0, std::plus<>{}, times_ten); // == 60
//
//    const std::array<int, 3> b{ 1, 2, 3 };
//    const auto product_times_ten = [](const auto a, const auto b) { return a * b * 10; };
//
//    std::transform_reduce(std::cbegin(a), std::cend(a), std::cbegin(b), 0, std::plus<>{}, product_times_ten); // == 140
//    Prefix sum algorithms
//      Support for prefix sums(both inclusive and exclusive scans) along with transformations.
//
//      const std::array<int, 3> a{ 1, 2, 3 };
//
//    std::inclusive_scan(std::cbegin(a), std::cend(a),
//      std::ostream_iterator<int>{ std::cout, " " }, std::plus<>{}); // 1 3 6
//
//    std::exclusive_scan(std::cbegin(a), std::cend(a),
//      std::ostream_iterator<int>{ std::cout, " " }, 0, std::plus<>{}); // 0 1 3
//
//    const auto times_ten = [](const auto n) { return n * 10; };
//
//    std::transform_inclusive_scan(std::cbegin(a), std::cend(a),
//      std::ostream_iterator<int>{ std::cout, " " }, std::plus<>{}, times_ten); // 10 30 60
//
//    std::transform_exclusive_scan(std::cbegin(a), std::cend(a),
//      std::ostream_iterator<int>{ std::cout, " " }, 0, std::plus<>{}, times_ten); // 0 10 30
//    GCDand LCM
//      Greatest common divisor(GCD) and least common multiple(LCM).
//
//      const int p = 9;
//    const int q = 3;
//    std::gcd(p, q); // == 3
//    std::lcm(p, q); // == 9
//    std::not_fn
//      Utility function that returns the negation of the result of the given function.
//
//      const std::ostream_iterator<int> ostream_it{ std::cout, " " };
//    const auto is_even = [](const auto n) { return n % 2 == 0; };
//    std::vector<int> v{ 0, 1, 2, 3, 4 };
//
//    // Print all even numbers.
//    std::copy_if(std::cbegin(v), std::cend(v), ostream_it, is_even); // 0 2 4
//    // Print all odd (not even) numbers.
//    std::copy_if(std::cbegin(v), std::cend(v), ostream_it, std::not_fn(is_even)); // 1 3
//    String conversion to / from numbers
//      Convert integrals and floats to a string or vice - versa.Conversions are non - throwing, do not allocate, and are more secure than the equivalents from the C standard library.
//
//      Users are responsible for allocating enough storage required for std::to_chars, or the function will fail by setting the error code object in its return value.
//
//      These functions allow you to optionally pass a base(defaults to base - 10) or a format specifier for floating type input.
//
//      std::to_chars returns a(non - const) char pointer which is one - past - the - end of the string that the function wrote to inside the given buffer, and an error code object.
//      std::from_chars returns a const char pointer which on success is equal to the end pointer passed to the function, and an error code object.
//      Both error code objects returned from these functions are equal to the default - initialized error code object on success.
//
//      Convert the number 123 to a std::string:
//
//    const int n = 123;
//
//    // Can use any container, string, array, etc.
//    std::string str;
//    str.resize(3); // hold enough storage for each digit of `n`
//
//    const auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), n);
//
//    if (ec == std::errc{}) { std::cout << str << std::endl; } // 123
//    else { /* handle failure */ }
//    Convert from a std::string with value "123" to an integer :
//
//    const std::string str{ "123" };
//    int n;
//
//    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), n);
//
//    if (ec == std::errc{}) { std::cout << n << std::endl; } // 123
//    else { /* handle failure */ }
//    Rounding functions for chrono durationsand timepoints
//      Provides abs, round, ceil, and floor helper functions for std::chrono::durationand std::chrono::time_point.
//
//      using seconds = std::chrono::seconds;
//    std::chrono::milliseconds d{ 5500 };
//    std::chrono::abs(d); // == 5s
//    std::chrono::round<seconds>(d); // == 6s
//    std::chrono::ceil<seconds>(d); // == 6s
//    std::chrono::floor<seconds>(d); // == 5s

};



#endif  //CPP17


