#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "Enemy.hpp"
#include "Player.hpp"

void Enemy::Update(float delta_time) {
    if (damageCooldownTimer > 0) {
        damageCooldownTimer -= delta_time;
    } else {
        damageCooldownTimer = 0;
    }

    current_state->Update(delta_time);

    // If Player collides with the enemy, they get damaged
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
        playerRef->TakeDamage(1.0f);
    }

    if (CheckCollisionCircleRec(
        playerRef->position,
        playerRef->radius + 25, // HARD CODED
        {
            position.x,
            position.y,
            size,
            size
        }) && 
        (dynamic_cast<PlayerAttacking*>(playerRef->GetCurrentState()))
    ) {
        TakeDamage();
    }
}

void Enemy::Draw() {
    DrawRectangle(position.x, position.y, size, size, color);
    DrawCircleLines(position.x + size/2, position.y + size/2, detectionRadius, LIGHTGRAY);
    DrawCircleLines(position.x + size/2, position.y + size/2, aggroRadius, ORANGE);
    DrawCircleLines(position.x + size/2, position.y + size/2, attackRadius, RED);
    DrawCircle(position.x + size/2, position.y + size/2, size/4, (hp <= 0 ? BLACK : RED));
}

Enemy::Enemy(Vector2 pos, float siz, float spd){
    position = pos;
    size = siz;
    speed = spd;
    hp = 2.0f;

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
    if (current_state != nullptr) {
        current_state->Exit();
    }

    current_state = state;
    current_state->Enter();
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

    if (hp < 0.0f) {
        hp = 0.0f;
    }
}

void EnemyWandering::Enter(){
    enemy->color = BLUE;
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
    enemy->lockedTargetPosition = enemy->playerRef->position;
    enemy->readyTimer = 0.6f;
}

void EnemyAttacking::Enter(){
    enemy->color = RED;
    
    Vector2 direction = Vector2Subtract(enemy->lockedTargetPosition, enemy->position);

    direction = Vector2Normalize(direction);
    
    enemy->dashTimer = enemy->dashDuration;
}

void EnemyWandering::Exit(){}

void EnemyChasing::Exit(){}

void EnemyReadyingAttack::Exit(){}

void EnemyAttacking::Exit(){}

void EnemyWandering::Update(float delta_time){

    // Move le enemie
    enemy->position = Vector2Add(
        enemy->position,
        Vector2Scale(enemy->velocity, enemy->speed * delta_time)
    );

    // Enemy doesnt go beyond the window space
    if (enemy->position.x < 0 || enemy->position.x > 1280){
        enemy->velocity.x *= -1;
    } 
    if (enemy->position.y < 0 || enemy->position.y > 720){
        enemy->velocity.y *= -1;
    } 
    
    // Enemy moves in random directions limitedly, 2% chance (lemme know if it should be higher...?)
    if (GetRandomValue(0, 100) < 2) {
        float angle = GetRandomValue(0, 359) * (PI / 180.0f);
        enemy->velocity = Vector2Normalize({ cosf(angle), sinf(angle) });
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

    //The enemy chases the player, rotating its body towards the Player’s direction
    playerDirection = Vector2Normalize(playerDirection);
    enemy->position = Vector2Add(enemy->position, Vector2Scale(playerDirection, enemy->speed * delta_time));

    //If Player leaves the Enemy’s aggro radius, the enemy transitions back to the Wandering state.
    if (playerDistance > enemy->aggroRadius) {
        enemy->SetState(&enemy->wandering);
    }

    //If Player enters the enemy’s attack radius, the enemy transitions to the Readying Attack state.
    if (playerDistance < enemy->attackRadius) {
        enemy->SetState(&enemy->readyingAttack);
    }
}

void EnemyReadyingAttack::Update(float delta_time){
    enemy->readyTimer -= delta_time;
    
    if (enemy->readyTimer <= 0.0f) {
        enemy->SetState(&enemy->attacking);
    }
}

void EnemyAttacking::Update(float delta_time){
    enemy->dashTimer -= delta_time;
    
    enemy->position = Vector2Add(enemy->position, Vector2Scale(enemy->dashDirection, enemy->dashSpeed * delta_time));

    if (enemy->dashTimer <= 0.0f) {
        enemy->SetState(&enemy->wandering);
    }
}