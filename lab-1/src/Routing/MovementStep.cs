using System;
using Lab1.Models;

namespace Lab1.Routing;

public class MovementStep : RouteStep
{
    public Coordinates Destination { get; }

    public MovementStep(Coordinates destination)
    {
        Destination = destination;
    }

    public override RouteStepResult Execute(Truck truck)
    {
        if (truck == null) throw new ArgumentNullException(nameof(truck));

        Distance distance = truck.Coordinates.DistanceTo(Destination);
        TimeSpan duration = distance.DivideBy(truck.Speed);
        
        truck.MoveTo(Destination);
        
        return new RouteStepResult.Success(duration);
    }
}
