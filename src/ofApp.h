#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxImGui.h"

#define MAX_WIDTH 4096.0
#define PREVIEW_SIZE 500.0
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
private:

	std::vector<ofImage> mImages;

	int mOffset;
	int mNumCells;
	float mFrameRate;

	ofFbo mFbo;
	ofShader mShader;
	ofPlanePrimitive mPlane;

	ofxJSONElement mSpriteInfoJson;

	ofxImGui::Gui gui;
	std::vector<AnimationProps> mAnimationProps;

	void setup();
	void update();
	void draw();

	void loadImagesFromDirectory(const string &path);
	void createSpriteAtlas();
	void exportResources();
	void clear();

	void keyReleased(int key);
	void dragEvent(ofDragInfo dragInfo);
};
