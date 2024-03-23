float4x3 Mul(float4x3 leftMatrix, float4x3 rightMatrix)
{
    // Calc Multiply
    float4x3 result = mul(leftMatrix, (float3x3) (rightMatrix));

    // Add the 3rd line on the left as float3 to the 3rd line on the right
    result[3] += rightMatrix[3];

    return result;
}