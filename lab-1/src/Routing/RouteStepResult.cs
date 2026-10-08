using System;

namespace Lab1.Routing;

public abstract record RouteStepResult
{
    private RouteStepResult() { }

    public sealed record Success(TimeSpan Duration) : RouteStepResult;
    public sealed record Failure(string ErrorMessage) : RouteStepResult;
}
