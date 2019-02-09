#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    
    mFbo.allocate(static_cast<int>(MAX_WIDTH), static_cast<int>(MAX_WIDTH), GL_RGBA, 4);
    
    mOffset = 0;
    mFrameRate = 30.f;
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofClear(0);
    
    mFbo.draw((ofGetWidth() - DISPLAY_WIDTH) * 0.5, (ofGetHeight() - DISPLAY_WIDTH) * 0.5, DISPLAY_WIDTH, DISPLAY_WIDTH);
}

//--------------------------------------------------------------
void ofApp::loadImage(const string &path, const int index)
{
    ofFile _file(path);
    
    if (_file.isDirectory())
    {
        const auto name_ = _file.getFileName();
        
        ofDirectory _dir(_file.getAbsolutePath());
        _dir.allowExt("png");
        _dir.listDir();
        
        auto &_files = _dir.getFiles();
        
        mSpriteInfoJson["animations"][index] = Json::objectValue;
        mSpriteInfoJson["animations"][index]["name"] = name_;
        mSpriteInfoJson["animations"][index]["count"] = (int)_files.size();
        mSpriteInfoJson["animations"][index]["start"] = mOffset;
        mSpriteInfoJson["animations"][index]["end"] = mOffset + (int)_files.size();
        mSpriteInfoJson["animations"][index]["duration"] = ((float)_files.size() / mFrameRate) * 1000.f;
        
        mOffset += ((int)_files.size() + 1);
        
        for (auto i = 0; i < _files.size(); ++i)
        {
            loadImage(_files.at(i).getAbsolutePath(), index);
        }
    }
    else
    {
        ofImage _image;
        _image.load(path);
        
        mImages.emplace_back(_image);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (' ' == key)
    {
        ofImage img_;
        mFbo.readToPixels(img_.getPixels());
        
        auto filenamePrefix_ = "export/" + ofGetTimestampString();
        img_.save(ofToDataPath(filenamePrefix_ + ".png"));
        
        mSpriteInfoJson.save(ofToDataPath(filenamePrefix_ + ".json"));
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{ 
    if (0 > dragInfo.files.size())
    {
        return;
    }
    
    mImages.clear();
    
    mSpriteInfoJson.clear();
    mSpriteInfoJson["animations"] = Json::arrayValue;
    mOffset = 0;
    
    for (auto i = 0; i < dragInfo.files.size(); ++i)
    {
        loadImage(dragInfo.files.at(i), i);
    }
    
    mSpriteInfoJson["total"] = mSpriteInfoJson["animations"].size();
    mSpriteInfoJson["frame_rate"] = mFrameRate;
    mSpriteInfoJson["size"]["width"] = (int)MAX_WIDTH;
    mSpriteInfoJson["size"]["height"] = (int)MAX_WIDTH;

    const auto numCells = std::ceil(std::sqrt(mImages.size()));
    const auto cellSize_ = MAX_WIDTH / numCells;
    
    ofLogNotice(__PRETTY_FUNCTION__) << "numCells: " << numCells << ", cellSize: " << cellSize_;
    
    mFbo.begin();
    ofClear(0, 0, 0, 0);
    
    ofImage image_;
    
    for (auto i = 0; i < mImages.size(); ++i)
    {
        image_ = mImages.at(i);
        
        if (image_.isAllocated())
        {
            const auto w_ = image_.getWidth();
            const auto h_ = image_.getHeight();
            
            const auto horizontalIndex_ = i % static_cast<int>(numCells);
            const auto verticalIndex_ = static_cast<int>(std::floor(static_cast<double>(i) / numCells)) % static_cast<int>(numCells);
            
            auto ratio_ = cellSize_ / w_;
            
            image_.resize(cellSize_, h_ * ratio_);
            image_.update();
            
//            ofLogNotice(__PRETTY_FUNCTION__) << dragInfo.files.at(i);
            ofLogNotice(__PRETTY_FUNCTION__) << "x: " << image_.getWidth() << ", y: " << image_.getHeight();
            
            image_.draw(((cellSize_ - image_.getWidth()) * 0.5) + (cellSize_ * horizontalIndex_), ((cellSize_ - image_.getHeight()) * 0.5) + (cellSize_ * verticalIndex_));
//                image_.draw(((cellSize_ - image_.getWidth()) * 0.5) + (cellSize_ * horizontalIndex_), (image_.getHeight() * verticalIndex_));
        }
    }
    
    mFbo.end();
}
