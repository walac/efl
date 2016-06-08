#ifndef EOLIAN_CXX_KLASS_DEF_HH
#define EOLIAN_CXX_KLASS_DEF_HH

#include "grammar/type_traits.hpp"
#include "grammar/variant.hpp"
#include "grammar/attributes.hpp"
#include "grammar/qualifier_def.hpp"
#include "grammar/string.hpp"
#include "grammar/sequence.hpp"
#include "grammar/kleene.hpp"
#include "grammar/case.hpp"

#include <Eolian.h>

#include <Eina.hh>

#include <vector>
#include <memory>
#include <set>

namespace efl { namespace eolian { namespace grammar { namespace attributes {

template <typename...Args, std::size_t I>
bool lexicographical_compare_impl(std::tuple<Args...> const&
                                  , std::tuple<Args...> const&
                                  , std::integral_constant<std::size_t, I>
                                  , std::true_type)
{
  return true;
}
template <typename...Args, std::size_t I>
bool lexicographical_compare_impl(std::tuple<Args...> const& lhs
                                  , std::tuple<Args...> const& rhs
                                  , std::integral_constant<std::size_t, I>
                                  , std::false_type)
{
  return std::get<I>(lhs) < std::get<I>(rhs)
      || (!(std::get<I>(rhs) < std::get<I>(lhs))
          && lexicographical_compare_impl(lhs, rhs, std::integral_constant<std::size_t, I+1>()
                                          , std::integral_constant<bool, I + 1 == sizeof...(Args)>())
         )
    ;
}
template <typename...Args>
bool lexicographical_compare(std::tuple<Args...> const& lhs
                             , std::tuple<Args...> const& rhs)
{
  return lexicographical_compare_impl(lhs, rhs, std::integral_constant<std::size_t, 0ul>(), std::false_type());
}
template <typename T, typename U>
bool lexicographical_compare(std::tuple<T, U> const& lhs
                             , std::tuple<T, U> const& rhs)
{
  return std::get<0>(lhs) < std::get<0>(rhs)
       || (!(std::get<0>(rhs) < std::get<0>(lhs))
          && std::get<1>(lhs) < std::get<1>(rhs));
}
        
struct pointer_indirection
{
   qualifier_def qualifier;
   bool reference;
};
inline bool operator<(pointer_indirection const& lhs, pointer_indirection const& rhs)
{
  return lexicographical_compare(std::make_tuple(lhs.qualifier, lhs.reference)
                                 , std::make_tuple(rhs.qualifier, rhs.reference));
}
inline bool operator==(pointer_indirection const& lhs, pointer_indirection const& rhs)
{
  return lhs.qualifier == rhs.qualifier && lhs.reference == rhs.reference;
}
inline bool operator!=(pointer_indirection const& lhs, pointer_indirection const& rhs)
{
  return !(lhs == rhs);
}

struct type_def;
bool operator==(type_def const& rhs, type_def const& lhs);
bool operator!=(type_def const& rhs, type_def const& lhs);
        
enum class class_type
{
  regular, abstract_, mixin, interface_
};
        
struct klass_name
{
   std::vector<std::string> namespaces;
   std::string eolian_name;
   qualifier_def base_qualifier;
   std::vector<pointer_indirection> pointers;
   class_type type;

