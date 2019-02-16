#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    
    mFbo.allocate(static_cast<int>(MAX_WIDTH), static_cast<int>(MAX_WIDTH), GL_RGBA, 4);
    
    mShader.load("shaders/sprite_animation");

	mPlane.set(ofGetHeight() / (float)CELL_COUNT, ofGetHeight() / (float)CELL_COUNT, 2, 2);
	mPlane.mapTexCoords(0, 0, mFbo.getWidth(), mFbo.getHeight());
    
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
	ofClear(210, 210, 210, 255);

	ofPushStyle();
	{
		ofSetColor(100, 255);
		ofDrawRectangle(1000, 0, 500, ofGetHeight());

		ofSetColor(20, 20, 20, 255);
		ofDrawRectangle(1000, (ofGetHeight() - PREVIEW_SIZE), PREVIEW_SIZE, PREVIEW_SIZE);

		ofSetColor(255, 255);
		mFbo.draw(1000, (ofGetHeight() - PREVIEW_SIZE), PREVIEW_SIZE, PREVIEW_SIZE);
	}
	ofPopStyle();

	auto time = ofGetElapsedTimef();

	const auto &animations_ = mSpriteInfoJson["animations"];

	for (auto i = 0; i < animations_.size(); ++i)
	{
		ofPushStyle();
		{
			if (i % 2 == 0)
			{
				ofSetColor(50, 50, 50, 255);
			}
			else
			{
				ofSetColor(20, 20, 20, 255);
			}

			ofDrawRectangle((i % CELL_COUNT) * 200, std::floor((float)i / (float)CELL_COUNT) * 200, 200, 200);

			ofSetColor(255, 255);
			ofDrawBitmapString(
				"name : " + animations_[i]["name"].asString(),
				mPlane.getWidth() * (i % CELL_COUNT) + 20,
				mPlane.getHeight() * std::floor((float)i / (float)CELL_COUNT) + 30);
		}
		ofPopStyle();

		mShader.begin();
		{
			mShader.setUniformTexture("mainTex", mFbo, 0);
			mShader.setUniform4f("color", ofFloatColor(1.0, 1.0, 1.0, 1.0));
			mShader.setUniform2f("textureResolution", glm::vec2(mFbo.getWidth(), mFbo.getHeight()));
			mShader.setUniform1f("gridPerSide", (float)mNumCells);
			mShader.setUniform1f("totalFrameCount", animations_[i]["count"].asFloat());
			mShader.setUniform1f("totalTimeMillis", animations_[i]["duration"].asFloat());
			mShader.setUniform1f("startTime", 0.0);
			mShader.setUniform1f("frameOffset", animations_[i]["offset"].asFloat());
			mShader.setUniform1f("time", time);

			ofPushMatrix();
			{
				ofTranslate(
					mPlane.getWidth() * (i % CELL_COUNT + 1) - mPlane.getWidth() * 0.5,
					mPlane.getHeight() * std::floor((float)i / (float)CELL_COUNT) + mPlane.getHeight() * 0.5);

				mPlane.draw();
			}
			ofPopMatrix();
		}
		mShader.end();
	}
    
    gui.begin();

	ImGui::SetWindowSize(ImVec2(ofGetWidth() - 1000, ofGetHeight() - PREVIEW_SIZE));
	ImGui::SetWindowPos(ImVec2(1000, 0));
	ImGui::Text(ofToString("Sprite Size : " + ofToString(MAX_WIDTH) + " x " + ofToString(MAX_WIDTH)).c_str());
	ImGui::Spacing();
	ImGui::Text(ofToString("Animations : " + ofToString(animations_.size())).c_str());
	ImGui::Text(
		ofToString(
			"Size Per Animations : " + 
			ofToString(std::round(mSpriteInfoJson["frame_size"]["width"].asFloat())) + 
			" x " + 
			ofToString(std::round(mSpriteInfoJson["frame_size"]["height"].asFloat()))).c_str());

	if (ImGui::Button("Clear"))
	{
		clear();
	}

	if (ImGui::Button("Export"))
	{
		exportResources();
	}
    
    gui.end();
}

