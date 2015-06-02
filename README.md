# DIContainer
Non-intrusive dependency injection container for C++

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

Register an instance
```cpp
auto instance = std::make_shared<ServiceImplementation>();
builder.registerInstance( instance ).as<IService>();
```

## Named services
```cpp
builder.registerType< ServiceImplementation >().named<IService>("myservice");
auto container = builder.build();
auto service = container->resolve<IService>("myservice");
```

