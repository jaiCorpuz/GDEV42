#ifndef ENEMY
#define ENEMY

#include <raylib.h>
#include <raymath.h>
#include "Player.hpp"

class Enemy;

class EnemyState {
public:
    Enemy* enemy;

    virtual ~EnemyState() {}
    virtual void Enter() = 0;
    virtual void Update(float delta_time) = 0;
    virtual void Exit() = 0;
};

//The different states
class EnemyWandering : public EnemyState {
public:
    void Enter();
    void Update(float delta_time);
    void Exit();
};

class EnemyChasing : public EnemyState {
public:
    void Enter();
    void Update(float delta_time);
    void Exit();
};

class EnemyReadyingAttack : public EnemyState {
public:
    void Enter();
    void Update(float delta_time);
    void Exit();
};

class EnemyAttacking : public EnemyState {
public:
    void Enter();
    void Update(float delta_time);
    void Exit();
};

//The enemy
class Enemy {
    EnemyState* current_state = nullptr;
public:
    Vector2 position;
    float size;
    Color color;
    float hp;
    bool alive;

    Vector2 velocity;
    Vector2 acceleration;
    float speed;
    
    float damageCooldownTimer;
    float damageCooldownDuration;
    float attackTimer = 0.0f;
    float attackDuration = 1.0f;

    float rotation = 0.0f;

    Vector2 dashDirection;
    Vector2 lockedTargetPosition;

    float dashTimer = 0.0f;
    float dashDuration = 0.3f;
    float readyTimer;

    //the detection radii's
    float aggroRadius; //first 
    float detectionRadius; //second middle
    float attackRadius; //innermost

    vector<vector<int>> grid;
    vector<TileType> tileTypes;
    float tileScale;
    int gridRows;
    int gridColumns;

    EnemyWandering wandering = EnemyWandering();
    EnemyChasing chasing = EnemyChasing();
    EnemyReadyingAttack readyingAttack = EnemyReadyingAttack();
    EnemyAttacking attacking = EnemyAttacking();

    Player* playerRef = nullptr;

    Enemy(Vector2 pos, float size, float speed);

    void Update(float delta_time);

    void Draw();

    void SetState(EnemyState* state);

    EnemyState* GetCurrentState();

    void TakeDamage();
};


#endif