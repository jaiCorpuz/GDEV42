#ifndef PLAYER
#define PLAYER

#include <raylib.h>
#include <raymath.h>
#include <vector>

struct Room;

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
    int level = 0;
    bool level_up = false;
    int level_rooms[3] = {5, 8, 11};
    // int level_rooms[3] = {5, 5, 5};
    bool win = false;

    Vector2 position;
    float radius;
    Color color;
    float hp = 5.0f;
    float max_hp = 5.0f;    //Does not change
    Camera2D* camera;
    bool key_collected = false;
    Room* current_room;
    Vector2 tongueEndPoint;

    Vector2 velocity;
    Vector2 acceleration;
    float speed;
    float speedMultiplier = 1.0f;
    float slowTimer = 0.0f;
    float obscureTimer = 0.0f;

    float attackTimer;
    float attackDuration;
    float baseAttackDuration = 0.5f;
    float nipTimer;

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

    float tileScale;
    int tileSize;

    Player(Vector2 pos, float rad, float spd);

    void Update(float delta_time);

    void Draw();

    void SetState(PlayerState* state);

    void ResetPosition();

    void TakeDamage(float amount);

    void Heal(float amount);

    void BoostAttack(float extraTime, float effectDuration);

    PlayerState* GetCurrentState();
};


#endif