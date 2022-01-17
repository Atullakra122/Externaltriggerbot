#include "memory.h"

#include <thread>
#include <iostream>

namespace offset {
	//client
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDB558C;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DD0AB4;
	constexpr ::std::ptrdiff_t dwForceAttack = 0x3200FC4;

	//entity
	constexpr ::std::ptrdiff_t m_iHealth = 0x100;
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
	constexpr ::std::ptrdiff_t m_iCrosshairId = 0x11838;
}

int main()
{
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");

	std::cout << std::hex << "client.dll -> 0x" << client << std::dec << std::endl;
	//hack loop
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		//skip if trigger key is not down
		if (!GetAsyncKeyState(VK_SHIFT))
			continue;

		const auto& LocalPlayer = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto& LocalHealth = memory.Read<std::int32_t>(LocalPlayer + offset::m_iHealth);

		//skip if local player is dead
		//if(!localHealth)
		//continue;
		
		const auto& crosshairID = memory.Read<std::int32_t>(LocalPlayer + offset::m_iCrosshairId);

		if (!crosshairID || crosshairID > 64)
			continue;
		const auto& player = memory.Read<std::uintptr_t>(client + offset::dwEntityList + (crosshairID - 1) * 0x10);

		//skip if player is dead
		if (!memory.Read < std::int32_t>(player + offset::m_iHealth))
			continue;

		//skip if player is on our team
		if (memory.Read<std::int32_t>(player + offset::m_iTeamNum) ==
			memory.Read<std::int32_t>(LocalPlayer + offset::m_iTeamNum))
			continue;

		memory.Write<std::uintptr_t>(client + offset::dwForceAttack, 6);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		memory.Write<std::uintptr_t>(client + offset::dwForceAttack, 4);

	}
	return 0;
}