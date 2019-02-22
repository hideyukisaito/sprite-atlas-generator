#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxImGui.h"

#define MAX_WIDTH 4096.0
#define PREVIEW_SIZE 500.0
#define PREVIEW_ANIMATION_SIZE 1000.0
#define PREVIEW_CELL_COUNT 5

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

	std::vector<std::vector<ofImage>> mImages;
	std::vector<ofFbo> mSpriteAtlasFbos;
	std::vector<std::string> mAnimationSpeeds;

	int mOffset;
	int mHorizontalFrameCount;
	int mVerticalFrameCount;
	int mPageCount;
	float mFrameRate;
	float mAnimationSpeed;
	int mPageIndex;

	ofFbo mFbo;
	ofShader mShader;
	ofPlanePrimitive mPlane;

	ofRectangle mAnimationPreviewAreaBounds;
	ofRectangle mSpriteAtlasPreviewAreaBounds;

	ofxJSONElement mJson;

	ofxImGui::Gui gui;
	std::vector<AnimationProps> mAnimationProps;

	void setup();
	void update();
	void draw();

	std::vector<ofImage> loadImagesFromDirectory(const string &path);
	void createSpriteAtlas();
	void exportResources();
	void clear();
	void clearImages();

	void keyReleased(int key);
	void dragEvent(ofDragInfo dragInfo);
};
