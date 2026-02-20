/*
|------------------------------------------------------|
|                    PLAYER HEADER                     |              
|------------------------------------------------------|
|   This is the header file (.hpp) file that contains  |
|   declarations for the Player class along with the   | 
|   PlayerState class and its subclasses.              |
|                                                      |
|   Note that ONLY DECLARATIONS are made here. The     |
|   DEFINITION of all member variables and functions   |
|   are done in the PlayerStateMachine.cpp file        |
|                                                      |
|   When adding new classes to this file               |
|   (i.e. PlayerBlocking, PlayerAttacking, etc.),      |
|   Only have the class declarations and define them   |
|   later in the PlayerStateMachine.cpp file           |
|                                                      |
|------------------------------------------------------|
*/

#ifndef PLAYER
#define PLAYER

#include <raylib.h>
#include <raymath.h>

class Player;

class PlayerState {
public:
    Player* player;

    virtual ~PlayerState() {}
    virtual void Enter() = 0;
    virtual void Update(float delta_time) = 0;
    virtual void Exit() = 0;

    virtual float GetDamageMult() {return 1.0f;}
};


//The different states
class PlayerIdle : public PlayerState {
public:
    void Enter() override;
    void Update(float delta_time) override;
    void Exit() override;
};

class PlayerMoving : public PlayerState {
public:
    void Enter() override;
    void Update(float delta_time) override;
    void Exit() override;
};

class PlayerAttacking : public PlayerState {
public:
    void Enter() override;
    void Update(float delta_time) override;
    void Exit() override;
};

class PlayerBlocking : public PlayerState {
public:
    void Enter() override;
    void Update(float delta_time) override;
    void Exit() override;
    float GetDamageMult() override;
};

class PlayerDodging : public PlayerState {
public:
    void Enter() override;
    void Update(float delta_time) override;
    void Exit() override;
    float GetDamageMult() override;
};

//The player
class Player {
    PlayerState* current_state = nullptr;
public:
    Vector2 position;
    float radius;
    Color color;
    float hp = 5.0f;

    Vector2 velocity;
    Vector2 acceleration;
    float speed;

    float attackTimer;
    float attackDuration;

    float dodgeTimer;
    float dodgeDuration;
    Vector2 dodgeDirection;

    float invincibleTimer;
    float invincibleDuration;
    bool isInvincible = false;

    PlayerIdle idle;
    PlayerMoving moving;
    PlayerAttacking attacking;
    PlayerDodging dodging;
    PlayerBlocking blocking;

    Player(Vector2 pos, float rad, float spd);

    void Update(float delta_time);

    void Draw();

    void SetState(PlayerState* state);

    void TakeDamage(float amount);

    PlayerState* GetCurrentState();
};


#endif