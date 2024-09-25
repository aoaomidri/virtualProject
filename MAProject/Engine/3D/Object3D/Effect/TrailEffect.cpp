#include "TrailEffect.h"
#include"../../Base/DirectXCommon.h"
#include"TextureManager.h"
#include <cassert>
#include<fstream>
#include<sstream>


void TrailEffect::Initialize(int bufferSize, const std::string& texturePath){
	posArray_.resize(bufferSize);
    max_ = bufferSize * 2;
    indexCount_ = (max_ - 2) * 3;
    textureHandle_ = TextureManager::GetInstance()->Load(texturePath);
    vertex_.resize(max_);
    indices_.resize(indexCount_);
    MakeVertexData();
}

void TrailEffect::Update(){
    //データを更新
    for (size_t i = posArray_.size() - 1; i > 0; --i){
        posArray_[i] = posArray_[i - 1];
    }
    posArray_.front().head = tempPos_.head;
    posArray_.front().tail = tempPos_.tail;
    tempPos_ = PosBuffer();

    ////曲線を作る
    std::vector<PosBuffer> usedPosArray = GetUsedPosArray();
    
    if (usedPosArray.empty())return;
    //CreateCurveVertex(usedPosArray);

    bufferSize_ = usedPosArray.size();

   /* vertex_.clear();
    indices_.clear();*/
    if (usedPosArray.size() > 1) {
        float amount = 1.0f / (usedPosArray.size() - 1);
        float v = 0.0f;
        vertex_.resize(usedPosArray.size() * 2);
        for (size_t j = 0; j < usedPosArray.size(); ++j) {
            // ヘッドの頂点
            vertex_[2 * j].position = usedPosArray[j].head;
            vertex_[2 * j].texcoord = Vector2(0.0f, v);

            // テールの頂点
            vertex_[2 * j + 1].position = usedPosArray[j].tail;
            vertex_[2 * j + 1].texcoord = Vector2(1.0f, v);
            vertexNum_ = vertex_.size();
            v += amount;
        }
        indices_.resize((usedPosArray.size() - 1) * 6);
        
        for (size_t i = 0; i < usedPosArray.size() - 1; ++i) {
            uint32_t headIndex = 2 * (uint32_t)(i);
            uint32_t tailIndex = headIndex + 1;

            // 三角形のインデックスを追加
            indices_[6 * i] = headIndex;     // head[j]
            indices_[6 * i + 1] = tailIndex;     // tail[j]
            indices_[6 * i + 2] = headIndex + 2; // head[j + 1]

            indices_[6 * i + 3] = tailIndex;     // tail[j]
            indices_[6 * i + 4] = tailIndex + 2; // tail[j + 1]
            indices_[6 * i + 5] = headIndex + 2; // head[j + 1]
        }

        indexNum_ = indices_.size(); // インデックス数の更新
    }
    //最後にresourceの更新
    HRESULT result = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate_));
    if (FAILED(result)) {
        // エラー処理
        throw std::runtime_error("Failed to map vertex buffer");
    }

    std::memcpy(vertexDate_, vertex_.data(), sizeof(VertexData) * vertex_.size());

    result = indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

    if (FAILED(result)) {
        // エラー処理
        throw std::runtime_error("Failed to map index buffer");
    }
    std::memcpy(indexData_, indices_.data(), sizeof(uint32_t) * indices_.size());
  
}

void TrailEffect::Draw() const{
    auto command = DirectXCommon::GetInstance()->GetCommandList();
    command->IASetVertexBuffers(0, 1, &vertexBufferView_);
    command->IASetIndexBuffer(&indexBufferView_);
}

void TrailEffect::DrawImgui(std::string name){
    ImGui::Begin(name.c_str());
    ImGui::Text("バッファのサイズ = %d", bufferSize_);
    ImGui::Text("vertexのサイズ = %d", vertexNum_);
    ImGui::Text("indexのサイズ = %d", indexNum_);
    ImGui::Text("バッファの最大サイズ = %d", posArray_.size());
    ImGui::End();

}

std::vector<TrailEffect::PosBuffer> TrailEffect::GetUsedPosArray(){
    std::vector<PosBuffer> usedPosArray;

    if (posArray_.empty()){
        return usedPosArray;
    }
    usedPosArray.push_back(posArray_.front());

    // posArray から有効な位置データを選択
    for (size_t i = 1; i < posArray_.size(); ++i) {
        // posArray[i] が有効であり、重複していない場合に選択
        if (posArray_[i].head != posArray_[i - 1].head) {
            usedPosArray.push_back(posArray_[i]);
        }
    }

    return usedPosArray;
}

void TrailEffect::MakeVertexData(){
    auto* textureManager = TextureManager::GetInstance();

    //頂点リソースの作成
    vertexResource_ = textureManager->CreateBufferResource(sizeof(VertexData) * max_);

    //リソースの先頭のアドレスから使う
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    //使用するリソースのサイズは頂点三つ分のサイズ
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * max_);
    //1頂点当たりのサイズ
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    //書き込むためのアドレスを取得
    HRESULT result = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate_));
    if (FAILED(result)) {
        // エラー処理
        throw std::runtime_error("Failed to map vertex buffer");
    }
    
    std::memcpy(vertexDate_, vertex_.data(), sizeof(VertexData) * max_);

    indexResource_ = textureManager->CreateBufferResource(sizeof(uint32_t) * indexCount_);

    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();

    indexBufferView_.SizeInBytes = sizeof(uint32_t) * indexCount_;

    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    // 書き込み用のアドレス取得

    result = indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

    if (FAILED(result)) {
        // エラー処理
        throw std::runtime_error("Failed to map index buffer");
    }
    std::memcpy(indexData_, indices_.data(), sizeof(uint32_t) * indexCount_);

}


