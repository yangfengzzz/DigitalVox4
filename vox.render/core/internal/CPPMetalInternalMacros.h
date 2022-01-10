/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Macros used for C++ Metal wrapper implementation
*/

#include "CPPMetalImplementation.hpp"

#define CPP_METAL_VALIDATE_WRAPPED_NIL() \
    assert( m_objCObj && "Calling object wrapping nil" )

#define CPP_METAL_VALIDATE_ENUM_ALIAS( name )                                \
    _Static_assert( (UInteger)MTL##name == (UInteger) name,                  \
                    "Metal enum value does not match CPPMetal enum value" )

#define CPP_METAL_VALIDATE_STRUCT_ALIAS(structname, member)                           \
    _Static_assert(offsetof(MTL##structname, member) == offsetof(structname, member), \
                   #structname " layout does not match Metal object layout");

#define CPP_METAL_NULL_REFERENCE_CONSTRUCTOR_IMPLEMENATATION(classname) \
    classname::classname()                                              \
    : m_objCObj(nullptr)                                                \
    , m_device(nullptr)                                                 \
    {                                                                   \
    }

// DO NOT USE IN HEADER: Prevents proper objC reference counting.
#define CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(classname)         \
    classname::classname(CPPMetalInternal::classname objCObj,   \
                         Device & device)                       \
    : m_objCObj(objCObj)                                        \
    , m_device(&device)                                         \
    {                                                           \
        assert([m_device->objCObj() isEqual:m_objCObj.device]); \
    }

// DO NOT USE IN HEADER: Prevents proper objC reference counting.
#define CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(classname) \
    classname::classname(const classname & rhs)                                    \
    : m_objCObj(rhs.m_objCObj)                                                     \
    , m_device(rhs.m_device)                                                       \
    {                                                                              \
                                                                                   \
    }                                                                              \
                                                                                   \
    classname & classname::operator=(const classname & rhs)                        \
    {                                                                              \
        m_objCObj = rhs.m_objCObj;                                                 \
        m_device = rhs.m_device;                                                   \
        return *this;                                                              \
    }


#define CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(classname, type, name) \
void classname::name(type val)                                                      \
{                                                                                   \
    CPP_METAL_VALIDATE_WRAPPED_NIL();                                               \
    m_objCObj.name = (MTL##type)val;                                                \
}                                                                                   \
                                                                                    \
type classname::name() const                                                        \
{                                                                                   \
    CPP_METAL_VALIDATE_WRAPPED_NIL();                                               \
    return (type)m_objCObj.name;                                                    \
}

#define CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(classname, type, name) \
void classname::name(type val)                                             \
{                                                                          \
    CPP_METAL_VALIDATE_WRAPPED_NIL();                                      \
    m_objCObj.name = val;                                                  \
}                                                                          \
                                                                           \
type classname::name() const                                               \
{                                                                          \
    CPP_METAL_VALIDATE_WRAPPED_NIL();                                      \
    return m_objCObj.name;                                                 \
}

#define CPP_METAL_READWRITE_BOOL_PROPERTY_IMPLEMENTATION(classname, setter, getter) \
void classname::setter(bool val)                                                    \
{                                                                                   \
    CPP_METAL_VALIDATE_WRAPPED_NIL();                                               \
    m_objCObj.setter = val;                                                         \
}                                                                                   \
                                                                                    \
bool classname::getter() const                                                      \
{                                                                                   \
    CPP_METAL_VALIDATE_WRAPPED_NIL();                                               \
    return m_objCObj.getter;                                                        \
}

#define CPP_METAL_READWRITE_LABEL_PROPERTY_IMPLEMENTATION(classname) \
const char* classname::label() const                                 \
{                                                                    \
    CPP_METAL_VALIDATE_WRAPPED_NIL();                                \
    return m_objCObj.label.UTF8String;                               \
}                                                                    \
                                                                     \
void classname::label(const CFStringRef string)                      \
{                                                                    \
    CPP_METAL_VALIDATE_WRAPPED_NIL();                                \
    m_objCObj.label = (__bridge NSString *)string;                   \
}

#define CPP_METAL_DEVICE_GETTER_IMPLEMENTATION(classname) \
Device classname::device() const                          \
{                                                         \
    CPP_METAL_VALIDATE_WRAPPED_NIL();                     \
    return *m_device;                                     \
}
