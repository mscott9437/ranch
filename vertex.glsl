#version 330 core

layout(location = 0) in vec2 aPos;

uniform vec2 uPosition;
uniform vec2 uSize;
uniform vec2 uScreenSize;

void main()
{
    vec2 pixelPos =
        uPosition +
        (aPos * uSize);

    vec2 ndc;

    ndc.x =
        (pixelPos.x / uScreenSize.x) * 2.0 - 1.0;

    ndc.y =
        1.0 -
        (pixelPos.y / uScreenSize.y) * 2.0;

    gl_Position =
        vec4(
            ndc,
            0.0,
            1.0
        );
}
