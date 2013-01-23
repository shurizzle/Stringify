#ifndef _STRINGIFY_HPP_
#define _STRINGIFY_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <typeinfo>

/* _IS_UNION(T) {{{ */
#if defined(_MSC_VER) && !defined(__MWERKS__) && !defined(__EDG_VERSION__)
# define __MSVC__ _MSC_VER
#endif

#ifdef _MSC_FULL_VER
# if _MSC_FULL_VER > 100000000
#   define __MSVC_FULL_VER__ _MSC_FULL_VER
# else
#   define __MSVC_FULL_VER__ (_MSC_FULL_VER * 10)
# endif
#endif

#if defined(__MSL_CPP__) && (__MSL_CPP__ >= 0x8000)
# include <msl_utility>
# define _IS_UNION(T) Metrowerks::is_union<T>::value
#endif

#if (defined(__MSVC__) && defined(__MSVC_FULL_VER__) && (__MSVC_FULL_VER__ >=140050215))\
    || (defined(BOOST_INTEL_CXX_VERSION) && defined(_MSC_VER) && (_MSC_VER >= 1500)) && !defined(_IS_UNION)
# define _IS_UNION(T) __is_union(T)
#endif

#if defined(__DMC__) && (__DMC__ >= 0x848) && !defined(_IS_UNION)
# define _IS_UNION(T) (__typeinfo(T) & 0x400)
#endif

#if defined(__has_feature)
# if __has_feature(is_union) && !defined(_IS_UNION)
#   define _IS_UNION(T) __is_union(T)
# endif
#endif

#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3) && !defined(__GCCXML__))) && !defined(_IS_UNION)
# define _IS_UNION(T) __is_union(T)
#endif

#if defined(__ghs__) && (__GHS_VERSION_NUMBER >= 600) && !defined(_IS_UNION)
# define _IS_UNION(T) __is_union(T)
#endif

#if defined(__CODEGEARC__) && !defined(_IS_UNION)
# define _IS_UNION(T) __is_union(T)
#endif

#ifndef _IS_UNION
# error "Cannot check union type."
#endif

/* }}} */

/* namespace Stringify {{{ */
namespace Stringify {
  /* namespace details {{{ */
  namespace details {
    template <typename type>
    class void_exp_result {};

    template <typename type, typename U>
    U const& operator,(U const&, void_exp_result<type>);

    template <typename type, typename U>
    U& operator,(U&, void_exp_result<type>);

    template <typename src_type, typename dest_type>
    struct clone_constness {
      typedef dest_type type;
    };

    template <typename src_type, typename dest_type>
    struct clone_constness<const src_type, dest_type> {
      typedef const dest_type type;
    };
  }
  /* }}} */

  /* struct is_to_string_callable<type, class_details> {{{ */
  template <typename type, typename call_details>
  struct is_to_string_callable {
  private:
    template <typename Type>
    class has_member {
      class yes { char m; };
      class no { yes m[2]; };

      struct BaseMixin {
        std::string to_string(){ return ""; };
      };
      struct Base : public Type, public BaseMixin {};

      template <typename T, T t> class Helper {};

      template <typename U>
      static no  deduce(U*, Helper<std::string (BaseMixin::*)(), &U::to_string>* = 0);
      static yes deduce(...);

    public:
      static const bool result = sizeof(yes) == sizeof(deduce((Base*)0));
    };
    class yes {};
    class no { yes m[2]; };

    struct derived : public type {
      using type::to_string;
      no to_string(...) const;
    };

    typedef typename details::clone_constness<type, derived>::type derived_type;

    template <typename T, typename due_type>
    struct return_value_check {
      static yes deduce(due_type);
      static no reduce(...);
      static no deduce(no);
      static no deduce(details::void_exp_result<type>);
    };

    template <typename T>
    struct return_value_check<T, void> {
      static yes deduce(...);
      static no deduce(no);
    };

    template <bool has, typename F>
    struct impl {
      static const bool value = false;
    };

    template <typename r>
    struct impl<true, r()>
    {
      static const bool value =
        sizeof(
            return_value_check<type, r>::deduce(
              (((derived_type*)0)->to_string(), details::void_exp_result<type>())
              )
            ) == sizeof(yes);
    };

