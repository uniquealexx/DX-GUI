// VertexShader.hlsl
struct VS_INPUT
{
    float2 pos : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    // Преобразование из пиксельных координат в нормализованные координаты (-1 до 1)
    output.pos = float4(input.pos.x / (800 / 2) - 1.0f,
                       1.0f - input.pos.y / (800 / 2),
                       0.0f, 1.0f);
    
    output.color = input.color;
    return output;
}