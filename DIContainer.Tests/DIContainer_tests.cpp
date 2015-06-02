#include "gtest/gtest.h"
#include "../DIContainer/ContainerBuilder.h"
#include "../DIContainer/Injector.h"

using namespace DIContainer;

namespace {
    class IService
    {
    public:
        virtual ~IService() = default;
    };

    class ServiceImplementation : public IService
    {};

    class ServiceImplementation2 : public IService
    {};

    class ServiceImplementation3 : public IService
    {};

    class IAnotherService
    {
    public:
        virtual ~IAnotherService() = default;
    };

    class DependentServiceImplementation : public IAnotherService
    {
    public:
        explicit DependentServiceImplementation(
            std::shared_ptr<IService> dependency)
        {
            if (dependency == nullptr)
                throw std::logic_error("invalid argument");
        }
    };
}

TEST(DIContainerTests, ResolveUnregisteredDependency_TrowsUnresolvedDependencyException)
{
    ContainerBuilder builder;
    auto resolver = builder.build();
    ASSERT_THROW(resolver->resolve<IService>(), UnresolvedDependencyException);
}

TEST(DIContainerTests, RegisteredSameDependencyTwice_ThrowsDuplicateDependencyException)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>().as<IService>();

    ASSERT_THROW(
        builder.registerType<ServiceImplementation>().as<IService>(),
        DuplicateDependencyException
        );
}

TEST(DIContainerTests, ResolveRegisteredDependency_ReturnsInstance)
{
    ContainerBuilder builder;
    builder.registerType<ServiceImplementation>().as<IService>();

    auto resolver = builder.build();
    auto obj = resolver->resolve< IService >();

    ASSERT_NE(std::dynamic_pointer_cast<ServiceImplementation>(obj), nullptr);
}

TEST(DIContainerTests, CallResolveTwice_ReturnsDifferentInstances)
{
    ContainerBuilder builder;
    builder.registerType<ServiceImplementation>()
        .as<IService>();

    auto resolver = builder.build();

    auto obj1 = resolver->resolve< IService >();
    auto obj2 = resolver->resolve< IService >();

    ASSERT_NE(obj1, obj2);
}

TEST(DIContainerTests, RegisterDependencyWithSameNameAndType_ThrowsDuplicateDependencyException)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .named<IService>("name");

    ASSERT_THROW(
        builder.registerType<ServiceImplementation>()
        .named<IService>("name"),
        DuplicateDependencyException
        );
}

TEST(DIContainerTests, RegisteredNamedAndUnnamedDependency_Succeeds)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .as<IService>();

    builder.registerType<ServiceImplementation2>()
        .named<IService>("name2");

    builder.registerType<ServiceImplementation3>()
        .named<IService>("name3");

}

TEST(DIContainerTests, ResolveNamedAndUnnamedTypes_InstanciateCorrectly)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .as<IService>();

    builder.registerType<ServiceImplementation2>()
        .named<IService>("name2");

    builder.registerType<ServiceImplementation3>()
        .named<IService>("name3");

    auto resolver = builder.build();

    auto obj = std::dynamic_pointer_cast<ServiceImplementation>(resolver->resolve<IService>());
    auto obj2 = std::dynamic_pointer_cast<ServiceImplementation2>(resolver->resolve<IService>("name2"));
    auto obj3 = std::dynamic_pointer_cast<ServiceImplementation3>(resolver->resolve<IService>("name3"));

    ASSERT_NE(obj, nullptr);
    ASSERT_NE(obj2, nullptr);
    ASSERT_NE(obj3, nullptr);
}

TEST(DIContainerTests, ResolveImplementationWithDependencyByCode_Succeeds)
{
    ContainerBuilder builder;
    builder.registerType<ServiceImplementation>()
        .as<IService>();

    auto create = [](Container &r){
        return std::make_shared<DependentServiceImplementation>(
            r.resolve<IService>()
            ); };

    builder.registerType<DependentServiceImplementation>(create)
        .as<IAnotherService>();


    auto resolver = builder.build();
    auto obj = resolver->resolve< IAnotherService >();
    ASSERT_NE(obj, nullptr);
}

TEST(DIContainerTests, ResolveImplementationWithDependency_Succeeds)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .as<IService>();

    builder.registerType<DependentServiceImplementation>(Injector<IService>())
        .as<IAnotherService>();

    auto resolver = builder.build();
    auto obj = resolver->resolve< IAnotherService >();
    ASSERT_NE(obj, nullptr);
}

TEST(DIContainerTests, RegisterAndResolveInstance_ReturnsSameInstance)
{
    ContainerBuilder builder;

    auto instance = std::make_shared<ServiceImplementation>();

    builder.registerInstance(instance)
        .as<IService>();

    auto resolver = builder.build();

    auto obj1 = resolver->resolve< IService >();
    auto obj2 = resolver->resolve< IService >();

    ASSERT_EQ(instance, obj1);
    ASSERT_EQ(instance, obj2);
}