#pragma once
#include<Windows.h>
#include<cstdint>
#include<wrl.h>
#include <cassert>
#include<dxgidebug.h>
#pragma comment(lib,"dxguid.lib")

#include"../../externals/Imgui/imgui.h"
#include"../../externals/Imgui/imgui_impl_dx12.h"
#include"../../externals/Imgui/imgui_impl_win32.h"

/*ウィンドウの生成、動作を行う*/

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace MAEngine {
	class WinApp{
	public: // 
		WinApp() = default;
		~WinApp() = default;
		WinApp(const WinApp&) = delete;
		WinApp& operator=(const WinApp&) = delete;

		/// <summary>
		/// シングルトンインスタンスの取得
		/// これにより1つしか生成されなくなる
		/// </summary>
		/// <returns>KeyInputのシングルトンインスタンス</returns>
		static WinApp* GetInstance();

		// ウィンドウサイズ
		const int32_t kWindowWidth = 1280; // 横幅
		const int32_t kWindowHeight = 720; // 縦幅
		// ウィンドウクラス名
		static const wchar_t kWindowClassName[];

		enum class SizeChangeMode {
			kNone,        //!< サイズ変更不可
			kNormal,      //!< 自由変更
			kFixedAspect, //!< アスペクト比一定
		};

	public: // 
		//初期化
		void Initialize();
		//終了処理
		void Finalize();


		/// <summary>
		/// ウィンドウプロシージャ
		/// </summary>
		/// <param name="hwnd">ウィンドウハンドル</param>
		/// <param name="msg">メッセージ番号</param>
		/// <param name="wparam">メッセージ情報1</param>
		/// <param name="lparam">メッセージ情報2</param>
		/// <returns>成否</returns>
		static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public: // メンバ関数


		/// <summary>
		/// メッセージの処理
		/// </summary>
		/// <returns>終了かどうか</returns>
		bool ProcessMessage();

		/// <summary>
		/// ウィンドウハンドルの取得
		/// </summary>
		/// <returns></returns>
		HWND GetHwnd() const { return hwnd_; }

		WNDCLASS GetWnClass() const { return wndClass_; }

		HINSTANCE GetHInstance() const { return wndClass_.hInstance; }



	private: // メンバ関数


	private: // メンバ変数
		// Window関連
		HWND hwnd_ = nullptr;   // ウィンドウハンドル
		WNDCLASS wndClass_{}; // ウィンドウクラス

		bool isFullscreen_ = false;

		SizeChangeMode sizeChangeMode_ = SizeChangeMode::kNormal;


	};
}
