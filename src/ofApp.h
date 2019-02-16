#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxImGui.h"

#define MAX_WIDTH 4096.0
#define DISPLAY_WIDTH 1024.0
#define CELL_COUNT 5

struct AnimationProps
{
public:
    float duration;
    float maxDuration;
    int totalFrames;
    int offset;
	int tweakedOffset;
    int maxFrameCount;
    std::string name;
};

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

    void loadImage(const string &path, const int index);
    
	void keyReleased(int key);
    void dragEvent(ofDragInfo dragInfo);
    
    std::vector<ofImage> mImages;
    int mOffset;
    float mFrameRate;
    
    ofFbo mFbo;
    ofShader mShader;
    
    ofxJSONElement mSpriteInfoJson;
    
    ofxImGui::Gui gui;
    float mFloatValue;
    std::vector<AnimationProps> mAnimationProps;
    
private:

	int mNumCells;
	ofPlanePrimitive mPlane;
    
    static bool compareFilename(const string &first, const string &second)
    {
        return filenameToInt(first) < filenameToInt(second);
    }
    
    static int filenameToInt(const string &str)
    {
        int x = ofToInt(ofSplitString(str, ".").at(0));
        ofLog() << ofSplitString(str, ".").at(0);
        return ofToInt(ofSplitString(str, ".").at(0));
    }
};