   klass_name(std::vector<std::string> namespaces
              , std::string eolian_name, qualifier_def base_qualifier
              , std::vector<pointer_indirection> pointers
              , class_type type)
     : namespaces(namespaces), eolian_name(eolian_name), base_qualifier(base_qualifier)
     , pointers(pointers), type(type) {}
   klass_name(Eolian_Class const* klass, qualifier_def base_qualifier
              , std::vector<pointer_indirection> pointers)
     : eolian_name( ::eolian_class_name_get(klass))
     , base_qualifier(base_qualifier), pointers(pointers)
   {
     for(efl::eina::iterator<const char> namespace_iterator ( ::eolian_class_namespaces_get(klass))
           , namespace_last; namespace_iterator != namespace_last; ++namespace_iterator)
       {
         namespaces.push_back(&*namespace_iterator);
       }
     switch(eolian_class_type_get(klass))
       {
       case EOLIAN_CLASS_REGULAR:
         type = class_type::regular;
         break;
       case EOLIAN_CLASS_ABSTRACT:
         type = class_type::abstract_;
         break;
       case EOLIAN_CLASS_MIXIN:
         type = class_type::mixin;
         break;
       case EOLIAN_CLASS_INTERFACE:
         type = class_type::interface_;
         break;
       default:
         throw std::runtime_error("Class with unknown type");
       }
   }
};

inline bool operator==(klass_name const& lhs, klass_name const& rhs)
{
  return lhs.namespaces == rhs.namespaces && lhs.eolian_name == rhs.eolian_name
    && lhs.base_qualifier == rhs.base_qualifier && lhs.pointers == rhs.pointers;
}
inline bool operator!=(klass_name const& lhs, klass_name const& rhs)
{
  return !(lhs == rhs);
}
inline bool operator<(klass_name const& lhs, klass_name const& rhs)
{
  typedef std::tuple<std::vector<std::string>const&
                     , std::string const&
                     , qualifier_def const&
                     , std::vector<pointer_indirection> const&
                     , class_type
                     > tuple_type;
  return lexicographical_compare(tuple_type(lhs.namespaces, lhs.eolian_name
                                            , lhs.base_qualifier, lhs.pointers
                                            , lhs.type)
                                 , tuple_type(rhs.namespaces, rhs.eolian_name
                                              , rhs.base_qualifier, rhs.pointers
                                              , rhs.type));
}

template <>
struct tuple_element<0ul, klass_name>
{
  typedef std::vector<std::string> type;
  static type& get(klass_name& klass) { return klass.namespaces; }
  static type const& get(klass_name const& klass) { return klass.namespaces; }
};
template <>
struct tuple_element<1ul, klass_name>
{
  typedef std::string type;
  static type& get(klass_name& klass) { return klass.eolian_name; }
  static type const& get(klass_name const& klass) { return klass.eolian_name; }
};
template <int N>
struct tuple_element<N, klass_name const> : tuple_element<N, klass_name> {};

template <int N>
typename tuple_element<N, klass_name>::type&
get(klass_name& klass)
{
  return tuple_element<N, klass_name>::get(klass);
}
template <int N>
typename tuple_element<N, klass_name>::type const&
get(klass_name const& klass)
{
  return tuple_element<N, klass_name>::get(klass);
}
        
struct regular_type_def
{
   std::string base_type;
   qualifier_def base_qualifier;
   std::vector<pointer_indirection> pointers;
   std::vector<std::string> namespaces;
};

inline bool operator==(regular_type_def const& rhs, regular_type_def const& lhs)
{
  return rhs.base_type == lhs.base_type && rhs.base_qualifier == lhs.base_qualifier
    && rhs.pointers == lhs.pointers;
}
inline bool operator!=(regular_type_def const& rhs, regular_type_def const& lhs)
{
  return !(rhs == lhs);
}

struct complex_type_def
{
   regular_type_def outer;
   std::vector<type_def> subtypes;
};

inline bool operator==(complex_type_def const& lhs, complex_type_def const& rhs)
{
  return lhs.outer == rhs.outer && lhs.subtypes == rhs.subtypes;
}
inline bool operator!=(complex_type_def const& lhs, complex_type_def const& rhs)
{
  return !(lhs == rhs);
}

struct type_def
{
   typedef attributes::variant<klass_name, regular_type_def, complex_type_def> variant_type;
   variant_type original_type;
   std::string c_type;

   type_def() {}
   type_def(variant_type original_type, std::string c_type)
     : original_type(original_type), c_type(c_type) {}

