uniform mat4 modelViewProjectionMatrix;
uniform float uHorizontalTile;
uniform float uFrameIndex;

attribute vec4 position;
attribute vec2 texcoord;

varying mat3 spriteMat;
varying vec2 texCoord;

void main() {
    
    mat3 scaleMat = mat3(
                         1.0 / uHorizontalTile, 0.0, 0.0,
                         0.0, 1.0 / uHorizontalTile, 0.0,
                         0.0, 0.0, 1.0
                         );
    
    mat3 translateMat = mat3(
                             1.0, 0.0, mod(uFrameIndex, uHorizontalTile),
                             0.0, 1.0, uHorizontalTile - (floor(uFrameIndex / uHorizontalTile) + 1.0),
                             0.0, 0.0, 1.0
                             );
    
    texCoord = texcoord;
    spriteMat = translateMat * scaleMat;
    
    gl_Position = modelViewProjectionMatrix * position;
    
}
