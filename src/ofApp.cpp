#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    
    mFbo.allocate(static_cast<int>(MAX_WIDTH), static_cast<int>(MAX_WIDTH), GL_RGBA, 4);
    
    mShader.load("shaders/sprite_animation");
    
    mOffset = 0;
    mFrameRate = 30.f;
	mHorizontalFrameCount = 5;
	mVerticalFrameCount = 4;
	mPageCount = 3;
	mPageIndex = 0;

	mAnimationPreviewAreaBounds.set(0, 0, PREVIEW_ANIMATION_SIZE, PREVIEW_ANIMATION_SIZE);
	mSpriteAtlasPreviewAreaBounds.set(1000, (ofGetHeight() - PREVIEW_SIZE), PREVIEW_SIZE, PREVIEW_SIZE);
    
    gui.setup();

	mAnimationSpeeds.push_back("x0.5");
	mAnimationSpeeds.push_back("x0.75");
	mAnimationSpeeds.push_back("x1.0");
	mAnimationSpeeds.push_back("x1.25");
	mAnimationSpeeds.push_back("x1.5");
	mAnimationSpeeds.push_back("x1.75");
	mAnimationSpeeds.push_back("x2.0");
	mAnimationSpeed = 1.f;
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
		ofDrawRectangle(mSpriteAtlasPreviewAreaBounds.getX(),
			mSpriteAtlasPreviewAreaBounds.getY(),
			mSpriteAtlasPreviewAreaBounds.getWidth(),
			mSpriteAtlasPreviewAreaBounds.getHeight());

	}
	ofPopStyle();

	auto time = ofGetElapsedTimef();

	if (!mSpriteAtlasFbos.empty())
	{
		ofSetColor(255, 255);

		try
		{
			mSpriteAtlasFbos.at(mPageIndex).draw(
				mSpriteAtlasPreviewAreaBounds.getX(),
				mSpriteAtlasPreviewAreaBounds.getY(),
				mSpriteAtlasPreviewAreaBounds.getWidth(),
				mSpriteAtlasPreviewAreaBounds.getHeight());
		}
		catch (const exception &e)
		{

		}

		mShader.begin();
		{
			for (auto i = 0; i < mSpriteAtlasFbos.size(); ++i)
			{
				mShader.setUniformTexture("tex" + std::to_string(i), mSpriteAtlasFbos.at(i), i);
			}

			mShader.setUniform4f("color", ofFloatColor(1.0, 1.0, 1.0, 1.0));
			mShader.setUniform2f("textureResolution", glm::vec2(MAX_WIDTH, MAX_WIDTH));
			mShader.setUniform1f("horizontalFrameCount", (float)mHorizontalFrameCount);
			mShader.setUniform1f("verticalFrameCount", (float)mVerticalFrameCount);
			mShader.setUniform1f("totalFrameCount", mJson["total_frame_count"].asFloat());
			mShader.setUniform1f("durationInSeconds", mJson["duration_in_seconds"].asFloat());
			mShader.setUniform1f("startTime", 0.0);
			mShader.setUniform1f("time", time);

			ofPushMatrix();
			{
				ofTranslate(
					mAnimationPreviewAreaBounds.getWidth() * 0.5,
					mAnimationPreviewAreaBounds.getHeight() * 0.5);

				mPlane.draw();
			}
			ofPopMatrix();
		}
		mShader.end();
	}
    
    gui.begin();

	ImGui::SetWindowSize(ImVec2(ofGetWidth() - 1000, ofGetHeight() - PREVIEW_SIZE));
	ImGui::SetWindowPos(ImVec2(1000, 0));
	ImGui::Text(ofToString("Sprite Dimension : " + ofToString(MAX_WIDTH) + " x " + ofToString(MAX_WIDTH)).c_str());
	ImGui::Text(ofToString("Total Pages : " + ofToString(mSpriteAtlasFbos.size())).c_str());
	ImGui::Text(ofToString("Total Frames : " + mJson["total_frame_count"].asString()).c_str());
	ImGui::Text(ofToString("Total Duration (Sec.) : " + mJson["duration_in_seconds"].asString()).c_str());
	ImGui::Text(
		ofToString(
			"Frame Dimension : " +
			ofToString(std::round(mJson["frame_size"]["width"].asFloat())) +
			" x " +
			ofToString(std::round(mJson["frame_size"]["height"].asFloat()))).c_str());

	ImGui::Spacing();
	ImGui::InputInt("Horizontal Frame Count", &mHorizontalFrameCount, 1, 100);
	ImGui::InputInt("Vertical Frame Count", &mVerticalFrameCount, 1, 100);
	ImGui::SliderInt("Page Number", &mPageIndex, 0, (int)mSpriteAtlasFbos.size() - 1);
	ImGui::Spacing();

	int animSpeed_;

	if (ofxImGui::VectorCombo("Speed", &animSpeed_, mAnimationSpeeds))
	{
		if (animSpeed_ == 0)
		{
			mAnimationSpeed = 0.5f;
		}
		else if (animSpeed_ == 1)
		{
			mAnimationSpeed = 0.75f;
		}
		else if (animSpeed_ == 2)
		{
			mAnimationSpeed = 1.0f;
		}
		else if (animSpeed_ == 3)
		{
			mAnimationSpeed = 1.25f;
		}
		else if (animSpeed_ == 4)
		{
			mAnimationSpeed = 1.5f;
		}
		else if (animSpeed_ == 5)
		{
			mAnimationSpeed = 1.75f;
		}
		else if (animSpeed_ == 6)
		{
			mAnimationSpeed = 2.0f;
		}
	}

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
std::vector<ofImage> ofApp::loadImagesFromDirectory(const string &path)
{
	const ofFile f_(path);
	const auto name_ = f_.getFileName();

	ofDirectory dir_(f_.getAbsolutePath());
	dir_.listDir();
	dir_.sort();
	
	const auto &files_ = dir_.getFiles();
	ofLog() << "Load " << files_.size() << " images from " << dir_.getAbsolutePath();

	auto images_ = std::vector<ofImage>();
	ofImage image_;

	for (auto &file_ : files_)
	{
		if (file_.isDirectory())
		{
			ofLogWarning() << file_.getFileName() << " is directory. Skip loading.";
			continue;
		}

		if (image_.load(file_.getAbsolutePath()))
		{
			images_.push_back(image_);

			image_.clear();

			++mOffset;
		}
		else
		{
			ofLogError() << "Failed to load image from " + file_.getAbsolutePath();
		}
	}

	return images_;
}

