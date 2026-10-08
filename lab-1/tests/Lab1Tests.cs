using System;
using System.Collections.Generic;
using Lab1.Models;
using Lab1.Routing;

using Xunit;
namespace Lab1.Tests;

public class Lab1Tests
{
    [Fact]
    public void TestSkuCreation()
    {
        var sku = new Sku(Guid.NewGuid(), "Box", new VolumeWeight(10));
        Assert.True(sku.VolumeWeight.Value == 10, "SKU created properly");
        Assert.Throws<ArgumentException>(() => new Sku(Guid.NewGuid(), "Box", new VolumeWeight(-5)));
        Assert.Throws<ArgumentException>(() => new Sku(Guid.NewGuid(), "Box", new VolumeWeight(0)));
    }

    [Fact]
    public void TestTruckCreation()
    {
        Assert.Throws<ArgumentException>(() => new Speed(0));
        Assert.Throws<ArgumentException>(() => new Speed(-10));
    }

    [Fact]
    public void TestTruckLoading()
    {
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0));
        var sku = new Sku(Guid.NewGuid(), "Item", new VolumeWeight(40));
        
        truck.Load(new Dictionary<Sku, int> { { sku, 2 } });
        Assert.True(truck.Cargo[sku] == 2, "Cargo loaded");
        
        Assert.Throws<InvalidOperationException>(() => truck.Load(new Dictionary<Sku, int> { { sku, 1 } })); // 80 + 40 > 100
    }

    [Fact]
    public void TestTruckUnloading()
    {
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0));
        var sku = new Sku(Guid.NewGuid(), "Item", new VolumeWeight(40));
        truck.Load(new Dictionary<Sku, int> { { sku, 2 } });
        
        truck.Unload(new Dictionary<Sku, int> { { sku, 1 } });
        Assert.True(truck.Cargo[sku] == 1, "Cargo unloaded correctly");
        
        Assert.Throws<InvalidOperationException>(() => truck.Unload(new Dictionary<Sku, int> { { sku, 2 } })); // Try to unload more than available
    }

    [Fact]
    public void TestTruckMoving()
    {
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0)); 
        var dest = new Coordinates(0, 1); 
        var route = new MovementStep(dest);
        
        var result = route.Execute(truck);
        Assert.True(result is RouteStepResult.Success, "Movement success");
        if (result is RouteStepResult.Success s)
        {
            Assert.True(s.Duration.TotalSeconds == 5550, "Correct movement time");
        }
        Assert.True(truck.Coordinates.Latitude == 0 && truck.Coordinates.Longitude == 1, "Coordinates updated");
    }

    [Fact]
    public void TestEmployeeMultipleTrips()
    {
        var employee = new Employee(new VolumeWeight(10), TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee });
        
        // Items volume weight = 25 (requires 3 trips: 10, 10, 5)
        var duration = warehouse.CalculateAcceptanceTime(new VolumeWeight(25));
        
        // 1st trip: load/unload = 5, transfer = 10. Done at 15
        // 2nd trip: load/unload = 5, transfer = 10. Done at 30
        // 3rd trip: load/unload = 5, transfer = 10. Done at 45
        // Since it's Acceptance, truck release time is max(unloadDoneTime).
        // 1st trip unload at 5, 2nd trip unload at 15 + 5 = 20, 3rd trip unload at 30 + 5 = 35.
        // Therefore, duration = 35.
        Assert.True(duration.TotalSeconds == 35, $"Multiple trips duration incorrect, got {duration.TotalSeconds}");
    }

    [Fact]
    public void TestWarehouseCapacityLimit()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(60));
        
        warehouse.Accept(new Dictionary<Sku, int> { { sku, 1 } });
        Assert.True(warehouse.Stock[sku] == 1, "Item accepted");
        
        Assert.Throws<InvalidOperationException>(() => warehouse.Accept(new Dictionary<Sku, int> { { sku, 1 } })); // 60 + 60 > 100
        Assert.True(warehouse.Stock[sku] == 1, "Stock didn't change on failure");
    }

    [Fact]
    public void TestWarehouseDispatching()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(60));
        
        warehouse.Accept(new Dictionary<Sku, int> { { sku, 1 } });
        
        warehouse.Dispatch(new Dictionary<Sku, int> { { sku, 1 } });
        Assert.True(!warehouse.Stock.ContainsKey(sku), "Item dispatched");
        
        Assert.Throws<InvalidOperationException>(() => warehouse.Dispatch(new Dictionary<Sku, int> { { sku, 1 } })); // Not enough stock
    }

    [Fact]
    public void TestWarehouseStaffCalculations()
    {
        var employee1 = new Employee(new VolumeWeight(10), TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10));
        var employee2 = new Employee(new VolumeWeight(10), TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee1, employee2 });
        
        // 2 employees, total needed: 25.
        // E1: 1st trip (10), starts at 0, unload at 5, available at 15
        // E2: 1st trip (10), starts at 0, unload at 5, available at 15
        // E1 (or E2): 2nd trip (5), starts at 15, unload at 20, available at 30
        // Duration = max unload time = 20
        var duration = warehouse.CalculateAcceptanceTime(new VolumeWeight(25));
        Assert.True(duration.TotalSeconds == 20, $"Multiple employees duration incorrect, got {duration.TotalSeconds}");
    }

    [Fact]
    public void TestWarehouseAcceptance()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(100));
        var truck = new Truck(new VolumeWeight(500), new Speed(20), new Coordinates(0, 0));
        truck.Load(new Dictionary<Sku, int> { { sku, 1 } }); 
        
        var acceptStep = new AcceptanceStep(warehouse, new Dictionary<Sku, int> { { sku, 1 } });
        var result = acceptStep.Execute(truck);
        
        Assert.True(result is RouteStepResult.Success, "Acceptance success");
        if (result is RouteStepResult.Success s)
        {
            Assert.True(s.Duration.TotalSeconds == 40, $"Duration was {s.Duration.TotalSeconds}, expected 40");
        }
        Assert.True(warehouse.Stock[sku] == 1, "Item in warehouse");
        Assert.True(truck.Cargo.Count == 0, "Truck is empty");
    }

    [Fact]
    public void TestWarehouseDispatchTime()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(100));
        warehouse.Accept(new Dictionary<Sku, int> { { sku, 1 } });
        
        var truck = new Truck(new VolumeWeight(500), new Speed(20), new Coordinates(0, 0));
        var dispatchStep = new DispatchStep(warehouse, new Dictionary<Sku, int> { { sku, 1 } });
        var result = dispatchStep.Execute(truck);
        
        Assert.True(result is RouteStepResult.Success, "Dispatch success");
        if (result is RouteStepResult.Success s)
        {
            Assert.True(s.Duration.TotalSeconds == 60, $"Duration was {s.Duration.TotalSeconds}, expected 60");
        }
    }

    [Fact]
    public void TestAcceptanceStepErrors()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(60));
        
        var truck = new Truck(new VolumeWeight(500), new Speed(20), new Coordinates(0, 0));
        var acceptStep = new AcceptanceStep(warehouse, new Dictionary<Sku, int> { { sku, 1 } });
        
        // 1. Not enough items in truck
        var res1 = acceptStep.Execute(truck);
        Assert.True(res1 is RouteStepResult.Failure, "Failure due to lack of items in truck");
        
        // 2. Not enough capacity in warehouse
        truck.Load(new Dictionary<Sku, int> { { sku, 2 } });
        var acceptStep2 = new AcceptanceStep(warehouse, new Dictionary<Sku, int> { { sku, 2 } }); // 120 > 100
        var res2 = acceptStep2.Execute(truck);
        Assert.True(res2 is RouteStepResult.Failure, "Failure due to lack of capacity in warehouse");
        
        // 3. Truck too far
        truck.MoveTo(new Coordinates(1, 1));
        var res3 = acceptStep.Execute(truck);
        Assert.True(res3 is RouteStepResult.Failure, "Failure due to distance > 10");
    }

    [Fact]
    public void TestDispatchStepErrors()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(60));
        
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0));
        var dispatchStep = new DispatchStep(warehouse, new Dictionary<Sku, int> { { sku, 1 } });
        
        // 1. Not enough items in warehouse
        var res1 = dispatchStep.Execute(truck);
        Assert.True(res1 is RouteStepResult.Failure, "Failure due to lack of items in warehouse");
        
        // 2. Not enough capacity in truck
        warehouse.Accept(new Dictionary<Sku, int> { { sku, 1 } });
        truck.Load(new Dictionary<Sku, int> { { sku, 1 } }); // Truck now has 60, only 40 capacity left.
        var res2 = dispatchStep.Execute(truck);
        Assert.True(res2 is RouteStepResult.Failure, "Failure due to lack of capacity in truck");
        truck.Unload(new Dictionary<Sku, int> { { sku, 1 } });
        
        // 3. Truck too far
        truck.MoveTo(new Coordinates(1, 1));
        var res3 = dispatchStep.Execute(truck);
        Assert.True(res3 is RouteStepResult.Failure, "Failure due to distance > 10");
    }

    [Fact]
    public void TestRouteListExecution()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse1 = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee });
        var warehouse2 = new Warehouse(Guid.NewGuid(), new Coordinates(0, 1), new VolumeWeight(500), new[] { employee });
        
        var sku = new Sku(Guid.NewGuid(), "Item", new VolumeWeight(100));
        warehouse1.Accept(new Dictionary<Sku, int> { { sku, 1 } });
        
        var truck = new Truck(new VolumeWeight(500), new Speed(20), new Coordinates(0, 0));
        
        RouteStep route = new RouteList(new RouteStep[] 
        {
            new DispatchStep(warehouse1, new Dictionary<Sku, int> { { sku, 1 } }),
            new MovementStep(warehouse2.Coordinates),
            new AcceptanceStep(warehouse2, new Dictionary<Sku, int> { { sku, 1 } })
        });
        
        var result = route.Execute(truck);
        Assert.True(result is RouteStepResult.Success, "Route success");
        if (result is RouteStepResult.Success s)
        {
            Assert.True(s.Duration.TotalSeconds == (60 + 5550 + 40), "Route correct duration");
        }
    }

    [Fact]
    public void TestRouteListInterruption()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse1 = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee });
        var warehouse2 = new Warehouse(Guid.NewGuid(), new Coordinates(0, 1), new VolumeWeight(500), new[] { employee });
        
        var sku = new Sku(Guid.NewGuid(), "Item", new VolumeWeight(100));
        
        var truck = new Truck(new VolumeWeight(500), new Speed(20), new Coordinates(0, 0));
        
        RouteStep route = new RouteList(new RouteStep[] 
        {
            new DispatchStep(warehouse1, new Dictionary<Sku, int> { { sku, 1 } }), // Fails, empty warehouse
            new MovementStep(warehouse2.Coordinates)
        });
        
        var result = route.Execute(truck);
        Assert.True(result is RouteStepResult.Failure, "Route fails if one step fails");
        Assert.True(truck.Coordinates.Latitude == 0 && truck.Coordinates.Longitude == 0, "Truck didn't move after failure");
    }

    [Fact]
    public void TestZeroOrNegativeQuantities()
    {
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0));
        var sku = new Sku(Guid.NewGuid(), "Item", new VolumeWeight(10));
        
        Assert.Throws<ArgumentException>(() => truck.Load(new Dictionary<Sku, int> { { sku, 0 } }));
        Assert.Throws<ArgumentException>(() => truck.Load(new Dictionary<Sku, int> { { sku, -5 } }));
        
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        
        Assert.Throws<ArgumentException>(() => warehouse.Accept(new Dictionary<Sku, int> { { sku, 0 } }));
        Assert.Throws<ArgumentException>(() => warehouse.Accept(new Dictionary<Sku, int> { { sku, -2 } }));
    }

    [Fact]
    public void TestEmptyDictionary()
    {
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0));
        truck.Load(new Dictionary<Sku, int>());
        Assert.True(truck.Cargo.Count == 0, "Empty dictionary loaded safely");
        
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        
        warehouse.Accept(new Dictionary<Sku, int>());
        Assert.True(warehouse.Stock.Count == 0, "Empty dictionary accepted safely");
    }

    [Fact]
    public void TestFloatingPointPrecision()
    {
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0));
        var sku1 = new Sku(Guid.NewGuid(), "Item1", new VolumeWeight(33.333333333333333333333333333));
        
        truck.Load(new Dictionary<Sku, int> { { sku1, 3 } });
        Assert.True(truck.Cargo.ContainsKey(sku1), "Precision test passed");
    }

    [Fact]
    public void TestDistanceExactly10eters()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "Item", new VolumeWeight(10));
        
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 10.0 / 111000.0)); // Exactly 10 away
        truck.Load(new Dictionary<Sku, int> { { sku, 1 } });
        
        var acceptStep = new AcceptanceStep(warehouse, new Dictionary<Sku, int> { { sku, 1 } });
        var result = acceptStep.Execute(truck);
        Assert.True(result is RouteStepResult.Success, "Acceptance at exactly 10 meters distance is successful");
    }
}