  public:
    static const bool value = impl<has_member<type>::result, call_details>::value;
  };
  /* }}} */

  /* struct IsClass<T> {{{ */
  template <typename T>
  struct IsClass {
    typedef char yes[1];
    typedef char no[2];

    template <class U>
    static char deduce(void(U::*)(void));
    template <typename>
    static long deduce(...);

    static const bool value = sizeof(deduce<T>(0)) == sizeof(char);
  };

  /* }}} */

  /* struct PrimitiveToString<T> {{{ */
  template <typename T>
  class PrimitiveToString {
  private:
    class yes { char m; };
    class no { yes m[2]; };

    template<typename U>
    static yes deduce(size_t (*)[sizeof(std::cout << *static_cast<U*>(0))]);
    template <typename>
    static no  deduce(...);

    template <typename IT, bool b1, bool b2>
    struct _ToString {
      static std::string to_string(IT& k) {
        std::ostringstream ss (std::ostringstream::out);
        ss << k;
        return ss.str();
      }
    };

    template <typename IT>
    struct _ToString<IT, false, true> {
      static std::string to_string(IT& k) {
        char *len_name = (char *) typeid(k).name();
        for (; isdigit(*len_name); len_name++) ;
        std::ostringstream ss (std::ostringstream::out);
        ss << "#<union " << len_name << ":0x" << std::hex << (long)&k << '>';
        return ss.str();
      }
    };

    template <typename IT>
    struct _ToString<IT, false, false> {
      static std::string to_string(IT& k) {
        std::ostringstream ss (std::ostringstream::out);
        ss << "#<Unknown:0x" << std::hex << (long)&k << '>';
        return ss.str();
      }
    };

  public:
    static const bool value = sizeof(deduce<T>(NULL)) == sizeof(yes);

    static std::string to_string(T& k) {
      return _ToString<T, value, _IS_UNION(T)>::to_string(k);
    }
  };
  /* }}} */

  /* struct ClassToString<T> {{{ */
  template <typename T>
  struct ClassToString {
    class yes { char m; };
    class no { yes m[2]; };

    template<typename U>
    static yes deduce(size_t (*)[sizeof(std::cout << *static_cast<U*>(0))]);
    template <typename>
    static no  deduce(...);

    template <class IT, bool b1, bool b2>
    struct _ToString {
      static std::string to_string(IT& k) {
        return k.to_string();
      }
    };

    template <class IT>
    struct _ToString<IT, false, true> {
      static std::string to_string(IT& k) {
        std::ostringstream ss (std::ostringstream::out);
        ss << k;
        return ss.str();
      }
    };

    template <class IT>
    struct _ToString<IT, false, false> {
      static std::string to_string(IT& k) {
        char *len_name = (char *) typeid(k).name();
        for (; isdigit(*len_name); len_name++) ;
        std::ostringstream ss (std::ostringstream::out);
        ss << "#<" << len_name << ":0x" << std::hex << (long) &k << '>';
        return ss.str();
      }
    };


    static std::string to_string(T& k) {
      return _ToString<T, is_to_string_callable<T, std::string()>::value, sizeof(deduce<T>(NULL)) == sizeof(yes)>::to_string(k);
    }
  };
  /* }}} */

  /* std::string to_string<T>(T&) {{{ */
  template <typename T>
  struct ToString {
  private:
    template <typename IT, bool x>
    struct _ToString {
      static std::string to_string(IT& k) {
        return ClassToString<IT>::to_string(k);
      }
    };

    template <typename IT>
    struct _ToString<IT, false> {
      static std::string to_string(IT& k) {
        return PrimitiveToString<IT>::to_string(k);
      }
    };

    static const bool is_class = IsClass<T>::value && !_IS_UNION(T);
  public:

    static std::string to_string(T& v) {
      return _ToString<T, is_class>::to_string(v);
    }
  };

  template <typename T>
  std::string to_string(T& v) {
    return ToString<T>::to_string(v);
  }
  /* }}} */
}

/* }}} */

#endif // _STRINGIFY_HPP_
