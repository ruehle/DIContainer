# DIContainer
Non-intrusive dependency injection container for C++. This project only uses C++11 
features and does not require any further dependencies. For an overview on dependency injection see

https://en.wikipedia.org/wiki/Dependency_injection

![Travis build status](https://travis-ci.org/ruehle/DIContainer.svg?branch=master)

## Getting started
```cpp
ContainerBuilder builder;
builder.registerType< ServiceImplementation >().as<IService>();

auto container = builder.build();
auto service = container->resolve<IService>();
```

## Register types

Register service without dependencies
```cpp
builder.registerType< ServiceImplementation >().as<IService>();
```

Register service with dependencies via lambda expression
```cpp
builder.registerType< DependentServiceImplementation >( 
    []{Container &r} { return std::make_shared<DependentServiceImplementation>( r.resolve<IService>() ); }
    ).as<IDependentService>();
```

Register service with dependencies using Injector helper class
```cpp
builder.registerType< DependentServiceImplementation >( Injector<IService>() ).as<IDependentService>();
```

## Livetime management
The default lifetime is per dependency, which means a new instance is created for every request. 
A different option is singleton lifetime, where every request gets the same instance

```cpp
builder.registerType< ServiceImplementation >().as<IService>().singleInstance
```

Futermore, an externally owned instance can be registered
```cpp
auto instance = std::make_shared<ServiceImplementation>();
builder.registerInstance( instance ).as<IService>();
```

## Named and keyed services
Named services are a special case of keyed services, where a std::string is used as a key
```cpp
builder.registerType< ServiceImplementation >().named<IService>("myservice");
auto container = builder.build();
auto service = container->resolveNamed<IService>("myservice");
```

Any type which has a copy constructor and comparison operator can be used as a key to 
register and resolve services. Ideally a std::hash function is provided. Note that enum classes do not
have a default hash function in the current standard. Visual Studio 2013 provides a standard hash function, 
however it is missing in gcc.
```cpp
enum class ServiceType {
    Red, Green
};

builder.registerType< ServiceImplementation >().keyed<IService>(ServiceType::Red);
auto container = builder.build();
auto service = container->resolveKeyed<IService>(ServiceType::Red);
```

