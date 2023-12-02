#pragma once

#include <memory>
#include <string>

#include <boost/dll/alias.hpp>

#include "interface/IInterface.hpp"
#include "interface/TDefinedInterface.hpp"
#include "component/TComponentFactory.hpp"

#define DECLARE_INTERFACE(INTERFACE)                                                  \
    template <>                                                                       \
    struct HBE::TDefinedInterface<INTERFACE> final                                    \
    {                                                                                 \
        static HBE::InterfaceTypeInfo getInterfaceTypeInfo()                          \
        { return HBE::InterfaceTypeInfo{#INTERFACE}; }                                \
    };                                                                                \

#define IMPLEMENT_INTERFACE(NAME, INTERFACE)                                          \
    struct NAME final                                                                 \
    {                                                                                 \
        using Type = INTERFACE;                                                       \
                                                                                      \
        static std::string getName() { return #NAME; }                                \
        static HBE::InterfaceTypeInfo getInterfaceTypeInfo()                          \
        {                                                                             \
            return HBE::TDefinedInterface<INTERFACE>::getInterfaceTypeInfo();         \
        }                                                                             \
    };                                                                                \

#define REGISTER_INTERFACE(NAME)                                                      \
    provideInterface<NAME>(shared_from_this());                                       \

#define DEFINE_DEPENDENCY(NAME, INTERFACE)                                            \
    struct NAME final                                                                 \
    {                                                                                 \
        using Type = INTERFACE;                                                       \
                                                                                      \
        static std::string getName() { return std::string(#NAME); }                   \
                                                                                      \
        explicit NAME(HBE::TInterfaceWPtr<Type> inst_ptr)                             \
        : instance{inst_ptr}                                                          \
        {                                                                             \
        }                                                                             \
                                                                                      \
        HBE::TInterfaceWPtr<Type> instance;                                           \
    };                                                                                \

#define DEFINE_COMPONENT(COMPONENT)                                                   \
    HBE::IComponentFactoryPtr createComponentFactoryImpl()                            \
    {                                                                                 \
        return std::make_shared<HBE::TComponentFactory<COMPONENT>>();                 \
    }                                                                                 \
    BOOST_DLL_ALIAS(createComponentFactoryImpl, createComponentFactory)
