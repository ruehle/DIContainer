#include "gtest/gtest.h"
#include "../DIContainer/Container.h"
#include "../DIContainer/Injector.h"

using namespace DIContainer;

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

TEST(DIContainerTests, ResolveUnregisteredDependency_TrowsUnresolvedDependencyException)
{
    Container resolver;
    ASSERT_THROW(resolver.resolve<IService>(), UnresolvedDependencyException);
}

TEST(DIContainerTests, RegisteredSameDependencyTwice_ThrowsDuplicateDependencyException)
{
    Container resolver;

    resolver.registerType<ServiceImplementation>().as<IService>();

    ASSERT_THROW(
        resolver.registerType<ServiceImplementation>().as<IService>(),
        DuplicateDependencyException
        );
}

TEST(DIContainerTests, ResolveRegisteredDependency_ReturnsInstance)
{
    Container resolver;
    resolver.registerType<ServiceImplementation>().as<IService>();

    auto obj = resolver.resolve< IService >();

    ASSERT_NE(std::dynamic_pointer_cast<ServiceImplementation>(obj), nullptr);
}

TEST(DIContainerTests, CallResolveTwice_ReturnsDifferentInstances)
{
    Container resolver;
    resolver.registerType<ServiceImplementation>()
        .as<IService>();

    auto obj1 = resolver.resolve< IService >();
    auto obj2 = resolver.resolve< IService >();

    ASSERT_NE(obj1, obj2);
}

TEST(DIContainerTests, RegisterDependencyWithSameNameAndType_ThrowsDuplicateDependencyException)
{
    Container resolver;

    resolver.registerType<ServiceImplementation>()
        .named<IService>("name");

    ASSERT_THROW(
        resolver.registerType<ServiceImplementation>()
        .named<IService>("name"),
        DuplicateDependencyException
        );
}

TEST(DIContainerTests, RegisteredNamedAndUnnamedDependency_Succeeds)
{
    Container resolver;

    resolver.registerType<ServiceImplementation>()
        .as<IService>();

    resolver.registerType<ServiceImplementation2>()
        .named<IService>("name2");

    resolver.registerType<ServiceImplementation3>()
        .named<IService>("name3");

}

TEST(DIContainerTests, ResolveNamedAndUnnamedTypes_InstanciateCorrectly)
{
    Container resolver;

    resolver.registerType<ServiceImplementation>()
        .as<IService>();

    resolver.registerType<ServiceImplementation2>()
        .named<IService>("name2");

    resolver.registerType<ServiceImplementation3>()
        .named<IService>("name3");

    auto obj = std::dynamic_pointer_cast<ServiceImplementation>(resolver.resolve<IService>());
    auto obj2 = std::dynamic_pointer_cast<ServiceImplementation2>(resolver.resolve<IService>("name2"));
    auto obj3 = std::dynamic_pointer_cast<ServiceImplementation3>(resolver.resolve<IService>("name3"));

    ASSERT_NE(obj, nullptr);
    ASSERT_NE(obj2, nullptr);
    ASSERT_NE(obj3, nullptr);
}

TEST(DIContainerTests, ResolveImplementationWithDependencyByCode_Succeeds)
{
    Container resolver;
    resolver.registerType<ServiceImplementation>()
        .as<IService>();

    auto create = [](Container &r){
        return std::make_shared<DependentServiceImplementation>(
            r.resolve<IService>()
            ); };

    resolver.registerType<DependentServiceImplementation>(create)
        .as<IAnotherService>();


    auto obj = resolver.resolve< IAnotherService >();
    ASSERT_NE(obj, nullptr);
}

TEST(DIContainerTests, ResolveImplementationWithDependency_Succeeds)
{
    Container resolver;

    resolver.registerType<ServiceImplementation>()
        .as<IService>();

    resolver.registerType<DependentServiceImplementation>(Injector<IService>())
        .as<IAnotherService>();

    auto obj = resolver.resolve< IAnotherService >();
    ASSERT_NE(obj, nullptr);
}

TEST(DIContainerTests, RegisterAndResolveInstance_ReturnsSameInstance)
{
    Container resolver;

    auto instance = std::make_shared<ServiceImplementation>();

    resolver.registerInstance(instance)
        .as<IService>();

    auto obj1 = resolver.resolve< IService >();
    auto obj2 = resolver.resolve< IService >();

    ASSERT_EQ(instance, obj1);
    ASSERT_EQ(instance, obj2);
}