//--------------------------------------------------------------
void ofApp::createSpriteAtlas()
{
	const auto frameWidth_ = MAX_WIDTH / (float)mHorizontalFrameCount;
	const auto frameHeight = MAX_WIDTH / (float)mVerticalFrameCount;

	mJson["frame_rate"] = mFrameRate;
	mJson["horizontal_frame_count"] = mHorizontalFrameCount;
	mJson["vertical_frame_count"] = mVerticalFrameCount;
	mJson["frame_size"]["width"] = frameWidth_;
	mJson["frame_size"]["height"] = frameHeight;
	mJson["size"]["width"] = (int)MAX_WIDTH;
	mJson["size"]["height"] = (int)MAX_WIDTH;

	mPlane.set(frameWidth_, frameHeight, 2, 2);
	mPlane.mapTexCoords(0, 0, 4096, 4096);

	mSpriteAtlasFbos.clear();

	auto totalFrames_ = 0;

	for (auto &images_ : mImages)
	{
		mSpriteAtlasFbos.push_back(ofFbo());
		auto &fbo_ = mSpriteAtlasFbos.back();

		fbo_.allocate((int)MAX_WIDTH, (int)MAX_WIDTH, GL_RGBA, 4);

		fbo_.begin();
		{
			ofClear(0, 0, 0, 0);

			int count_ = std::min(mHorizontalFrameCount * mVerticalFrameCount, (int)images_.size());
			totalFrames_ += count_;

			for (auto i = 0; i < count_; ++i)
			{
				try
				{
					auto &image_ = images_.at(i);

					const auto w_ = image_.getWidth();
					const auto h_ = image_.getHeight();

					const auto horizontalIndex_ = i % mHorizontalFrameCount;
					const auto verticalIndex_ = (int)std::floor((float)i / (float)mHorizontalFrameCount);

					auto ratio_ = w_ < h_ ? frameHeight / h_ : frameWidth_ / w_;

					image_.resize(w_ * ratio_, h_ * ratio_);
					image_.update();

					image_.draw(
						((frameWidth_ - image_.getWidth()) * 0.5) + (frameWidth_ * horizontalIndex_),
						((frameHeight - image_.getHeight()) * 0.5) + (frameHeight * verticalIndex_));
				}
				catch (const exception &e)
				{
					ofLogError() << e.what();
				}
			}
		}
		fbo_.end();
	}

	mJson["total_frame_count"] = totalFrames_;
	mJson["duration_in_seconds"] = (float)totalFrames_ / mFrameRate;
}

//--------------------------------------------------------------
void ofApp::exportResources()
{
	ofImage img_;
	auto page_ = 1;
	auto timestamp_ = ofGetTimestampString();
	auto prefix_ = "export/" + timestamp_ + "/";
	std::string path_ = "";

	mJson["pages"] = Json::arrayValue;

	for (auto &fbo_ : mSpriteAtlasFbos)
	{
		fbo_.readToPixels(img_.getPixels());

		std::stringstream ss_;
		ss_ << prefix_;
		ss_ << "page-";
		ss_ << page_;
		ss_ << ".png";

		path_ = ofToDataPath(ss_.str());

		if (img_.save(path_))
		{
			mJson["pages"][page_ - 1] = path_;
			++page_;
		}

		img_.clear();
		ss_.clear();
	}

	mJson.save(ofToDataPath(prefix_ + "animation.json"), true);
}

//--------------------------------------------------------------
void ofApp::clear()
{
	clearImages();

	for (auto &fbo_ : mSpriteAtlasFbos)
	{
		fbo_.clear();
	}

	mSpriteAtlasFbos.clear();
}

//--------------------------------------------------------------
void ofApp::clearImages()
{
	for (auto &images_ : mImages)
	{
		for (auto &image_ : images_)
		{
			image_.clear();
		}

		images_.clear();
	}

	mImages.clear();
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

	clearImages();

	for (auto &file_ : dragInfo.files)
	{
		ofFile f_(file_);

		if (!f_.isDirectory())
		{
			ofLogError() << f_.getFileName() << " is not directory.";
			continue;
		}

		mImages.push_back(loadImagesFromDirectory(f_.getAbsolutePath()));
	}

	ofLog() << "Page count : " << mImages.size();

	for (auto i = 0; i < mImages.size(); ++i)
	{
		ofLog() << "Image count in page " << (i + 1) << " : " << mImages.at(i).size();
	}

	createSpriteAtlas();

	clearImages();
}
