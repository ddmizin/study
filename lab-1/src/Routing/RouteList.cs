using System;
using System.Collections.Generic;
using Lab1.Models;

namespace Lab1.Routing;

// RouteList is a Composite over RouteStep
public class RouteList : RouteStep
{
    private readonly List<RouteStep> _steps;

    public RouteList(IEnumerable<RouteStep> steps)
    {
        if (steps == null) throw new ArgumentNullException(nameof(steps));
        _steps = new List<RouteStep>(steps);
    }

    public override RouteStepResult Execute(Truck truck)
    {
        if (truck == null) throw new ArgumentNullException(nameof(truck));

        TimeSpan totalDuration = TimeSpan.Zero;

        for (int i = 0; i < _steps.Count; i++)
        {
            var result = _steps[i].Execute(truck);
            
            if (result is RouteStepResult.Failure failure)
            {
                return new RouteStepResult.Failure($"Step {i + 1} failed: {failure.ErrorMessage}");
            }
            if (result is RouteStepResult.Success success)
            {
                totalDuration += success.Duration;
            }
        }

        return new RouteStepResult.Success(totalDuration);
    }
}