//--------------------------------------------------------------
void ofApp::loadImagesFromDirectory(const string &path)
{
	const ofFile f_(path);
	const auto name_ = f_.getFileName();

	ofDirectory dir_(f_.getAbsolutePath());
	dir_.allowExt("png");
	dir_.listDir();
	dir_.sort();
	
	mSpriteInfoJson["animations"].append(Json::objectValue);

	const auto index = mSpriteInfoJson["animations"].size() - 1;
	mSpriteInfoJson["animations"][index]["name"] = name_;
	mSpriteInfoJson["animations"][index]["count"] = dir_.getFiles().size();
	mSpriteInfoJson["animations"][index]["offset"] = mOffset;
	mSpriteInfoJson["animations"][index]["duration"] = ((float)dir_.getFiles().size() / mFrameRate) * 1000.f;

	const auto &files_ = dir_.getFiles();

	for (auto &file_ : files_)
	{
		if (file_.isDirectory())
		{
			ofLogWarning() << file_.getFileName() << " is directory. Skip loading.";
			continue;
		}

		ofImage _image;
		_image.load(file_.getAbsolutePath());

		mImages.push_back(_image);

		++mOffset;
	}
}

//--------------------------------------------------------------
void ofApp::createSpriteAtlas()
{
	mNumCells = (int)std::ceil(std::sqrt(mImages.size()));
	const auto cellSize_ = MAX_WIDTH / mNumCells;

	mSpriteInfoJson["total"] = mSpriteInfoJson["animations"].size();
	mSpriteInfoJson["frame_rate"] = mFrameRate;
	mSpriteInfoJson["frame_count_per_side"] = mNumCells;
	mSpriteInfoJson["frame_size"]["width"] = cellSize_;
	mSpriteInfoJson["frame_size"]["height"] = cellSize_;
	mSpriteInfoJson["size"]["width"] = (int)MAX_WIDTH;
	mSpriteInfoJson["size"]["height"] = (int)MAX_WIDTH;

	mFbo.begin();
	{
		ofClear(0, 0, 0, 0);

		ofImage image_;

		for (auto i = 0; i < mImages.size(); ++i)
		{
			image_ = mImages.at(i);

			const auto w_ = image_.getWidth();
			const auto h_ = image_.getHeight();

			const auto horizontalIndex_ = i % (int)mNumCells;
			const auto verticalIndex_ = (int)std::floor((float)i / (float)mNumCells);

			auto ratio_ = w_ < h_ ? cellSize_ / h_ : cellSize_ / w_;

			image_.resize(w_ * ratio_, h_ * ratio_);
			image_.update();

			image_.draw(
				((cellSize_ - image_.getWidth()) * 0.5) + (cellSize_ * horizontalIndex_),
				((cellSize_ - image_.getHeight()) * 0.5) + (cellSize_ * verticalIndex_));
		}
	}
	mFbo.end();
}

//--------------------------------------------------------------
void ofApp::exportResources()
{
	ofImage img_;
	mFbo.readToPixels(img_.getPixels());

	auto timestamp_ = ofGetTimestampString();
	auto prefix_ = "export/";
	img_.save(ofToDataPath(prefix_ + timestamp_ + ".png"));

	mSpriteInfoJson["filename"] = timestamp_ + ".png";
	mSpriteInfoJson.save(ofToDataPath(prefix_ + timestamp_ + ".json"), true);
}

//--------------------------------------------------------------
void ofApp::clear()
{
	for (auto &image : mImages)
	{
		image.clear();
	}
	mImages.clear();

	mSpriteInfoJson["animations"].clear();
	mSpriteInfoJson["total"] = 0;
	mSpriteInfoJson["frame_size"]["width"] = 0;
	mSpriteInfoJson["frame_size"]["height"] = 0;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    if (' ' == key)
    {
        
    }
	else if ('r' == key)
	{
		mShader.load("shaders/sprite_animation");
	}
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{ 
	ofLogNotice() << "dragEvent: files = " << dragInfo.files.size();

    if (0 > dragInfo.files.size())
    {
        return;
    }

	for (auto &file_ : dragInfo.files)
	{
		ofFile f_(file_);

		if (!f_.isDirectory())
		{
			ofLogError() << f_.getFileName() << " is not directory.";
			continue;
		}
		
		loadImagesFromDirectory(f_.getAbsolutePath());
	}

	createSpriteAtlas();
}
