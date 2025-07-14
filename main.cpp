#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
class Animation {
public:
    int startFrame;
    int frameCount;
    int frameWidth;
    float frameTime;
    float timer;
    int currentFrame;

    Animation(int start, int count, int width, float timePerFrame):
        startFrame(start), frameCount(count), frameWidth(width),
        frameTime(timePerFrame), timer(0.0f), currentFrame(0){ }

    void update(float deltaTime) {
        timer += deltaTime;
        if (timer >= frameTime) {
            timer = 0.0f;
            currentFrame = (currentFrame + 1) % frameCount;
        }
    }

    SDL_Rect getFrameRect(int y = 0, int frameHeight = 64) const {
        return SDL_Rect{
            (startFrame + currentFrame) * frameWidth,
            y,
            frameWidth,
            frameHeight
        };
    }
};
class Collider {
public:
    static bool AABB(const SDL_Rect& a, const SDL_Rect& b) {
        return(a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h);
    }
};
class Entity {
private:
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    float acceleration = 500.0f;
    float friction = 800.0f;
    float maxSpeed = 250.0f;
    int inputX = 0;
    int inputY = 0;
protected:
    SDL_Rect position;
    SDL_Texture* texture;
    Animation* animation;

public:
    Entity(int x, int y, int w, int h, SDL_Texture* tex, Animation* anim = nullptr)
        : texture(tex), animation(anim) {
        position = { x, y, w, h };
    }
    float getVelocityX() const {
        return velocityX;
    }
    float getVelocityY() const {
        return velocityY;
    }
    void setVelocityX(float vx) {
        velocityX = vx;
    }
    void setVelocityY(float vy) {
        velocityY = vy;
    }
    virtual void update(float deltaTime) {
        if (animation) {
            animation->update(deltaTime);
        }
        velocityX += inputX * acceleration * deltaTime;
        velocityY += inputY * acceleration * deltaTime;
        if (inputX == 0) {
            if (velocityX > 0) {
                velocityX -= friction * deltaTime;
                if (velocityX < 0) velocityX = 0;
            }else if (velocityX < 0) {
                velocityX += friction * deltaTime;
                if (velocityX > 0) velocityX = 0;
            }
        }
        if (inputY == 0) {
            if (velocityY > 0) {
                velocityY -= friction * deltaTime;
                if (velocityY < 0) velocityY = 0;
            }
            else if (velocityY < 0) {
                velocityY += friction * deltaTime;
                if (velocityY > 0) velocityY = 0;
            }
        }
        if (velocityX > maxSpeed) velocityX = maxSpeed;
        if (velocityX < -maxSpeed) velocityX = -maxSpeed;
        if (velocityY > maxSpeed) velocityY = maxSpeed;
        if (velocityY < -maxSpeed) velocityY = -maxSpeed;
        //move(static_cast<int>(velocityX * deltaTime), static_cast<int>(velocityY * deltaTime));
        inputX = 0;
        inputY = 0;
    }
    virtual void render(SDL_Renderer* renderer) {
        if (texture && animation) {
            SDL_Rect srcRect = animation->getFrameRect();
            SDL_RenderCopy(renderer, texture, &srcRect, &position);
        }
        else if (texture) {
            SDL_RenderCopy(renderer, texture, nullptr, &position);
        }
    }
    SDL_Rect getBounds() const {
        return position;
    }
    bool collidesWith(const Entity* other) const {
        return Collider::AABB(this->getBounds(), other->getBounds());
    }
    void move(int dx, int dy) {
        position.x += dx;
        position.y += dy;
    }
    void setInput(int x, int y) {
        inputX = x;
        inputY = y;
    }
    void setAnimation(Animation* anim) {
        if (animation != anim) {
            animation = anim;
            animation->currentFrame = 0;
            animation->timer = 0.0f;
        }
    }
    virtual ~Entity() {}
};

bool Init(SDL_Window** window, SDL_Renderer** renderer, int w, int h){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;
    *window = SDL_CreateWindow("Etapa 7", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    return *window && *renderer;
}

//bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b) {
//    return(a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h);
//}

SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << path << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!Init(&window, &renderer, 800, 600)) return 1;

    SDL_Texture* spriteSheet = LoadTexture("assets/spritesheet.png", renderer);
    if (!spriteSheet) return 1;

    Animation* idleAnim = new Animation(0, 1, 64, 0.2f);
    Animation* walkAnim = new Animation(1, 4, 64, 0.15f);
    Entity* player = new Entity(100, 100, 64, 64, spriteSheet, walkAnim);
    //Entity* obstacle = new Entity(300, 200, 64, 64, spriteSheet, idleAnim);
    std::vector<Entity*> obstacles;
    obstacles.push_back(new Entity(300, 200, 64, 64, spriteSheet, idleAnim));
    obstacles.push_back(new Entity(400, 300, 64, 64, spriteSheet, idleAnim));
    obstacles.push_back(new Entity(200, 400, 64, 64, spriteSheet, idleAnim));
    Uint32 lastTick = SDL_GetTicks();

    bool running = true;
    SDL_Event event;
    while (running) {
        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        int dx = 0, dy = 0, speed = 200;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }
        Uint32 currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        //if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) dx -= static_cast<int>(speed * deltaTime);
        //if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) dx += static_cast<int>(speed * deltaTime);
        //if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) dy -= static_cast<int>(speed * deltaTime);
        //if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) dy += static_cast<int>(speed * deltaTime);
        //player->move(dx, dy);
        int inputX = 0, inputY = 0;
        if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) inputX = -1;
        if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) inputX = 1;
        if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) inputY = -1;
        if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) inputY = 1;
        player->setInput(inputX, inputY);
        //SDL_Rect previousPos = player->getBounds();
        //player->update(deltaTime);
        //if (CheckCollision(player->getBounds(), obstacle->getBounds())) {
            //player->move(-dx, -dy);
        //if(Collider::AABB(player->getBounds(), obstacle->getBounds())){
        //if(player->collidesWith(obstacle)){
        //    player->move(previousPos.x - player->getBounds().x, previousPos.y - player->getBounds().y);
        //}
        player->update(deltaTime);
        SDL_Rect originalPos = player->getBounds();
        player->move(static_cast<int>(player->getVelocityX()*deltaTime), 0);
        //if (player->collidesWith(obstacle)) {
        for(Entity* obstacle: obstacles){
            if (player->collidesWith(obstacle)) {
                player->move(originalPos.x - player->getBounds().x, 0);
                player->setVelocityX(0);
            }
        }
        player->move(0, static_cast<int>(player->getVelocityY() * deltaTime));
        //if (player->collidesWith(obstacle)) {
        for (Entity* obstacle : obstacles) {
            if (player->collidesWith(obstacle)) {
                player->move(0, originalPos.y - player->getBounds().y);
                player->setVelocityY(0);
            }
        }
        //player->update(deltaTime);
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        //obstacle->render(renderer);
        for (Entity* obstacle : obstacles) {
            obstacle->render(renderer);
        }
        player->render(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyTexture(spriteSheet);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    delete player;
    //delete obstacle;
    for (Entity* obstacle : obstacles) {
        delete obstacle;
    }
    delete walkAnim;
    delete idleAnim;
    return 0;
}