   type_def(Eolian_Type const* eolian_type)
   {
     set(eolian_type);
   }
   struct set_pointer_visitor
   {
      typedef void result_type;
      std::vector<pointer_indirection> pointers;
      template <typename T>
      void operator()(T& v) const
      {
        v.pointers = pointers;
      }
      void operator()(complex_type_def& complex) const
      {
        (*this)(complex.outer);
      }
   };
   void set(Eolian_Type const* eolian_type);
};

inline bool operator==(type_def const& lhs, type_def const& rhs)
{
  return lhs.original_type == rhs.original_type && lhs.c_type == rhs.c_type;
}
inline bool operator!=(type_def const& lhs, type_def const& rhs)
{
  return !(lhs == rhs);
}
        
type_def const void_ {attributes::regular_type_def{"void", {qualifier_info::is_none, {}}, {}}, "void"};
        
inline void type_def::set(Eolian_Type const* eolian_type)
{
   c_type = ::eolian_type_c_type_get(eolian_type);
   // ::eina_stringshare_del(stringshare); // this crashes
   switch( ::eolian_type_type_get(eolian_type))
     {
     case EOLIAN_TYPE_VOID:
       original_type = attributes::regular_type_def{"void", {qualifiers(eolian_type), {}}, {}};
       break;
     case EOLIAN_TYPE_REGULAR:
       {
         std::vector<std::string> namespaces;
         for(efl::eina::iterator<const char> namespace_iterator( ::eolian_type_namespaces_get(eolian_type))
               , namespace_last; namespace_iterator != namespace_last; ++namespace_iterator)
           namespaces.push_back(&*namespace_iterator);
         original_type = {regular_type_def{ ::eolian_type_name_get(eolian_type), {qualifiers(eolian_type), {}}, {}, namespaces}};
       }
       break;
     case EOLIAN_TYPE_POINTER:
       {
          std::vector<pointer_indirection> pointers
          {{ {qualifiers(eolian_type), {}}, false }};
          Eolian_Type const* base_type = eolian_type_base_type_get(eolian_type);
          while(eolian_type_type_get(base_type) == EOLIAN_TYPE_POINTER)
            {
               pointers.push_back({{qualifiers(base_type), {}}});
               base_type = eolian_type_base_type_get(base_type);
            }

          set(base_type);
          original_type.visit(set_pointer_visitor{pointers});
          c_type = ::eolian_type_c_type_get(eolian_type);
          break;
       }
     case EOLIAN_TYPE_CLASS:
       {
          Eolian_Class const* klass = eolian_type_class_get(eolian_type);
          original_type = klass_name(klass, {qualifiers(eolian_type), {}}, {});
       }
       break;
     case EOLIAN_TYPE_COMPLEX:
       {
         complex_type_def complex
          {{::eolian_type_name_get(eolian_type), {qualifiers(eolian_type), {}}, {}}, {}};
         for(efl::eina::iterator<Eolian_Type const> type_iterator( ::eolian_type_subtypes_get(eolian_type))
               , type_last; type_iterator != type_last; ++type_iterator)
           {
             Eolian_Type const* type = &*type_iterator;
             complex.subtypes.push_back({type});
           }
         original_type = complex;
       }
       break;
     default:
       std::abort();
       break;
     }
}

enum class parameter_direction
{
  in, inout, out
};

namespace detail {
struct add_optional_qualifier_visitor
{
  typedef void result_type;
  template <typename T>
  void operator()(T&  object) const
  {
    add_optional(object.base_qualifier);
  }
  void operator()(complex_type_def& complex) const
  {
    (*this)(complex.outer);
  }
};
}
        
struct parameter_def
{
  parameter_direction direction;
  type_def type;
  std::string param_name;
  std::string c_type;

