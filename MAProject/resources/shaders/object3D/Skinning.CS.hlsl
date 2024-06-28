[numthreads(1024, 1, 1)]

struct Well{
	float32_t4x4 skeletonSpaceMatrix;
	float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

struct TransformationMatrix {
	float32_t4x4 WVP;
	float32_t4x4 World;
	float32_t4x4 WorldInverseTranspose;
};

struct Vertex{
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
};

struct VertexInfluence{
    float32_t4 weight;
    int32_t4 index;
}

struct SkinningInformation{
    uint32_t numVertices;
}

//SkinningObject3d.VS.hlslと同じPalette
StructuredBuffer<Well> gMatrixParette : register(t0);

//VertexBufferViewのstream0として利用していた入力頂点
StructuredBuffer<Vertex> gInputVertices : register(t1);

//VertexBufferViewのstream1として利用していた入力インフルエンス
StructuredBuffer<VertexInfluence> gInfluences : register(t2);

//Skinning計算後の頂点データ。SkinningVertex
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);

//Skinningに関するちょっとした情報
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);

void main(uint32_t3 DTid : SV_DispatchThreadID){
    uint32_t vertexIndex = DTid.x;
    if(vertexIndex < gSkinningInformation.numVertices){
        //必要なデータをStructuredBufferから取ってくる。
        //SkinningObject3d.VSでは入力頂点として受け取っていた
        Vertex input = gInputVertices[vertexIndex];
        VertexInfluence influence = gInfluences[vertexIndex];

        //skinning後の頂点を計算
        Vertex skinned;
        skinned.texcoord = input.texcoord;

        //計算の方法はSkinningObject3d.VSと同じ
        //データの取得方法が変わるだけで、原理は変わらない
        //位置の変換
        skinned.position = mul(input.position, gMatrixParette[influence.index.x].skeletonSpaceMatrix) * influence.weight.x;
        skinned.position += mul(input.position, gMatrixParette[influence.index.y].skeletonSpaceMatrix) * influence.weight.y;
	    skinned.position += mul(input.position, gMatrixParette[influence.index.z].skeletonSpaceMatrix) * influence.weight.z;
	    skinned.position += mul(input.position, gMatrixParette[influence.index.w].skeletonSpaceMatrix) * influence.weight.w;
	    skinned.position.w = 1.0f;//確実に１を入れる

        //法線の変換
        skinned.normal = mul(input.normal, (float32_t3x3)gMatrixParette[influence.index.x].skeletonSpaceInverseTransposeMatrix) * influence.weight.x;
	    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixParette[influence.index.y].skeletonSpaceInverseTransposeMatrix) * influence.weight.y;
    	skinned.normal += mul(input.normal, (float32_t3x3)gMatrixParette[influence.index.z].skeletonSpaceInverseTransposeMatrix) * influence.weight.z;
	    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixParette[influence.index.w].skeletonSpaceInverseTransposeMatrix) * influence.weight.w;
	    skinned.normal = normalize(skinned.normal);//正規化して戻してあげる


        //Skinning後の頂点データを格納
        gOutputVertices[vertexIndex] = skinned;
    }
}