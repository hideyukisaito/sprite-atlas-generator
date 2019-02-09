uniform sampler2D uTexture;
uniform sampler2D uBgTexture;
uniform vec4 uColor;
uniform int uInvert;
uniform int uEnableDot;

uniform vec2 uCenter;
uniform float uAngle;
uniform float uScale;
uniform vec2 tSize;

varying vec2 texCoord;
varying mat3 spriteMat;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float pattern(vec2 uv)
{
    float s = sin(uAngle), c = cos(uAngle);

    vec2 tex = uv * tSize - uCenter;
    vec2 point = vec2( c * tex.x - s * tex.y, s * tex.x + c * tex.y ) * 10.0;

    return ( sin( point.x ) * sin( point.y ) ) * uScale;
}

void main()
{
    vec3 uv = vec3(texCoord, 1.0);

    uv *= spriteMat;

    vec2 coord = uv.xy / uv.z;
    vec4 sampleColor = texture2D(uTexture, coord);

    if (sampleColor.a < 0.5)
    {
        discard;
    }

    // vec4 bgColor = texture2D(uBgTexture, vUv);

    // if (bgColor.r + bgColor.g + bgColor.b == 3.0) {

    //     bgColor = sampleColor;

    // }

    vec4 blendedColor = sampleColor * uColor;

    vec4 outputColor = blendedColor;

    if (uEnableDot > 0) {

    outputColor = vec4( blendedColor.r + pattern(coord), blendedColor.g + pattern(coord), blendedColor.b + pattern(coord), blendedColor.a ) * uColor;

}

    if (uInvert == 0)
    {
        gl_FragColor = vec4(outputColor.r, outputColor.g, outputColor.b, 0.8);

    }
    else
    {
        vec4 col = vec4(vec3(1.0) - vec3(uColor.rgb), uColor.a);
        blendedColor = sampleColor * col;
        outputColor = vec4( blendedColor.r + pattern(coord), blendedColor.g + pattern(coord), blendedColor.b + pattern(coord), blendedColor.a ) * col;
        gl_FragColor = outputColor;
    }

}
