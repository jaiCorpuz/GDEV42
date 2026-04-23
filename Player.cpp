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
        // std::cout << TextFormat("%d, %d", tile.x, tile.y) << std::endl;
        Rectangle wall = {rX + (tile.x * scaledTile), rY + (tile.y * scaledTile), scaledTile, scaledTile};
        if (CheckCollisionCircleRec(testPosition, radius, wall)) {
            // std::cout << TextFormat("%d, %d", tile.x, tile.y) << std::endl;
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

bool CheckTileInteract (
    InteractType interact,
    Vector2 testPosition,
    float radius,
    Room* room
) {
    float scaledTile = Tile::size * Tile::scale;
    float roomX = 12 * scaledTile;
    float roomY = 10 * scaledTile;

    float rX = room->position.x * roomX;
    float rY = room->position.y * roomY;

    switch (interact)
    {
        case InteractType::COLLIDE:
            // std::cout << "=== INTERACT collide UPDATE" << std::endl;
            for (Vector2 tile : room->collidable_tiles) {
                // std::cout << TextFormat("%d, %d", tile.x, tile.y) << std::endl;
                Rectangle wall = {rX + (tile.x * scaledTile), rY + (tile.y * scaledTile), scaledTile, scaledTile};
                // std::cout << "=== INTERACT wall UPDATE" << std::endl;
                if (CheckCollisionCircleRec(testPosition, radius, wall)) {
                    // std::cout << "=== INTERACT true UPDATE" << std::endl;
                    // std::cout << TextFormat("%d, %d", tile.x, tile.y) << std::endl;
                    return true;
                }
                // std::cout << "=== INTERACT false UPDATE" << std::endl;
            }
            return false;
            break;
        case InteractType::COLLECT:
            for (Vector2 tile : room->collectable_tiles) {
                Rectangle wall = {rX + (tile.x * scaledTile), rY + (tile.y * scaledTile), scaledTile, scaledTile};
                if (CheckCollisionCircleRec(testPosition, radius, wall)) {
                    room->key_collected = true;
                    return true;
                }
            }
            return false;
            break;
        case InteractType::UNLOCK:
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
            break;
        case InteractType::STAIRS:
            {Rectangle stairs = {rX + (room->stair_tile.x * scaledTile), rY + (room->stair_tile.y * scaledTile), scaledTile, scaledTile};
            if (CheckCollisionCircleRec(testPosition, radius, stairs)) {
                return true;
            }
            return false;}
            break;
        
        default:
            break;
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

void Player::ResetPosition() {
    this->position = {(float)screen_width/2.0f,(float)screen_height/2.0f};
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
    if (
        IsKeyDown(KEY_UP) ||
        IsKeyDown(KEY_DOWN) ||
        IsKeyDown(KEY_LEFT) ||
        IsKeyDown(KEY_RIGHT) ||
        IsKeyDown(KEY_W) ||
        IsKeyDown(KEY_A) ||
        IsKeyDown(KEY_S) ||
        IsKeyDown(KEY_D)
    ) {
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
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)){
        player->velocity.y -= 1;
    }
    //Move left
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
        player->velocity.x -= 1;
    }
    //Move down
    if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)){
        player->velocity.y += 1;
    }
    //Move right
    if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
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
        if (!CheckTileInteract(
            InteractType::COLLIDE,
            nextX,
            player->radius,
            player->current_room
        )) {
            player->position.x = nextX.x;
        }
    
    // Check Y movement
    Vector2 nextY = { player->position.x, player->position.y + player->velocity.y * dist };
    if (!CheckTileInteract(
        InteractType::COLLIDE,
        nextY,
        player->radius,
        player->current_room
    )) {
        player->position.y = nextY.y;
    }
    
    if (CheckTileInteract(
        InteractType::COLLECT,
        {nextX.x, nextY.y},
        player->radius,
        player->current_room
    )) {
        player->key_collected = true;
    }
    if (player->key_collected) {
        if (CheckTileInteract(
            InteractType::UNLOCK,
            {nextX.x, nextY.y},
            player->radius,
            player->current_room
        )) {
            player->key_collected = false;
        }
    }
    
    // if (CheckTileInteract(
    //         InteractType::STAIRS,
    //         {nextX.x, nextY.y},
    //         player->radius,
    //         player->current_room
    // )) {
         
    // }

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
    // std::cout << "=== DODGING timer UPDATE" << std::endl;
    
    //When dodging, move player in direction faster, but if slowed, it will also be slow
    Vector2 dashedPosition = Vector2Scale(player->dodgeDirection, (player->speed * player->speedMultiplier) * 2 * delta_time);
    // std::cout << "=== DODGING dashedpos UPDATE" << std::endl;
    
    Vector2 nextPosition = Vector2Add(player->position, dashedPosition);
    // std::cout << "=== DODGING nextpos UPDATE" << std::endl;
    
    if (!CheckTileInteract(
        InteractType::COLLIDE,
        nextPosition,
        player->radius,
        player->current_room
    )) {
        player->position = nextPosition;
        // std::cout << "=== DODGING no collide UPDATE" << std::endl;
    } else {
        player->dodgeTimer = 0; 
        // std::cout << "=== DODGING yes collide UPDATE" << std::endl;
    }
    // std::cout << "=== DODGING collide UPDATE" << std::endl;
    
    if (CheckTileInteract(
        InteractType::COLLECT,
        nextPosition,
        player->radius,
        player->current_room
    )) {
        player->key_collected = true;
    }
    // std::cout << "=== DODGING collect UPDATE" << std::endl;
    if (player->key_collected) {
        if (CheckTileInteract(
            InteractType::UNLOCK,
            nextPosition,
            player->radius,
            player->current_room
        )) {
            player->key_collected = false;
        }
    }
    // std::cout << "=== DODGING unlock UPDATE" << std::endl;

    if (CheckTileInteract(
            InteractType::STAIRS,
            nextPosition,
            player->radius,
            player->current_room
    )) {
        player->ResetPosition();
        player->SetState(&player->idle);
        if (player->level == 2) {
            std::cout << "=== win" << std::endl;
            player->win = true;
        } else {
            player->level++;
        }
        player->level_up = true;
    }
    // std::cout << "=== DODGING stair UPDATE" << std::endl;

    //If dodge timer finished, set state to idle
    if (player->dodgeTimer <= 0.0f) {
        player->SetState(&player->idle);
    }
    // std::cout << "=== DODGING idle UPDATE" << std::endl;

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

