#include "TrailEffect.h"
#include"../../Base/DirectXCommon.h"
#include"TextureManager.h"
#include <cassert>
#include<fstream>
#include<sstream>


void TrailEffect::Initialize(int bufferSize){
	posArray_.resize(bufferSize);
}

void TrailEffect::Update(){
    //データを更新
    for (size_t i = posArray_.size() - 1; i > 0; --i){
        posArray_[i] = posArray_[i - 1];
    }
    posArray_.front() = tempPos_;
    tempPos_ = PosBuffer();

    ////曲線を作る
    std::vector<PosBuffer> usedPosArray = GetUsedPosArray();
    
    if (usedPosArray.empty())return;
    //CreateCurveVertex(usedPosArray);

    bufferSize_ = usedPosArray.size();

    //頂点データを更新する
    float amount = 1.0f / (usedPosArray.size() - 1);
    float v = 0;
    vertex_.clear();
    vertex_.resize(usedPosArray.size() * 2);
    for (size_t i = 0, j = 0; i < vertex_.size() && j < usedPosArray.size(); i += 2, ++j)
    {
        vertex_[i].position = usedPosArray[j].head;
        vertex_[i].texcoord = Vector2(1.0f, v);
        vertex_[i + 1].position = usedPosArray[j].tail;
        vertex_[i + 1].texcoord = Vector2(0.0f, v);
        v += amount;
    }
}

void TrailEffect::DrawImgui(std::string name){
    ImGui::Begin(name.c_str());
    ImGui::Text("バッファのサイズ = %d", bufferSize_);
    ImGui::End();

}

std::vector<TrailEffect::PosBuffer> TrailEffect::GetUsedPosArray(){
    std::vector<PosBuffer> usedPosArray;

    // posArray から有効な位置データを選択
    for (size_t i = 0; i < posArray_.size(); ++i) {
        // posArray[i] が有効であり、重複していない場合に選択
        if (i == 0 || posArray_[i].head != posArray_[i - 1].head) {
            usedPosArray.push_back(posArray_[i]);
        }
    }

    return usedPosArray;
}


