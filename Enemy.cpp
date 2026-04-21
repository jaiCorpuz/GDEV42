#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include "Enemy.hpp"
#include "Player.hpp"

bool CheckTileCollisionRoom(
    Vector2 testPosition, 
    float radius,
    Room* room
);

void Enemy::Update(float delta_time) {
    if (damageCooldownTimer > 0) {
        damageCooldownTimer -= delta_time;
    } else {
        damageCooldownTimer = 0;
    }
    
    if (attackDuration > 0.0f)
    attackDuration -= delta_time;

    current_state->Update(delta_time);

    // If Player collides with the enemy, they get get affected based on the ghost type
    if (CheckCollisionCircleRec(
        playerRef->position,
        playerRef->radius,
        {
            position.x,
            position.y,
            size,
            size
        })
    ) {
        HandlePlayerCollision();
    }

    
    if (CheckCollisionCircles(
        playerRef->position,
        playerRef->radius + 25,
        position,
        (size * sqrt(2))/2
        ) && 
        (dynamic_cast<PlayerAttacking*>(playerRef->GetCurrentState()))
    ) {
        TakeDamage();
    }
}

void Enemy::Draw() {
    Rectangle dest = {
        position.x,
        position.y,
        size,
        size
    };

    Vector2 origin = {
        size / 2.0f,
        size / 2.0f
    };

    
    if (alive) {
        DrawRectanglePro(
            dest,
            origin,
            rotation * RAD2DEG,
            color
        );
        DrawCircleLines(position.x, position.y, detectionRadius, LIGHTGRAY);
        DrawCircleLines(position.x, position.y, aggroRadius, ORANGE);
        DrawCircleLines(position.x, position.y, attackRadius, RED);
    }
}

Enemy::Enemy(Vector2 pos, float siz, float spd){
    position = pos;
    size = siz;
    speed = spd;
    hp = 2.0f;
    alive = true;

    damageCooldownDuration = 1.0f;

    aggroRadius = 300;
    detectionRadius = 200;
    attackRadius = 100;

    wandering.enemy = &*this;
    chasing.enemy = &*this;
    readyingAttack.enemy = &*this;
    attacking.enemy = &*this;

    SetState(&wandering);
}

void Enemy::SetState(EnemyState* state){
    if (current_state == state) return;
    
    if (current_state != nullptr) {
        current_state->Exit();
    }

    current_state = state;
    if (current_state != nullptr) {
        current_state->Enter();
    }
}

EnemyState* Enemy::GetCurrentState(){
    return current_state;
}

void Enemy::TakeDamage() {
    if (damageCooldownTimer > 0.0f) {
        return;
    }

    hp -= 1.0f;

    damageCooldownTimer = damageCooldownDuration;

    if (hp <= 0.0f) {
        alive = false;
        hp = 0.0f;
    }
}

void EnemyWandering::Enter(){
    enemy->color = enemy->baseColor;
    // Chooses a random initial direction for the enemy to face
    // UGH MATH I HAD TO RECONSULT MY CALKILLUS
    // JIC so basically because cos and sin uses radians i sbeve
    // 0-359 for the degree, then to make it radians, multiply it by pi over 180
    float angle = GetRandomValue(0, 359) * (PI / 180.0f);
    // cos gives the x vector axis
    // sin gives the y vector axis
    // courtesy of Jozen HDSAHDSAHDSAH who is smarter in calkillus than mwah
    enemy->velocity = { cosf(angle), sinf(angle) };

    //to make movement consistent 
    enemy->velocity = Vector2Normalize(enemy->velocity);
}

void EnemyChasing::Enter(){
    enemy->color = YELLOW;
}

void EnemyReadyingAttack::Enter(){
    enemy->color = ORANGE;
    enemy->velocity = {0, 0};
    enemy->readyTimer = 0.6f;
}

void EnemyAttacking::Enter(){
    enemy->color = RED;
    enemy->dashTimer= enemy->dashDuration;
    enemy->rotation = atan2f(enemy->dashDirection.y, enemy->dashDirection.x);
}

void EnemyWandering::Exit(){}

void EnemyChasing::Exit(){}

void EnemyReadyingAttack::Exit(){}

void EnemyAttacking::Exit(){}



