#ifndef SYMMETRY_BREAKING_TYPE_H
#define SYMMETRY_BREAKING_TYPE_H

enum class SymmetryBreakingType
{
    NONE,           // No symmetry breaking
    FIRST,          // Symmetry breaking on the first vertex
    HIGHEST_DEGREE, // Symmetry breaking on the highest degree vertex
    LOWEST_DEGREE,  // Symmetry breaking on the lowest degree vertex
};

#endif // SYMMETRY_BREAKING_TYPE_H