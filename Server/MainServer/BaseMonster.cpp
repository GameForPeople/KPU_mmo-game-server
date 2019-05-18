#include "pch.h"

#include "ObjectInfo.h"
#include "BaseMonster.h"

BaseMonster::BaseMonster(_KeyType inKey, _PosType inX, _PosType inY, const MonsterModel* const inMonsterModel)
	: objectInfo(nullptr)
	, monsterModel(inMonsterModel)
	, level()
	, spawnPosX(inX)
	, spawnPosY(inY)
	, isDead(false)
	, noDamageTick(0)
	, faintTick(0)
	, freezeTick(0)
	, electricTick(0)
	, burnTick(0)
{
	objectInfo = new ObjectInfo(inKey, inX, inY);
	level = ( rand() % monsterModel->levelMaxDifference ) + monsterModel->startLevel;

	hp = monsterModel->hpPerLevel * level;
	damage = monsterModel->damagePerLevel * level;
}

BaseMonster::~BaseMonster()
{
	// ���� ���� �����Ǽ��� �ȵ˴ϴ�.
	// monsterModel = nullptr;

	delete objectInfo;
}