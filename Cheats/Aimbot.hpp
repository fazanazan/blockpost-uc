#pragma once

class Aimbot {
public:
	static void Run() {
		if (!(GetAsyncKeyState('V') & 0x8000)) return;

		InitializeStaticVariables();

		Controll_StaticFields* GameControll = GameManager->GetGameControll();
		if (!GameControll) return;
		if (!GameControll->local_player) return;

		Aimbot::Players = GameManager->GetPlayers();

		for (size_t i = 0; i < Aimbot::Players.size(); i++)
		{
			PlayerData* Player = Aimbot::Players[i];

			if (Player->idx == GameControll->local_player->idx) continue;

			if (!GameManager->TeamCheck(Player)) continue;
			if (Player->spawnprotect) continue;
			if (Player->health <= 10) continue;

			UnityEngine::GameObject* GameObject = Player->head_bone;
			if (!GameObject) continue;

			UnityEngine::Transform* Transform = UnityEngine::GameObject::GetTransform(GameObject);
			if (!Transform) continue;

			Vector3 HeadPosition = UnityEngine::Transform::GetPosition(Transform);

			if (!GameControll->transform_cam) continue;

			Vector3 CameraPosition = UnityEngine::Transform::GetPosition(GameControll->transform_cam);
			Vector3 CameraForward = UnityEngine::Transform::GetForward(GameControll->transform_cam);

			CameraPosition += CameraForward;

			if (!GameControll->camera) continue;

			Vector3 HeadW2S = UnityEngine::Camera::WorldToScreen(GameControll->camera, HeadPosition);
			if (HeadW2S.z < 1.f) continue;

			Vector3 CameraW2S = UnityEngine::Camera::WorldToScreen(GameControll->camera, CameraPosition);
			CameraW2S.y = UnityEngine::Screen::GetResolution().height - CameraW2S.y;

			Vector2 Camera2D{ CameraW2S.x - HeadW2S.x, CameraW2S.y - HeadW2S.y };

			if (Camera2D.length() < Aimbot::BestFov) {
				Aimbot::BestFov = Camera2D.length();
				Aimbot::Target = Player;
			}
		}

		if (Aimbot::Target) {
			UnityEngine::GameObject* GameObject = Aimbot::Target->head_bone;
			if (!GameObject) return;

			UnityEngine::Transform* Transform = UnityEngine::GameObject::GetTransform(GameObject);
			if (!Transform) return;

			Vector3 BonePosition = UnityEngine::Transform::GetPosition(Transform);
			if (BonePosition.length() <= 0.f) return;

			BonePosition.y += 0.4f;

			Vector3 CameraPosition = UnityEngine::Transform::GetPosition(GameControll->transform_cam);

			Vector2 CalculateAngle = Aimbot::CalculateAngle(CameraPosition, BonePosition);

			if (CalculateAngle.length() <= 0.f) return;

			GameControll->view_angle = CalculateAngle;
		}
	}
private:
	static inline float BestFov = 90.f;
	static inline PlayerData* Target = nullptr;
	static inline std::vector<PlayerData*> Players{};

	static void InitializeStaticVariables() {
		Aimbot::BestFov = 90.f;
		Aimbot::Target = nullptr;
	}

	static Vector2 CalculateAngle(Vector3 src, Vector3 dst) {
		Vector3 delta = dst - src;

		float x = static_cast<float>((atan2(delta.z, delta.x) * 180.0 / M_PI) - 90.f);
		float y = static_cast<float>((0 - atan2(delta.y, std::sqrtf(delta.x * delta.x + delta.z * delta.z)) * 180.0 / M_PI));

		return Vector2{ -x, y };
	};
};