  parameter_def(parameter_direction direction, type_def type, std::string param_name, std::string c_type)
    : direction(direction), type(type), param_name(param_name), c_type(c_type) {}
  parameter_def(Eolian_Function_Parameter const* param)
    : type( ::eolian_parameter_type_get(param))
    , param_name( ::eolian_parameter_name_get(param))
    , c_type( ::eolian_type_c_type_get(::eolian_parameter_type_get(param)))
  {
     Eolian_Parameter_Dir direction = ::eolian_parameter_direction_get(param);
     switch(direction)
       {
       case EOLIAN_IN_PARAM:
         this->direction = parameter_direction::in;
         break;
       case EOLIAN_INOUT_PARAM:
         this->direction = parameter_direction::inout;
         break;
       case EOLIAN_OUT_PARAM:
         this->direction = parameter_direction::out;
         break;
       }
     if( ::eolian_parameter_is_optional(param))
       type.original_type.visit(detail::add_optional_qualifier_visitor{});
  }
};

template <>
struct tuple_element<0ul, parameter_def>
{
  typedef parameter_direction type;
  static type const& get(parameter_def const& p) { return p.direction; }
  static type& get(parameter_def& p) { return p.direction; }
};
template <>
struct tuple_element<1ul, parameter_def>
{
  typedef type_def type;
  static type const& get(parameter_def const& p) { return p.type; }
  static type& get(parameter_def& p) { return p.type; }
};
template <>
struct tuple_element<2ul, parameter_def>
{
  typedef std::string type;
  static type const& get(parameter_def const& p) { return p.param_name; }
  static type& get(parameter_def& p) { return p.param_name; }
};
template <>
struct tuple_element<3ul, parameter_def>
{
  typedef std::string type;
  static type const& get(parameter_def const& p) { return p.c_type; }
  static type& get(parameter_def& p) { return p.c_type; }
};
template <int I>
typename tuple_element<I, parameter_def>::type const& get(parameter_def const& p)
{ return tuple_element<I, parameter_def>::get(p); }
template <int I>
typename tuple_element<I, parameter_def>::type& get(parameter_def& p)
{ return tuple_element<I, parameter_def>::get(p); }

struct function_def
{
  type_def return_type;
  std::string name;
  std::vector<parameter_def> parameters;
  std::string c_name;
  bool is_beta;
  bool is_protected;

  function_def(type_def return_type, std::string name, std::vector<parameter_def> parameters
               , std::string c_name, bool is_beta)
    : return_type(return_type), name(name), parameters(parameters), c_name(c_name), is_beta(is_beta) {}
  function_def() = default;
  function_def( ::Eolian_Function const* function, Eolian_Function_Type type)
    : return_type(void_)
  {
    Eolian_Type const* r_type = ::eolian_function_return_type_get(function, type);
    name = ::eolian_function_name_get(function);
    if(r_type)
      return_type.set(r_type);
     if(type == EOLIAN_METHOD)
       {
          for(efl::eina::iterator<Eolian_Function_Parameter> param_iterator ( ::eolian_function_parameters_get(function))
            , param_last; param_iterator != param_last; ++param_iterator)
            {
               parameters.push_back(&*param_iterator);
            }
       }
     else if(type == EOLIAN_PROP_GET || type == EOLIAN_PROP_SET)
       {
         if(type == EOLIAN_PROP_GET)
           name += "_get";
         else
           name += "_set";
         for(efl::eina::iterator<Eolian_Function_Parameter> param_iterator
               ( ::eolian_property_keys_get(function, type))
               , param_last; param_iterator != param_last; ++param_iterator)
           {
              parameters.push_back(&*param_iterator);
           }
         std::vector<parameter_def> values;
         for(efl::eina::iterator<Eolian_Function_Parameter> param_iterator
               ( ::eolian_property_values_get(function, type))
               , param_last; param_iterator != param_last; ++param_iterator)
           {
              values.push_back(&*param_iterator);
           }

         if(type == EOLIAN_PROP_GET && values.size() == 1 && return_type == void_)
           {
             return_type = values[0].type;
           }
         else if(type == EOLIAN_PROP_GET)
           {
             for(auto&& v : values)
               {
                 v.direction = parameter_direction::out;
                 parameters.push_back(v);
               }
           }
         else
           parameters.insert(parameters.end(), values.begin(), values.end());
       }
     c_name = eolian_function_full_c_name_get(function, type, EINA_FALSE);
     is_beta = eolian_function_is_beta(function);
     is_protected = eolian_function_scope_get(function) == EOLIAN_SCOPE_PROTECTED;
  }
};

template <>
struct tuple_element<0ul, function_def>
{
   typedef type_def type;
   static type& get(function_def& f) { return f.return_type; }
   static type const& get(function_def const& f) { return f.return_type; }
};

template <>
struct tuple_element<1ul, function_def>
{
   typedef std::string type;
   static type& get(function_def& f) { return f.name; }
   static type const& get(function_def const& f) { return f.name; }
};

template <>
struct tuple_element<2ul, function_def>
{
   typedef std::vector<parameter_def> type;
   static type& get(function_def& f) { return f.parameters; }
   static type const& get(function_def const& f) { return f.parameters; }
};

// template <int N>
// struct tuple_element<N, function_def const> : tuple_element<N, function_def> {};
// template <int N>
// struct tuple_element<N, function_def&> : tuple_element<N, function_def> {};
// template <int N>
// struct tuple_element<N, function_def const&> : tuple_element<N, function_def> {};

// template <std::size_t I>
// typename tuple_element<I, function_def>::type const&
// get(function_def const& f)
// {
//   return tuple_element<I, function_def>::get(f);
// }

// template <std::size_t I>
// typename tuple_element<I, function_def>::type&
// get(function_def& f)
// {
//   return tuple_element<I, function_def>::get(f);
// }

struct compare_klass_name_by_name
{
  bool operator()(klass_name const& lhs, klass_name const& rhs) const
  {
    return lhs.namespaces < rhs.namespaces
        || (!(rhs.namespaces < lhs.namespaces) && lhs.eolian_name < rhs.eolian_name);
  }
};

struct event_def
{
  eina::optional<type_def> type;
  std::string name, c_name;