void EnemyWandering::Update(float delta_time){
    // Move le enemie
    Vector2 newPosition = Vector2Add(
        enemy->position,
        Vector2Scale(enemy->velocity, enemy->speed * delta_time)
    );

    if (!CheckTileCollisionRoom(newPosition, enemy->size, enemy->current_room)) {
        enemy->position = newPosition;
    }

    // Enemy moves in random directions limitedly, 2% chance (lemme know if it should be higher...?)
    if (GetRandomValue(0, 100) < 2) {
        float angle = GetRandomValue(0, 359) * (PI / 180.0f);
        enemy->velocity = Vector2Normalize({ cosf(angle), sinf(angle) });
    }

    if (Vector2Length(enemy->velocity) > 0.001f) {
        Vector2 moveDir = Vector2Normalize(enemy->velocity);
        enemy->rotation = atan2f(moveDir.y, moveDir.x);
    }

    //If Player enters the enemy’s detection radius, the enemy transitions to the Chasing state.
    if (enemy->playerRef != nullptr) {
        float detectionDistance = Vector2Distance(enemy->position, enemy->playerRef->position);
        if (detectionDistance < enemy->detectionRadius) {
            enemy->SetState(&enemy->chasing);
        }
    }
    
}

void EnemyChasing::Update(float delta_time){
    if (enemy->playerRef == nullptr) return;
    
    Vector2 playerDirection = Vector2Subtract(enemy->playerRef->position, enemy->position);
    float playerDistance = Vector2Length(playerDirection);
    
    //If Player leaves the Enemy’s aggro radius, the enemy transitions back to the Wandering state.
    if (playerDistance > enemy->aggroRadius) {
        enemy->SetState(&enemy->wandering);
    }

    //If Player enters the enemy’s attack radius, the enemy transitions to the Readying Attack state.
    if (playerDistance < enemy->attackRadius && enemy->attackDuration <= 0.0f) {
        enemy->SetState(&enemy->readyingAttack);
        return;
    }
    
    //The enemy chases the player, rotating its body towards the Player’s direction
    playerDirection = Vector2Normalize(playerDirection);
    enemy->rotation = atan2f(playerDirection.y, playerDirection.x);
    Vector2 newPosition = Vector2Add(enemy->position, Vector2Scale(playerDirection, enemy->speed * delta_time));

    if (!CheckTileCollisionRoom(newPosition, enemy->size, enemy->current_room)) {
        enemy->position = newPosition;
    }
}

void EnemyReadyingAttack::Update(float delta_time){
    enemy->readyTimer -= delta_time;

    Vector2 dirToPlayer = Vector2Subtract(enemy->playerRef->position, enemy->position);
    dirToPlayer = Vector2Normalize(dirToPlayer);

    enemy->rotation = atan2f(dirToPlayer.y, dirToPlayer.x);
    
    if (enemy->readyTimer <= 0.0f) {
        Vector2 lockedDir = Vector2Subtract (enemy->playerRef->position, enemy->position);
        enemy->dashDirection = Vector2Normalize(lockedDir);
        enemy->SetState(&enemy->attacking);
    }
}

void EnemyAttacking::Update(float delta_time){
    enemy->dashTimer -= delta_time;
    Vector2 newPosition = Vector2Add(enemy->position, Vector2Scale(enemy->dashDirection, enemy->speed * 4.5 * delta_time));
    if (!CheckTileCollisionRoom(newPosition, enemy->size, enemy->current_room)) {
        enemy->position = newPosition;
    }
    if (enemy->dashTimer <= 0.0f)
        {
            enemy->attackTimer = enemy->attackDuration;
            enemy->SetState(&enemy->wandering);
        }
}

void Enemy::HandlePlayerCollision() {
    // ill add damage latur
}

Shadow::Shadow(Vector2 pos, float size, float speed) : Enemy(pos, size, speed) {
    //ill set custom stats latur
    hp = 1.0f;
    baseColor = DARKBROWN; 
    color = baseColor; 
}

void Shadow::HandlePlayerCollision() {
    // obscures view when you touch them. upon touching, they disappear too. they do not hurt the player
    if (alive) {
            playerRef->obscureTimer = 2.0f; 
            alive = false; 
            std::cout << "Shadow touched! Vision obscured." << std::endl;
        }
}


Spirit::Spirit(Vector2 pos, float size, float speed) : Enemy(pos, size, speed) {
    hp = 2.0f;
    baseColor = DARKGRAY; 
    color = baseColor;
}

void Spirit::HandlePlayerCollision() {
    //slows the player down temporarily (for maybe 3 seconds) but doesnt hurt the player
    if (playerRef->slowTimer <= 0) {
            playerRef->slowTimer = 3.0f; 
            std::cout << "Spirit touched! Player slowed." << std::endl;
        }

}

Poltergeist::Poltergeist(Vector2 pos, float size, float speed) : Enemy(pos, size, speed) {
    hp = 3.0f;
    baseColor = PURPLE;
    color = baseColor;
}

void Poltergeist::HandlePlayerCollision() {

    playerRef->TakeDamage(1.0f);

}