#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Camera.h"

class GameHandler
{
	public:
		GameHandler(DX::DeviceResources * resources);
		~GameHandler();

		void initResources();
		void initWindowDependentResources();
		void render();
		void onDeviceLost();
		void update(DX::StepTimer const& timer);
		void setWindow(HWND window);

private:
		DX::DeviceResources * resources;
		std::unique_ptr<Camera> camera;

		// Temp
		std::unique_ptr<DirectX::BasicEffect> basicEffect;
		std::unique_ptr<DirectX::BasicEffect> effectShape;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> primitiveBatch;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>> batch;
		std::unique_ptr<DirectX::GeometricPrimitive> shape;
		DirectX::SimpleMath::Matrix mWorld;
		DirectX::SimpleMath::Matrix mView;
		DirectX::SimpleMath::Matrix mProj;

		std::unique_ptr<DirectX::DescriptorHeap> m_resourceDescriptors;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;
		std::unique_ptr<DirectX::CommonStates> m_states;

		enum Descriptors
		{
			Earth,
			Count
		};

		std::unique_ptr<DirectX::Keyboard> keyboard;
		std::unique_ptr<DirectX::Mouse> mouse;
};