  event_def(type_def type, std::string name, std::string c_name)
    : type(type), name(name), c_name(c_name) {}
  event_def(Eolian_Event const* event)
    : type( ::eolian_event_type_get(event) ? ::eolian_event_type_get(event) : eina::optional<type_def>{})
    , name( ::eolian_event_name_get(event))
    , c_name( ::eolian_event_c_name_get(event)) {}
};

template <>
struct tuple_element<0, event_def>
{
  typedef eina::optional<type_def> type;
  static type& get(event_def& def) { return def.type; }
  static type const& get(event_def const& def) { return def.type; }
};
template <>
struct tuple_element<1, event_def>
{
  typedef std::string type;
  static type& get(event_def& def) { return def.name; }
  static type const& get(event_def const& def) { return def.name; }
};
template <>
struct tuple_element<2, event_def>
{
  typedef std::string type;
  static type& get(event_def& def) { return def.c_name; }
  static type const& get(event_def const& def) { return def.c_name; }
};
template <int N>
struct tuple_element<N, event_def const> : tuple_element<N, event_def> {};
template <int N>
auto get(event_def const& def) -> decltype(tuple_element<N, event_def>::get(def))
{
  return tuple_element<N, event_def>::get(def);
}
template <int N>
auto get(event_def& def) -> decltype(tuple_element<N, event_def>::get(def))
{
  return tuple_element<N, event_def>::get(def);
}

struct klass_def
{
  std::string eolian_name;
  std::string cxx_name;
  std::vector<std::string> namespaces;
  std::vector<function_def> functions;
  std::set<klass_name, compare_klass_name_by_name> inherits;
  class_type type;
  std::vector<event_def> events;

