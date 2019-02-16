#version 150

uniform sampler2DRect mainTex;
uniform vec4 color;
uniform vec2 textureResolution;
uniform float gridPerSide;
uniform float totalFrameCount;
uniform float totalTimeMillis;
uniform float startTime;
uniform float frameOffset;
uniform float time;

in vec2 texCoordVarying;

out vec4 outputColor;

void main()
{
	vec2 uv = texCoordVarying;
	uv.y = textureResolution.y - uv.y;
	
	float scale = 1.0 / gridPerSide;

	float totalDuration = totalTimeMillis * 0.001;
	float currentTime = mod(time, totalDuration);

	float ratio = currentTime / totalDuration;
	float index = frameOffset + max(0.0, min(floor(totalFrameCount * ratio), totalFrameCount));

	uv.x *= scale;
	uv.y *= scale;

	uv.x = uv.x + (textureResolution.x * scale) * mod(index, gridPerSide);
	uv.y = uv.y + (textureResolution.y * scale) * floor(index / gridPerSide);

    outputColor = texture(mainTex, uv) * color;
}