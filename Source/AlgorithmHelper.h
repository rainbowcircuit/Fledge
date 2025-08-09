/*
  ==============================================================================

    AlgorithmHelper.h
    Created: 8 Aug 2025 2:32:43pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once

class AlgorithmHelper
{
public:
    std::array<float, 4> toBinary4(int input)
   {
       std::array<float, 4> bits;
       for (int i = 0; i < 4; ++i)
           bits[i] = (input >> i) & 1;
       return bits;
   }
    
    int fromBinary4(const std::array<float, 4>& bits)
    {
        int result = 0;
        for (int i = 0; i < 4; ++i)
        {
            if (bits[i] >= 0.5f)
            result |= (1 << i);

        }
        return result;
    }
};
