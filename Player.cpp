/*
|------------------------------------------------------|
|                 PLAYER STATE MACHINE                 |              
|------------------------------------------------------|
|   This is the file that contains definitions for     |
|   all the functions declared in the Player.hpp       |
|   file.                                              |
|                                                      |
|   All functions from the Player.hpp SHOULD be        |
|   defined here (For Now)                             |
|                                                      |
|------------------------------------------------------|
*/

#pragma once
#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "Player.hpp"
#include "Room.cpp"


bool CheckTileCollision (
    Vector2 testPosition,
    float radius,
    const std::vector<Room*>& rooms,
    float tileScale, 
    int tileSize
) {
    float scaledTile = (float)tileSize * tileScale;
    float roomX = 12 * scaledTile;
    float roomY = 10 * scaledTile;

    for (Room* r : rooms) {
        float rX = r->position.x * roomX;
        float rY = r->position.y * roomY;

        for (Vector2 tile : r->collidable_tiles) {
            Rectangle wall = {rX + (tile.x * scaledTile), rY + (tile.y * scaledTile), scaledTile, scaledTile};
            if (CheckCollisionCircleRec(testPosition, radius, wall)) {
                return true;
            }
        }
    }
    return false;
}

bool CheckTileCollect (
    Vector2 testPosition,
    float radius,
    const std::vector<Room*>& rooms,
    float tileScale, 
    int tileSize
) {
    float scaledTile = (float)tileSize * tileScale;
    float roomX = 12 * scaledTile;
    float roomY = 10 * scaledTile;

    for (Room* r : rooms) {
        float rX = r->position.x * roomX;
        float rY = r->position.y * roomY;

        for (Vector2 tile : r->collectable_tiles) {
            Rectangle wall = {rX + (tile.x * scaledTile), rY + (tile.y * scaledTile), scaledTile, scaledTile};
            if (CheckCollisionCircleRec(testPosition, radius, wall)) {
                r->key_collected = true;
                return true;
            }
        }
    }
    return false;
}

bool CheckTileUnlock (
    Vector2 testPosition,
    float radius,
    const std::vector<Room*>& rooms,
    float tileScale, 
    int tileSize
) {
    float scaledTile = (float)tileSize * tileScale;
    float roomX = 12 * scaledTile;
    float roomY = 10 * scaledTile;

    for (Room* r : rooms) {
        float rX = r->position.x * roomX;
        float rY = r->position.y * roomY;

        for (Vector2 tile : r->unlockable_tiles) {
            Rectangle wall = {rX + (tile.x * scaledTile), rY + (tile.y * scaledTile), scaledTile, scaledTile};
            if (CheckCollisionCircleRec(testPosition, radius, wall)) {
                for (Room* n: r->neighbors) {
                    if (n != nullptr) {
                        n->is_locked = false;
                    }
                }
                r->collidable_tiles.clear();
                return true;
            }
        }
    }
    return false;
}

void Player::Update(float delta_time, const std::vector<Room*>& rooms) {
    
    current_state->Update(delta_time, rooms);

    if (isInvincible) {
        invincibleTimer -= delta_time;

        if (invincibleTimer <= 0.0f)
        {
            isInvincible = false;
        }
    }
}

void Player::Draw() {
    if (dynamic_cast<PlayerAttacking*>(current_state)) {
        DrawCircleV(position, radius + 25, VIOLET);
    }
    DrawCircleV(position, radius, color);
    // DrawCircleSector(position, radius/2, 0, invincibleTimer/invincibleDuration*360, 20, BLACK);
}

Player::Player(Vector2 pos, float rad, float spd) {
    position = pos;
    radius = rad;
    speed = spd;

    attackDuration = 0.5f;
    dodgeDuration  = 0.3f;
    invincibleTimer = 0.0f;
    invincibleDuration = 1.0f;

    tileScale = 4.0f;
    tileSize = 16;

    idle.player = &*this;
    moving.player = &*this;
    attacking.player = &*this;
    dodging.player = &*this;
    blocking.player = &*this;

    SetState(&idle);
}

void Player::SetState(PlayerState* state) {
    if (current_state != nullptr) {
        current_state->Exit();
    }

    current_state = state;
    current_state->Enter();
}

PlayerState* Player::GetCurrentState() {
    return current_state;
}

void PlayerIdle::Enter() {
    player->color = DARKBLUE;
}

void PlayerMoving::Enter() {
    player->color = DARKGREEN;
}

void PlayerAttacking::Enter() {
    player->color = MAROON;
    player->attackTimer = player->attackDuration;
}

