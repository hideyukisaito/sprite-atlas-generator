#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    
    mFbo.allocate(static_cast<int>(MAX_WIDTH), static_cast<int>(MAX_WIDTH), GL_RGBA, 4);
    
    mShader.load(ofToDataPath("shaders/sprite_animation"));
    
    mSpriteInfoJson["animations"] = Json::arrayValue;
    
    mOffset = 0;
    mFrameRate = 30.f;
    
    gui.setup();
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
    
    gui.begin();
    
    for (auto i = 0; i < mAnimationProps.size(); ++i)
    {
        auto &props = mAnimationProps.at(i);
        
        ImGui::Text("Animation Name : %s", props.name.c_str());
        
        auto name_ = "Position " + ofToString(i);
        ImGui::SliderFloat(name_.c_str(), &props.currentTime, 0.0f, props.duration);
        
        props.index = std::ceil(props.totalFrames * (props.currentTime / props.duration));
        
        name_ = "Index " + ofToString(i);
        ImGui::SliderFloat(name_.c_str(), &props.index, 0.0f, props.totalFrames);
    }
    
    gui.end();
}

//--------------------------------------------------------------
void ofApp::loadImage(const string &path, const int index)
{
    ofFile file_(path);
    
    if (file_.isDirectory())
    {
        const auto name_ = file_.getFileName();
        
        ofDirectory dir_(file_.getAbsolutePath());
        dir_.allowExt("png");
        dir_.listDir();
        
        auto &files_ = dir_.getFiles();
        dir_.sort();
        
        auto duration = ((float)files_.size() / mFrameRate) * 1000.f;
        
        mSpriteInfoJson["animations"][index] = Json::objectValue;
        mSpriteInfoJson["animations"][index]["name"] = name_;
        mSpriteInfoJson["animations"][index]["count"] = (int)files_.size();
        mSpriteInfoJson["animations"][index]["offset"] = mOffset;
        mSpriteInfoJson["animations"][index]["duration"] = duration;
        
        auto props = AnimationProps();
        props.currentTime = 0.f;
        props.totalFrames = (float)files_.size();
        props.duration = duration;
        props.name = name_;
        mAnimationProps.push_back(props);
        
        mOffset += ((int)dir_.size() + 1);
        
        for (auto i = 0; i < dir_.size(); ++i)
        {
            loadImage(files_.at(i).getAbsolutePath(), index);
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
        
        auto timestamp_ = ofGetTimestampString();
        auto prefix_ = "export/";
        img_.save(ofToDataPath(prefix_ + timestamp_ + ".png"));
        
        mSpriteInfoJson["filename"] = timestamp_ + ".png";
        mSpriteInfoJson.save(ofToDataPath(prefix_ + timestamp_ + ".json"), true);
    }
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{ 
	ofLogNotice(__FUNCSIG__) << "dragEvent: files = " << dragInfo.files.size();

    if (0 > dragInfo.files.size())
    {
        return;
    }
    
    mImages.clear();
    mAnimationProps.clear();
    
    mSpriteInfoJson["animations"].clear();
    
    mOffset = 0;
    
    for (auto i = 0; i < dragInfo.files.size(); ++i)
    {
        loadImage(dragInfo.files.at(i), i);
    }
    
    const auto numCells_ = std::ceil(std::sqrt(mImages.size()));
    const auto cellSize_ = MAX_WIDTH / numCells_;
    
    mSpriteInfoJson["total"] = mSpriteInfoJson["animations"].size();
    mSpriteInfoJson["frame_rate"] = mFrameRate;
    mSpriteInfoJson["frame_count_per_side"] = numCells_;
    mSpriteInfoJson["frame_size"]["width"] = cellSize_;
    mSpriteInfoJson["frame_size"]["height"] = cellSize_;
    mSpriteInfoJson["size"]["width"] = (int)MAX_WIDTH;
    mSpriteInfoJson["size"]["height"] = (int)MAX_WIDTH;
    
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
            
            const auto horizontalIndex_ = i % static_cast<int>(numCells_);
            const auto verticalIndex_ = static_cast<int>(std::floor(static_cast<double>(i) / numCells_)) % static_cast<int>(numCells_);
            
            auto ratio_ = cellSize_ / w_;
            
            image_.resize(cellSize_, h_ * ratio_);
            image_.update();
            
//            ofLogNotice(__PRETTY_FUNCTION__) << dragInfo.files.at(i);
            
            image_.draw(((cellSize_ - image_.getWidth()) * 0.5) + (cellSize_ * horizontalIndex_), ((cellSize_ - image_.getHeight()) * 0.5) + (cellSize_ * verticalIndex_));
//                image_.draw(((cellSize_ - image_.getWidth()) * 0.5) + (cellSize_ * horizontalIndex_), (image_.getHeight() * verticalIndex_));
        }
    }
    
    mFbo.end();
}
