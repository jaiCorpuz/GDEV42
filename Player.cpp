#pragma once
#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "Player.hpp"
#include "Room.cpp"
#include "Tile.cpp"


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
bool CheckTileCollisionRoom (
    Vector2 testPosition,
    float radius,
    Room* room
) {
    float scaledTile = Tile::size * Tile::scale;
    float roomX = 12 * scaledTile;
    float roomY = 10 * scaledTile;

    float rX = room->position.x * roomX;
    float rY = room->position.y * roomY;

    for (Vector2 tile : room->collidable_tiles) {
        Rectangle wall = {rX + (tile.x * scaledTile), rY + (tile.y * scaledTile), scaledTile, scaledTile};
        if (CheckCollisionCircleRec(testPosition, radius, wall)) {
            return true;
        }
    }
    return false;
}

bool CheckTileCollect (
    Vector2 testPosition,
    float radius,
    Room* room
) {
    float scaledTile = Tile::size * Tile::scale;
    float roomX = 12 * scaledTile;
    float roomY = 10 * scaledTile;

    float rX = room->position.x * roomX;
    float rY = room->position.y * roomY;

    for (Vector2 tile : room->collectable_tiles) {
        Rectangle wall = {rX + (tile.x * scaledTile), rY + (tile.y * scaledTile), scaledTile, scaledTile};
        if (CheckCollisionCircleRec(testPosition, radius, wall)) {
            room->key_collected = true;
            return true;
        }
    }
    return false;
}

bool CheckTileUnlock (
    Vector2 testPosition,
    float radius,
    Room* room
) {
    float scaledTile = Tile::size * Tile::scale;
    float roomX = 12 * scaledTile;
    float roomY = 10 * scaledTile;

    float rX = room->position.x * roomX;
    float rY = room->position.y * roomY;

    for (Vector2 tile : room->unlockable_tiles) {
        Rectangle wall = {rX + (tile.x * scaledTile), rY + (tile.y * scaledTile), scaledTile, scaledTile};
        if (CheckCollisionCircleRec(testPosition, radius, wall)) {
            for (Room* n: room->neighbors) {
                if (n != nullptr && n->is_locked) {
                    n->is_locked = false;
                    for (Room* m: n->neighbors) {
                        if (m != nullptr) {
                            m->collidable_tiles.clear();
                        }
                    }
                }
            }
            room->collidable_tiles.clear();
            return true;
        }
    }

    return false;
}

void Player::Update(float delta_time) {
    
    current_state->Update(delta_time);

    if (isInvincible) {
        invincibleTimer -= delta_time;

        if (invincibleTimer <= 0.0f)
        {
            isInvincible = false;
        }
    }

    if (slowTimer > 0) {
        slowTimer -= delta_time;
        speedMultiplier = 0.5f;
    } else {
        speedMultiplier = 1.0f;
    }

    if (obscureTimer > 0) obscureTimer -= delta_time;

}

void Player::Draw() {
    if (dynamic_cast<PlayerAttacking*>(current_state)) {
        DrawLineEx(position, tongueEndPoint, 8.0f, PINK);
        DrawCircleV(tongueEndPoint, 10.0f, MAROON);
        Vector2 mouseInWorld = GetScreenToWorld2D(GetMousePosition(), *camera);
        DrawCircleV(mouseInWorld, 5, YELLOW);
        DrawCircleV(position, radius + 25, VIOLET);
    }
    DrawCircleV(position, radius, color);

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

void PlayerIdle::Update(float delta_time) {
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

void PlayerMoving::Update(float delta_time) {
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

    float currentSpeed = player->speed * player->speedMultiplier;
    float dist = currentSpeed * delta_time;

    Vector2 nextX = { player->position.x + player->velocity.x * dist, player->position.y + player->velocity.y * dist };
        if (!CheckTileCollisionRoom(nextX, player->radius, player->current_room)) {
            player->position.x = nextX.x;
        }
    
    // Check Y movement
    Vector2 nextY = { player->position.x, player->position.y + player->velocity.y * dist };
        if (!CheckTileCollisionRoom(nextY, player->radius, player->current_room)) {
            player->position.y = nextY.y;
        }
    
    if (CheckTileCollect({nextX.x, nextY.y}, player->radius, player->current_room)) {
        player->key_collected = true;
    }
    if (player->key_collected) {
        if (CheckTileUnlock({nextX.x, nextY.y}, player->radius, player->current_room)) {
            player->key_collected = false;
        }
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

void PlayerAttacking::Update(float delta_time) {
    //count down from the active time of your Attack
    player->attackTimer -= delta_time;

    // Get mouse position in game world
    Vector2 worldMousePos = GetScreenToWorld2D(GetMousePosition(), *player->camera);
    
    Vector2 tongueStart = player->position;
    Vector2 direction = Vector2Normalize(Vector2Subtract(worldMousePos, tongueStart));
    float maxDistance = Vector2Distance(tongueStart, worldMousePos);

    // Check for collision
    float reachedDistance = 0.0f;
    float step = 4.0f;

    while (reachedDistance < maxDistance) {
        Vector2 checkpoint = Vector2Add(tongueStart, Vector2Scale(direction, reachedDistance + step));
        if (CheckTileCollisionRoom(checkpoint, 2.0f, player->current_room)) {
            break;
        }
        reachedDistance += step;
    }

    player->tongueEndPoint = Vector2Add(tongueStart, Vector2Scale(direction, reachedDistance));


    //When the attack is finished, transition back to idle
    if (player->attackTimer <= 0.0f) {
        player->SetState(&player->idle);
    }


  // HARD CODED
}

void PlayerBlocking::Update(float delta_time) {
    player->velocity = Vector2Zero();
    
    //If right mouse button released, set state to idle
    if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        player->SetState(&player->idle);
    }

}

void PlayerDodging::Update(float delta_time) {
    player->dodgeTimer -= delta_time;

    //When dodging, move player in direction faster, but if slowed, it will also be slow
    Vector2 dashedPosition = Vector2Scale(player->dodgeDirection, (player->speed * player->speedMultiplier) * 2 * delta_time);

    Vector2 nextPosition = Vector2Add(player->position, dashedPosition);

    if (!CheckTileCollisionRoom(nextPosition, player->radius, player->current_room)) {
        player->position = nextPosition;
    } else {
        player->dodgeTimer = 0; 
    }

    if (CheckTileCollect(nextPosition, player->radius, player->current_room)) {
        player->key_collected = true;
    }
    if (player->key_collected) {
        if (CheckTileUnlock(nextPosition, player->radius, player->current_room)) {
            player->key_collected = false;
        }
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

