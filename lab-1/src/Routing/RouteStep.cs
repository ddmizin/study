using Lab1.Models;

namespace Lab1.Routing;

public abstract class RouteStep
{
    public abstract RouteStepResult Execute(Truck truck);
}
