#pragma once

#include "ofMain.h"
#include "ofxJSON.h"

#define MAX_WIDTH 4096.0
#define DISPLAY_WIDTH 1024.0

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

    void loadImage(const string &path, const int index);
    
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    
    std::vector<ofImage> mImages;
    int mOffset;
    float mFrameRate;
    
    ofFbo mFbo;
    ofShader mShader;
    
    ofxJSONElement mSpriteInfoJson;
    
private:
    
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
