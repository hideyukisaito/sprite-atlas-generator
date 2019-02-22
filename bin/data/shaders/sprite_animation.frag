#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect tex2;
uniform sampler2DRect tex3;
uniform vec4 color;
uniform vec2 textureResolution;
uniform float gridPerSide;
uniform float horizontalFrameCount;
uniform float verticalFrameCount;
uniform float totalFrameCount;
uniform float durationInSeconds;
uniform float startTime;
uniform float frameOffset;
uniform float time;

in vec2 texCoordVarying;

out vec4 outputColor;

void main()
{
	vec2 uv = texCoordVarying;
	uv.y = textureResolution.y - uv.y;
	
	float scaleX = 1.0 / horizontalFrameCount;
	float scaleY = 1.0 / verticalFrameCount;

	float currentTime = mod(time, durationInSeconds);

	float ratio = currentTime / durationInSeconds;
	float index = max(0.0, min(floor(totalFrameCount * ratio), totalFrameCount));

	uv.x *= scaleX;
	uv.y *= scaleY;

	uv.x = uv.x + (textureResolution.x * scaleX) * mod(mod(index, 20.0), horizontalFrameCount);
	uv.y = uv.y + (textureResolution.y * scaleY) * floor(mod(index, 20.0) / horizontalFrameCount);

	float page = floor(index / 20.0);
	vec4 texel;

	if (page == 0.0)
	{
		texel = texture(tex0, uv);
	}
	else if (page == 1.0)
	{
		texel = texture(tex1, uv);
	}
	else if (page == 2.0)
	{
		texel = texture(tex2, uv);
	}

    outputColor = texel * color;
}