  klass_def(std::string eolian_name, std::string cxx_name
            , std::vector<std::string> namespaces
            , std::vector<function_def> functions
            , std::set<klass_name, compare_klass_name_by_name> inherits
            , class_type type)
    : eolian_name(eolian_name), cxx_name(cxx_name)
    , namespaces(namespaces)
    , functions(functions), inherits(inherits), type(type)
  {}
  klass_def(Eolian_Class const* klass)
  {
     for(efl::eina::iterator<const char> namespace_iterator( ::eolian_class_namespaces_get(klass))
           , namespace_last; namespace_iterator != namespace_last; ++namespace_iterator)
       {
          this->namespaces.push_back(&*namespace_iterator);
       }
     cxx_name = eolian_name = eolian_class_name_get(klass);
     for(efl::eina::iterator<Eolian_Function const> eolian_functions ( ::eolian_class_functions_get(klass, EOLIAN_PROPERTY))
       , functions_last; eolian_functions != functions_last; ++eolian_functions)
       {
         Eolian_Function const* function = &*eolian_functions;
         Eolian_Function_Type type = ::eolian_function_type_get(function);
         if(type == EOLIAN_PROPERTY)
           {
             if(! ::eolian_function_is_legacy_only(function, EOLIAN_PROP_GET)
                && ::eolian_function_scope_get(function) != EOLIAN_SCOPE_PRIVATE)
               functions.push_back({function, EOLIAN_PROP_GET});
             if(! ::eolian_function_is_legacy_only(function, EOLIAN_PROP_SET)
                && ::eolian_function_scope_get(function) != EOLIAN_SCOPE_PRIVATE)
               functions.push_back({function, EOLIAN_PROP_SET});
           }
         else
           if(! ::eolian_function_is_legacy_only(function, type)
              && ::eolian_function_scope_get(function) != EOLIAN_SCOPE_PRIVATE)
             functions.push_back({function, type});
       }
     for(efl::eina::iterator<Eolian_Function const> eolian_functions ( ::eolian_class_functions_get(klass, EOLIAN_METHOD))
       , functions_last; eolian_functions != functions_last; ++eolian_functions)
       {
         Eolian_Function const* function = &*eolian_functions;
          if(! ::eolian_function_is_legacy_only(function, EOLIAN_METHOD)
             && ::eolian_function_scope_get(function) != EOLIAN_SCOPE_PRIVATE)
            functions.push_back({function, EOLIAN_METHOD});
       }
     std::function<void(Eolian_Class const*)> inherit_algo = 
       [&] (Eolian_Class const* klass)
       {
         for(efl::eina::iterator<const char> inherit_iterator ( ::eolian_class_inherits_get(klass))
               , inherit_last; inherit_iterator != inherit_last; ++inherit_iterator)
           {
             Eolian_Class const* inherit = ::eolian_class_get_by_name(&*inherit_iterator);
             inherits.insert({inherit, {}, {}});
             inherit_algo(inherit);
           }
       };
     inherit_algo(klass);
     switch(eolian_class_type_get(klass))
       {
       case EOLIAN_CLASS_REGULAR:
         type = class_type::regular;
         break;
       case EOLIAN_CLASS_ABSTRACT:
         type = class_type::abstract_;
         break;
       case EOLIAN_CLASS_MIXIN:
         type = class_type::mixin;
         break;
       case EOLIAN_CLASS_INTERFACE:
         type = class_type::interface_;
         break;
       default:
         throw std::runtime_error("Class with unknown type");
       }
     for(efl::eina::iterator<Eolian_Event const> event_iterator( ::eolian_class_events_get(klass))
           , event_last; event_iterator != event_last; ++event_iterator)
       {
         events.push_back(&*event_iterator);
       }
  }

};

inline klass_name get_klass_name(klass_def const& klass)
{
  return {klass.namespaces, klass.eolian_name, {qualifier_info::is_none, {}}, {}, klass.type};
}

inline Eolian_Class const* get_klass(klass_name const& klass_name_)
{
  std::string klass_name;
  if(!as_generator(*(string << ".") << string)
     .generate(std::back_insert_iterator<std::string>(klass_name)
               , std::make_tuple(klass_name_.namespaces, klass_name_.eolian_name)
               , context_null{}))
    return nullptr;
  else
    return ::eolian_class_get_by_name(klass_name.c_str());
}

inline std::vector<std::string> cpp_namespaces(std::vector<std::string> namespaces)
{
  if(namespaces.empty())
    namespaces.push_back("nonamespace");
  return namespaces;
}

}
namespace type_traits {

template <>
struct is_tuple<attributes::parameter_def> : std::true_type {};
template <>
struct is_tuple<attributes::event_def> : std::true_type {};
  
} } } }

#endif
