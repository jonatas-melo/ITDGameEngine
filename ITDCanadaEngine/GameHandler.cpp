#include "pch.h"
#include "GameHandler.h"
#include "GameObject.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

GameHandler::GameHandler(DX::DeviceResources* resources)
	: resources(resources)
{
	keyboard = std::make_unique<Keyboard>();
	mouse = std::make_unique<Mouse>();
}

GameHandler::~GameHandler()
{
}

void GameHandler::initResources()
{
	m_resourceDescriptors = std::make_unique<DescriptorHeap>(resources->GetD3DDevice(),
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		Descriptors::Count);

	m_states = std::make_unique<CommonStates>(resources->GetD3DDevice());

	ResourceUploadBatch resourceUpload(resources->GetD3DDevice());

	resourceUpload.Begin();

	DX::ThrowIfFailed(
		CreateWICTextureFromFile(resources->GetD3DDevice(), resourceUpload, L"dirt.png",
			m_texture.ReleaseAndGetAddressOf(), false));

	CreateShaderResourceView(resources->GetD3DDevice(), m_texture.Get(),
		m_resourceDescriptors->GetCpuHandle(Descriptors::Earth));

	auto uploadResourcesFinished = resourceUpload.End(resources->GetCommandQueue());

	uploadResourcesFinished.wait();

	camera = std::make_unique<Camera>();
	mWorld = Matrix::Identity;
	batch = std::make_unique<PrimitiveBatch<VertexPositionNormalTexture>>(resources->GetD3DDevice());

	RenderTargetState rtState(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT);

	CD3DX12_RASTERIZER_DESC rastDesc(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE, FALSE,
		D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE,
		0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	EffectPipelineStateDescription pdShape(
		&GeometricPrimitive::VertexType::InputLayout,
		CommonStates::Opaque,
		CommonStates::DepthDefault,
		rastDesc,
		rtState);

	effectShape = std::make_unique<BasicEffect>(resources->GetD3DDevice(), EffectFlags::Texture, pdShape);
	effectShape->EnableDefaultLighting();
	effectShape->SetTexture(m_resourceDescriptors->GetGpuHandle(Descriptors::Earth), m_states->AnisotropicWrap());

	// shape = GeometricPrimitive::CreateSphere(0.1f);
	// MAX DIVISIONS 63
	GameObject obj(3.f / float(63), 63);
	obj.createVertices();
	shape = GeometricPrimitive::CreateCustom(obj.vertexList, obj.triangles, resources->GetD3DDevice());
}

void GameHandler::initWindowDependentResources()
{
	auto m_outputSize = resources->GetOutputSize();
	UINT backBufferWidth = std::max<UINT>(m_outputSize.right - m_outputSize.left, 1);
	UINT backBufferHeight = std::max<UINT>(m_outputSize.bottom - m_outputSize.top, 1);

	//mView = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f), (Vector3(0.f, 4.f, 0.f)), Vector3::UnitY);
	camera->GetViewMatrix(mView);
	mProj = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(70.f), float(backBufferWidth) / float(backBufferHeight), 0.01f, 100.f);
}

void GameHandler::render()
{
	camera->GetViewMatrix(mView);

	// Shape
	ID3D12DescriptorHeap* heaps[] = { m_resourceDescriptors->Heap(), m_states->Heap() };
	resources->GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);

	effectShape->SetMatrices(mWorld, mView, mProj);
	effectShape->Apply(resources->GetCommandList());
	shape->Draw(resources->GetCommandList());

	effectShape->SetMatrices(mWorld, mView, mProj);
	effectShape->Apply(resources->GetCommandList());
	shape->Draw(resources->GetCommandList());
}

void GameHandler::onDeviceLost()
{
	m_texture.Reset();
	m_states.reset();
	m_resourceDescriptors.reset();
	batch.reset();
	shape.reset();
	effectShape.reset();
}

void GameHandler::update(DX::StepTimer const & timer)
{
	auto keyboardState = keyboard->GetState();
	if (keyboardState.Escape)
	{
		PostQuitMessage(0);
	}

	if (keyboardState.Home)
	{
		camera->ResetCamera();
	}

	Vector3 directionMovement = Vector3::Zero;

	if (keyboardState.Up || keyboardState.W)
	{
		directionMovement.z -= 1.f;
	}
	if (keyboardState.Down || keyboardState.S)
	{
		directionMovement.z += 1.f;
	}
	if (keyboardState.Left || keyboardState.A)
	{
		directionMovement.x -= 1.f;
	}
	if (keyboardState.Right || keyboardState.D)
	{
		directionMovement.x += 1.f;
	}
	if (keyboardState.PageUp || keyboardState.Space)
	{
		directionMovement.y += 1.f;
	}
	if (keyboardState.PageDown || keyboardState.LeftControl)
	{
		directionMovement.y -= 1.f;
	}
	if (keyboardState.F)
	{
		camera->Tilt(-float(timer.GetElapsedSeconds()));
	}
	if (keyboardState.R)
	{
		camera->Tilt(float(timer.GetElapsedSeconds()));
	}
	if (keyboardState.Q)
	{
		camera->Pan(float(timer.GetElapsedSeconds()));
	}
	if (keyboardState.E)
	{
		camera->Pan(-float(timer.GetElapsedSeconds()));
	}

	auto mouseState = mouse->GetState();

	if (mouseState.positionMode == Mouse::MODE_RELATIVE)
	{
		camera->Tilt(((mouseState.y < 0) - (mouseState.y > 0)) * timer.GetElapsedSeconds());
		camera->Pan(((mouseState.x < 0) - (mouseState.x > 0)) * timer.GetElapsedSeconds());
	}

	mouse->SetMode(mouseState.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	//mWorld = Matrix::CreateRotationY(cosf(static_cast<float>(timer.GetTotalSeconds())));
	//Vector3 move = Vector3(0.f, 4.f, 0.f) * cosf(static_cast<float>(timer.GetTotalSeconds())) * timer.GetElapsedSeconds() * 4.f;
	directionMovement *= float(timer.GetElapsedSeconds());
	camera->UpdatePosition(directionMovement);
}

void GameHandler::setWindow(HWND window)
{
	mouse->SetWindow(window);
}