void PlayerDodging::Enter() {
    player->color = GOLD;

    player->dodgeTimer = player->dodgeDuration;
    //To determine the direction of the player when dodging
    player->dodgeDirection = Vector2Normalize(player->velocity);
}

void PlayerBlocking::Enter() {
    player->color = GRAY;
}

void PlayerIdle::Exit() {}

void PlayerMoving::Exit() {}

void PlayerAttacking::Exit() {}

void PlayerDodging::Exit() {}

void PlayerBlocking::Exit() {}

void PlayerIdle::Update(float delta_time, const std::vector<Room*>& rooms) {
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D)) {
        player->SetState(&player->moving);
    } 

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        player->SetState(&player->attacking);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        player->SetState(&player->blocking);
    }
}

void PlayerMoving::Update(float delta_time, const std::vector<Room*>& rooms) {
    player->velocity = Vector2Zero();
    //Movement Logic
    //Move up
    if (IsKeyDown(KEY_W)){
        player->velocity.y -= 1;
    }
    //Move left
    if (IsKeyDown(KEY_A)){
        player->velocity.x -= 1;
    }
    //Move down
    if(IsKeyDown(KEY_S)){
        player->velocity.y += 1;
    }
    //Move right
    if(IsKeyDown(KEY_D)){
        player->velocity.x += 1;
    }
    
    //If player not moving, set state to idle
    if(Vector2Length(player->velocity) == 0) {
        player->SetState(&player->idle);
    }

    //Move le player
    player->velocity = Vector2Normalize(player->velocity);
    Vector2 nextPosition = Vector2Add(
        player->position,
        Vector2Scale(player->velocity, player->speed * delta_time)
    );

    float dist = player->speed * delta_time;
    
    Vector2 nextX = { player->position.x + player->velocity.x * dist, player->position.y + player->velocity.y * dist };
    if (!CheckTileCollision(nextX, player->radius, rooms, player->tileScale, player->tileSize)) {
        player->position.x = nextX.x;
    }
    
    // Check Y movement
    Vector2 nextY = { player->position.x, player->position.y + player->velocity.y * dist };
    if (!CheckTileCollision(nextY, player->radius, rooms, player->tileScale, player->tileSize)) {
        player->position.y = nextY.y;
    }
    
    if (CheckTileCollect({nextX.x, nextY.y}, player->radius, rooms, player->tileScale, player->tileSize)) {
        player->key_collected = true;
    }
    if (CheckTileUnlock({nextX.x, nextY.y}, player->radius, rooms, player->tileScale, player->tileSize)) {
        player->key_collected = false;
    }

    //If Space while moving, set state to dodge 
    if(IsKeyPressed(KEY_SPACE)){
        player->SetState(&player->dodging);
    }

    //If left mouse button pressed, set state to attack
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        player->SetState(&player->attacking);
    }

}

void PlayerAttacking::Update(float delta_time, const std::vector<Room*>& rooms) {
    //count down from the active time of your Attack
    player->attackTimer -= delta_time;

    //When the attack is finished, transition back to idle
    if (player->attackTimer <= 0.0f) {
        player->SetState(&player->idle);
    }
  // HARD CODED
}

void PlayerBlocking::Update(float delta_time, const std::vector<Room*>& rooms) {
    player->velocity = Vector2Zero();
    
    //If right mouse button released, set state to idle
    if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        player->SetState(&player->idle);
    }

}

void PlayerDodging::Update(float delta_time, const std::vector<Room*>& rooms) {
    player->dodgeTimer -= delta_time;

    //When dodging, move player in direction faster
    Vector2 dashedPosition = Vector2Scale(player->dodgeDirection, player->speed * 2 * delta_time);

    Vector2 nextPosition = Vector2Add(player->position, dashedPosition);

    if (!CheckTileCollision(nextPosition, player->radius, rooms, player->tileScale, player->tileSize)) {
        player->position = nextPosition;
    } else {
        player->dodgeTimer = 0; 
    }

    //If dodge timer finished, set state to idle
    if (player->dodgeTimer <= 0.0f) {
        player->SetState(&player->idle);
    }

}

void Player::TakeDamage(float damage) {
    if (isInvincible) return;

    float multiplier = current_state->GetDamageMult();
    float finalDamage = damage * multiplier;
    
    if (finalDamage <= 0) return;

    hp -= finalDamage;

    isInvincible = true;
    invincibleTimer = invincibleDuration;
}

float PlayerBlocking::GetDamageMult() {
    return 0.5f;
}

float PlayerDodging::GetDamageMult() {
    return 0.0f;
}

