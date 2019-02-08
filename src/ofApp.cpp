#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    
    mFbo.allocate(static_cast<int>(MAX_WIDTH), static_cast<int>(MAX_WIDTH), GL_RGBA, 4);
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
void ofApp::loadImage(const string &path, const string &parent)
{
    ofFile _file(path);
    
    if (_file.isDirectory())
    {
        const auto name_ = _file.getFileName();
        
        ofDirectory _dir(_file.getAbsolutePath());
        _dir.allowExt("png");
        _dir.listDir();
        
        auto &_files = _dir.getFiles();
        
        mSpriteInfoJson["animations"][name_] = Json::objectValue;
        mSpriteInfoJson["animations"][name_]["count"] = (int)_files.size();
        mSpriteInfoJson["animations"][name_]["start"] = mOffset;
        mSpriteInfoJson["animations"][name_]["end"] = mOffset + (int)_files.size();
        
        mOffset += ((int)_files.size() + 1);
        
        for (auto i = 0; i < _files.size(); ++i)
        {
            loadImage(_files.at(i).getAbsolutePath(), name_);
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
        
        auto filenamePrefix_ = ofGetTimestampString();
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
    mSpriteInfoJson["animations"] = Json::objectValue;
    mOffset = 0;
    
    for (auto i = 0; i < dragInfo.files.size(); ++i)
    {
        loadImage(dragInfo.files.at(i), "");
    }

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
