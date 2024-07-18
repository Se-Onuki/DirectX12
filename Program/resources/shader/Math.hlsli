float4x3 Mul(float4x3 leftMatrix, float4x3 rightMatrix)
{
    // Calc Multiply
    float4x3 result = mul(leftMatrix, (float3x3) (rightMatrix));

    // Add the 3rd line on the left as float3 to the 3rd line on the right
    result[3] += rightMatrix[3];

    return result;
}

// RotationMatrixOnry
float3x3 InverseTranspone(float3x3 input)
{
    float3x3 result = input;
    
    for (int y = 0; y < 3; y++)
    {
        // Divide each value by the norm length
        result[y] /= length(result[y]);
    }
    
    return result;
    
}

float WrapValue01(float value)
{
    float modValue = fmod(value, 1);
    if (modValue < 0)
    {
        modValue += 1;
    }
    return modValue;
}

float WrapValue(float value, const float minRange, const float maxRange)
{
    const float range = maxRange - minRange;
    
    float modValue = fmod(value - minRange, range);
    if (modValue < 0)
    {
        modValue += range;
    }
    return modValue + minRange;
}

float3 RGBtoHSV(const float3 rgb)
{
    const float maxValue = max(max(rgb.r, rgb.g), rgb.b);
        
    const float minValue = min(min(rgb.r, rgb.g), rgb.b);
    
    const float delta = maxValue - minValue;
    
    if (delta == 0)
    {
        return float3(0, 0, maxValue);
    }
    
    uint maxIndex;
    if (rgb[0] == maxValue)
    {
        maxIndex = 0;
    }
    else if (rgb[1])
    {
        maxIndex = 1;
    }
    else
    {
        maxIndex = 2;
    }
    
    const float h = WrapValue01((2.f * maxValue + (rgb[fmod(maxIndex + 1, 3)] - rgb[fmod(maxIndex + 2, 3)]) / delta) / 6.f);
    
    return float3(h, delta / maxValue, maxValue);
}

float3 HSVtoRGB(float3 hsv)
{
    float3 result;
    if (hsv.y > 0.0f)
    {
        hsv.x *= 6.0f;
        const int i = (int) hsv.x;
        const float f = hsv.x - (float) i;
        switch (i)
        {
            default:
            case 0:
                result.g *= 1 - hsv.y * (1 - f);
                result.b *= 1 - hsv.y;
                break;
            case 1:
                result.r *= 1 - hsv.y * f;
                result.b *= 1 - hsv.y;
                break;
            case 2:
                result.r *= 1 - hsv.y;
                result.b *= 1 - hsv.y * (1 - f);
                break;
            case 3:
                result.r *= 1 - hsv.y;
                result.g *= 1 - hsv.y * f;
                break;
            case 4:
                result.r *= 1 - hsv.y * (1 - f);
                result.g *= 1 - hsv.y;
                break;
            case 5:
                result.g *= 1 - hsv.y;
                result.b *= 1 - hsv.y * f;
                break;
        }
    }
    return result;
}
