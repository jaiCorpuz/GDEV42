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


void Player::Update(float delta_time) {
    current_state->Update(delta_time);

    if (isInvincible) {
        invincibleTimer -= delta_time;

        if (invincibleTimer <= 0.0f)
        {
            isInvincible = false;
        }
    }
}

void Player::Draw() {
    DrawCircleV(position, radius, color);
}

Player::Player(Vector2 pos, float rad, float spd) {
    position = pos;
    radius = rad;
    speed = spd;

    attackDuration = 0.5f;
    dodgeDuration  = 0.3f;
    invincibleDuration = 0.5f;

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
    player->position = Vector2Add(
        player->position,
        Vector2Scale(player->velocity, player->speed * delta_time)
    );

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

    DrawCircleV(player->position, player->radius + 25, VIOLET);  // HARD CODED
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

    //When dodging, move player in direction faster
    player->position = Vector2Add(
        player->position,
        Vector2Scale(player->dodgeDirection, player->speed * 2 * delta_time)
    );

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

    hp -= damage;

    isInvincible = true;
    invincibleTimer = invincibleDuration;
}



float PlayerBlocking::GetDamageMult() {
    return 0.0f;
}

float PlayerDodging::GetDamageMult() {
    return 0.5f;
}

