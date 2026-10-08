using System;

namespace Lab1.Models;

public record Speed
{
    public double Value { get; }

    public Speed(double value)
    {
        if (value <= 0)
            throw new ArgumentException("Speed must be strictly positive.", nameof(value));
        Value = value;
    }
}
