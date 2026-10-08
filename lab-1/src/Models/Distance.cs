using System;

namespace Lab1.Models;

public record Distance
{
    public double Value { get; }

    public Distance(double value)
    {
        if (value < 0)
            throw new ArgumentException("Distance cannot be negative.", nameof(value));
        Value = value;
    }

    public static bool operator >(Distance left, Distance right) => left.Value > right.Value;
    public static bool operator <(Distance left, Distance right) => left.Value < right.Value;
    public static bool operator >=(Distance left, Distance right) => left.Value >= right.Value;
    public static bool operator <=(Distance left, Distance right) => left.Value <= right.Value;
    
    public TimeSpan DivideBy(Speed speed)
    {
        return TimeSpan.FromSeconds(Value / speed.Value);
    }
    
    public static Distance FromMeters(double meters) => new Distance(meters);
}
