#include <pch-il2cpp.h>
#include "util.h"

#include <Windows.h>
#include <helpers.h>

// Getting resource from the memory
bool GetResourceMemory(HINSTANCE hInstance, int resId, LPBYTE& pDest, DWORD& size) 
{
    HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resId), RT_RCDATA);
    if (hResource) {
        HGLOBAL hGlob = LoadResource(hInstance, hResource);
        if (hGlob) {
            size = SizeofResource(hInstance, hResource);
            pDest = static_cast<LPBYTE>(LockResource(hGlob));
            if (size > 0 && pDest)
                return true;
        }
    }
    return false;
}

// Game utils

app::Vector3 GetRelativePosition(app::BaseEntity* entity)
{
    return app::Transform_get_position(entity->fields._transform_k__BackingField, nullptr);
}

void SetRelativePosition(app::BaseEntity* entity, app::Vector3 position)
{
    app::Transform_set_position(entity->fields._transform_k__BackingField, position, nullptr);
}

app::BaseEntity* GetAvatarEntity()
{
    if (!IsSingletonLoaded(EntityManager))
        return nullptr;

    auto entityManager = GetSingleton(EntityManager);
    auto avatarEntity = app::EntityManager_GetCurrentAvatar(entityManager, nullptr);
    return avatarEntity;
}

app::Vector3 GetAvatarRelativePosition()
{
    if (!IsSingletonLoaded(EntityManager))
        return app::Vector3();

    return GetRelativePosition(GetAvatarEntity());
}

void SetAvatarRelativePosition(app::Vector3 position)
{
    if (!IsSingletonLoaded(EntityManager))
        return;

    SetRelativePosition(GetAvatarEntity(), position);
}

float GetDistToAvatar(app::BaseEntity* entity)
{
    if (entity == nullptr || entity->fields._transform_k__BackingField == nullptr)
        return 0;

    auto dist = app::Vector3_Distance(nullptr, GetAvatarRelativePosition(), GetRelativePosition(entity), nullptr);
    return dist;
}

bool IsEntityFilterValid(app::BaseEntity* entity, const EntityFilter& filter) 
{
    if (entity == nullptr)
        return false;

    if (filter.typeFilter.enabled && entity->fields.entityType != filter.typeFilter.value)
        return false;

    if (filter.nameFilter.enabled)
    {
        bool found = false;
        auto name = il2cppi_to_string(app::BaseEntity_ToStringRelease(entity, nullptr));
        for (auto& pattern : filter.nameFilter.value)
        {
            if (name.find(pattern) != -1) {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;
}

std::vector<app::BaseEntity*> FindEntities(const EntityFilter& filter)
{
    std::vector<app::BaseEntity*> result {};

    if (!IsSingletonLoaded(EntityManager))
        return result;

    auto entityManager = GetSingleton(EntityManager);
    if (entityManager == nullptr)
        return result;

    auto entities = ToUniList(entityManager->fields._entities, app::BaseEntity*);
    if (entities == nullptr)
        return result;

    for (auto& entity : *entities) 
    {
        if (IsEntityFilterValid(entity, filter))
            result.push_back(entity);
    }

    return result;
}

app::BaseEntity* FindNearestEntity(const EntityFilter& filter)
{
    auto entities = FindEntities(filter);
    app::BaseEntity* minDistEntity = nullptr;
    float minDistance = 100000;
    for (auto& entity : entities) 
    {
        auto dist = GetDistToAvatar(entity);
        if ( dist < minDistance)
        {
            minDistance = dist;
            minDistEntity = entity;
        }
    }
    return minDistEntity;
}

const EntityFilter& GetFilterCrystalShell() 
{
    static const EntityFilter crystallShellFilter = {
        {true, app::EntityType__Enum_1::GatherObject},
        {true, {
            /*Anemoculus, Geoculus*/ "CrystalShell" ,
            /*Crimson Agate*/ "Prop_Essence_01",
            /*Electroculus*/ "ElectricCrystal"
            }
        }
    };
    return crystallShellFilter;
}

const EntityFilter& GetFilterChest() 
{
    static const EntityFilter filter = {
        {true, app::EntityType__Enum_1::Chest}
    };
    return filter;
}

bool IsEntityCrystalShell(app::BaseEntity* entity) {

    return IsEntityFilterValid(entity, GetFilterCrystalShell());
}

