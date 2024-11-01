#pragma once
#include"DirectXCommon.h"
/*デストラクタにてポインタなどの解放漏れがないかを確認する*/
class ResourceObject
{
public:
	~ResourceObject();
	
};

