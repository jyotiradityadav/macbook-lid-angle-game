#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <string>
#include "Sensor.h"

class LidPongGame {
private:
    GLFWwindow* window;
    LidPong::LidSensor sensor;
    
    // Game objects
    struct Ball {
        float x, y;
        float vx, vy;
        float radius;
        bool active;
        
        Ball() : x(0.0f), y(0.0f), vx(0.8f), vy(0.6f), radius(0.02f), active(true) {}
        
        void update(float deltaTime, float speedMultiplier) {
            if (!active) return;
            
            x += vx * deltaTime * speedMultiplier;
            y += vy * deltaTime * speedMultiplier;
            
            // Bounce off top/bottom walls
            if (y + radius > 0.95f || y - radius < -0.95f) {
                vy = -vy;
                if (y + radius > 0.95f) y = 0.95f - radius;
                if (y - radius < -0.95f) y = -0.95f + radius;
            }
            
            // Bounce off right wall
            if (x + radius > 0.98f) {
                vx = -vx;
                x = 0.98f - radius;
            }
            
            // Ball missed - goes off left side
            if (x + radius < -1.0f) {
                active = false; // Don't auto-reset, let game handle it
            }
        }
        
        void reset() {
            x = 0.0f;
            y = 0.0f;
            vx = 0.8f * (rand() % 2 == 0 ? 1.0f : -1.0f);
            vy = 0.6f * (rand() % 2 == 0 ? 1.0f : -1.0f);
            active = true;
        }
        
        void draw() {
            if (!active) return;
            
            glColor3f(1.0f, 1.0f, 1.0f); // White ball
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y);
            for (int i = 0; i <= 20; i++) {
                float angle = 2.0f * M_PI * i / 20;
                glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
            }
            glEnd();
        }
    };
    
    struct Slider {
        float x, y;
        float width, height;
        float targetY;
        float speed;
        
        // VERY SENSITIVE and LONGER slider
        Slider() : x(-0.95f), y(0.0f), width(0.02f), height(0.6f), targetY(0.0f), speed(12.0f) {}
        
        void update(float deltaTime, double lidPosition) {
            // VERY HIGH SENSITIVITY: small lid movements = big slider movements
            float normalizedPos = lidPosition - 0.5f; // Center around 0
            float superSensitive = normalizedPos * 4.0f; // 4x sensitivity!
            targetY = superSensitive * 0.85f; // Map to screen coordinates
            
            // Clamp to screen bounds
            if (targetY > 0.85f) targetY = 0.85f;
            if (targetY < -0.85f) targetY = -0.85f;
            
            // Very fast movement towards target
            float diff = targetY - y;
            y += diff * speed * deltaTime;
        }
        
        void draw() {
            glColor3f(0.8f, 0.8f, 0.8f); // Light gray slider
            glBegin(GL_QUADS);
            glVertex2f(x - width/2, y - height/2);
            glVertex2f(x + width/2, y - height/2);
            glVertex2f(x + width/2, y + height/2);
            glVertex2f(x - width/2, y + height/2);
            glEnd();
        }
        
        bool checkCollision(Ball& ball) {
            return (ball.x - ball.radius <= x + width/2 && 
                    ball.x + ball.radius >= x - width/2 &&
                    ball.y - ball.radius <= y + height/2 && 
                    ball.y + ball.radius >= y - height/2);
        }
    };
    
    Ball ball;
    Slider slider;
    int score;
    int lives;
    int totalHits;
    float ballSpeedMultiplier;
    double currentLidAngle;
    bool gameOver;
    bool showGameOverModal;
    
public:
    LidPongGame() : window(nullptr), score(0), lives(3), totalHits(0), ballSpeedMultiplier(0.6f), currentLidAngle(0.0), gameOver(false), showGameOverModal(false) {}
    
    bool init() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        // Create window
        window = glfwCreateWindow(800, 600, "Clean Lid Pong - C++ Only", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create window" << std::endl;
            glfwTerminate();
            return false;
        }
        
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        
        // Check sensor availability
        if (!sensor.isAvailable()) {
            std::cerr << "Warning: Lid sensor not available, using keyboard controls" << std::endl;
        }
        
        std::cout << "Lid Pong - MacBook Lid Angle Game" << std::endl;
        std::cout << "==================================" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  MacBook Lid: Move slider up/down" << std::endl;
        std::cout << "  Mouse: Drag speed slider" << std::endl;
        std::cout << "  +/- keys: Adjust ball speed" << std::endl;
        std::cout << "  SPACE: Reset ball / Restart game" << std::endl;
        std::cout << "  ESC: Quit" << std::endl;
        std::cout << std::endl;
        
        return true;
    }
    
    void run() {
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (!glfwWindowShouldClose(window)) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // Handle input
            glfwPollEvents();
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }
            static bool spacePressed = false;
            bool spaceKey = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
            if (spaceKey && !spacePressed) {
                if (gameOver) {
                    // Restart game
                    score = 0;
                    lives = 3;
                    totalHits = 0;
                    gameOver = false;
                    showGameOverModal = false;
                    ball.reset();
                } else if (!ball.active) {
                    // Reset ball if it's inactive
                    ball.reset();
                }
            }
            spacePressed = spaceKey;
            
            // Ball speed controls with mouse/keyboard
            handleSpeedSliderInput();
            
            // Update game
            update(deltaTime);
            
            // Render
            render();
            
            glfwSwapBuffers(window);
        }
    }
    
    void update(float deltaTime) {
        // Update sensor and get current angle
        sensor.update();
        currentLidAngle = sensor.getCurrentAngle();
        
        // Update slider with lid sensor
        double lidPosition = 0.5; // Default center
        try {
            if (sensor.isAvailable()) {
                lidPosition = sensor.getSliderPosition();
            } else {
                // Use keyboard fallback
                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                    lidPosition = 0.8;
                } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                    lidPosition = 0.2;
                }
            }
        } catch (...) {
            // Fallback to keyboard
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                lidPosition = 0.8;
            } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                lidPosition = 0.2;
            }
        }
        
        if (!gameOver) {
            slider.update(deltaTime, lidPosition);
            ball.update(deltaTime, ballSpeedMultiplier);
            
            // Check collision with slider
            if (ball.active && slider.checkCollision(ball)) {
                // Only count if ball was moving towards slider (prevent multiple hits)
                if (ball.vx < 0) {
                    ball.vx = std::abs(ball.vx); // Bounce right
                    
                    // Add spin based on where ball hits slider
                    float hitPos = (ball.y - slider.y) / (slider.height / 2);
                    ball.vy += hitPos * 1.5f; // Reduced spin for smoother gameplay
                    
                    // Clamp velocity for smoother gameplay
                    if (ball.vy > 1.2f) ball.vy = 1.2f;
                    if (ball.vy < -1.2f) ball.vy = -1.2f;
                    
                    totalHits++;
                    score = totalHits; // Score is number of hits
                }
            }
            
            // Check if ball was missed
            if (!ball.active && lives > 0) {
                lives--;
                if (lives <= 0) {
                    gameOver = true;
                    showGameOverModal = true;
                } else {
                    // Auto-reset ball after a short delay
                    ball.reset();
                }
            }
        }
        
        // Console output with live data
        if (gameOver) {
            std::cout << "\rGAME OVER! Final Score: " << score << " hits | Lives: " << lives 
                      << " | Press SPACE to restart | ESC to quit    " << std::flush;
        } else {
            std::cout << "\rHits: " << score << " | Lives: " << lives
                      << " | Lid: " << std::fixed << std::setprecision(1) << currentLidAngle << " degrees"
                      << " | Speed: " << std::setprecision(1) << ballSpeedMultiplier << "x"
                      << " | Ball: (" << std::setprecision(2) << ball.x << "," << ball.y << ")"
                      << "    " << std::flush;
        }
    }
    
    void render() {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
        
        // Draw THIN walls
        glColor3f(0.5f, 0.5f, 0.5f); // Gray walls
        
        // Top wall (very thin)
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, 0.95f);
        glVertex2f(1.0f, 0.95f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();
        
        // Bottom wall (very thin)
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, -0.95f);
        glVertex2f(-1.0f, -0.95f);
        glEnd();
        
        // Right wall (very thin)
        glBegin(GL_QUADS);
        glVertex2f(0.98f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(0.98f, 1.0f);
        glEnd();
        
        // Draw game objects
        slider.draw();
        ball.draw();
        
        // Draw simple HUD indicators
        drawHUD();
        
        // Draw game over modal
        if (showGameOverModal) {
            drawGameOverModal();
        }
    }
    
    void drawHUD() {
        // Draw lives as simple squares (no text)
        glColor3f(1.0f, 0.2f, 0.2f);
        for (int i = 0; i < lives; i++) {
            float x = -0.9f + i * 0.08f;
            glBegin(GL_QUADS);
            glVertex2f(x - 0.02f, 0.82f);
            glVertex2f(x + 0.02f, 0.82f);
            glVertex2f(x + 0.02f, 0.86f);
            glVertex2f(x - 0.02f, 0.86f);
            glEnd();
        }
        
        // Draw score as simple number
        drawSimpleNumber(score, 0.0f, 0.84f, 0.04f);
        
        // Draw speed slider (interactive)
        drawSpeedSlider();
        
        // Lid angle indicator (vertical bar on right)
        if (sensor.isAvailable()) {
            glColor3f(0.0f, 1.0f, 0.0f); // Green if sensor working
            float angleNormalized = (currentLidAngle - 30.0) / 120.0; // Normalize 30-150 degrees
            if (angleNormalized < 0) angleNormalized = 0;
            if (angleNormalized > 1) angleNormalized = 1;
            float barHeight = angleNormalized * 1.6f - 0.8f;
            
            glBegin(GL_QUADS);
            glVertex2f(0.85f, -0.8f);
            glVertex2f(0.9f, -0.8f);
            glVertex2f(0.9f, barHeight);
            glVertex2f(0.85f, barHeight);
            glEnd();
        } else {
            glColor3f(1.0f, 0.0f, 0.0f); // Red if sensor not working
        }
        
        // Lid angle bar outline
        glBegin(GL_LINE_LOOP);
        glVertex2f(0.85f, -0.8f);
        glVertex2f(0.9f, -0.8f);
        glVertex2f(0.9f, 0.8f);
        glVertex2f(0.85f, 0.8f);
        glEnd();
    }
    
    void drawGameOverModal() {
        // Semi-transparent overlay
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();
        
        // Modal box
        glColor3f(0.2f, 0.2f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(-0.6f, -0.4f);
        glVertex2f(0.6f, -0.4f);
        glVertex2f(0.6f, 0.4f);
        glVertex2f(-0.6f, 0.4f);
        glEnd();
        
        // Modal border
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-0.6f, -0.4f);
        glVertex2f(0.6f, -0.4f);
        glVertex2f(0.6f, 0.4f);
        glVertex2f(-0.6f, 0.4f);
        glEnd();
        
        // Show final score as number
        glColor3f(1.0f, 1.0f, 1.0f);
        drawSimpleNumber(score, 0.0f, 0.0f, 0.08f);
        
        // Simple indicator that game is over (red X)
        glColor3f(1.0f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        // First diagonal
        glVertex2f(-0.1f, 0.25f); glVertex2f(-0.05f, 0.3f); glVertex2f(0.1f, 0.1f); glVertex2f(0.05f, 0.05f);
        // Second diagonal  
        glVertex2f(0.05f, 0.3f); glVertex2f(0.1f, 0.25f); glVertex2f(-0.05f, 0.05f); glVertex2f(-0.1f, 0.1f);
        glEnd();
        
        // "Press space to continue" text
        glColor3f(0.7f, 0.7f, 0.7f);
        drawSimpleText("PRESS SPACE TO CONTINUE", 0.0f, -0.25f, 0.025f);
        
        glDisable(GL_BLEND);
    }
    
    void drawSimpleNumber(int number, float x, float y, float size) {
        if (number == 0) {
            drawSimpleDigit(0, x, y, size);
            return;
        }
        
        // Convert to string to get digits
        std::string numStr = std::to_string(number);
        float digitWidth = size * 0.8f;
        float startX = x - (numStr.length() - 1) * digitWidth * 0.5f;
        
        for (size_t i = 0; i < numStr.length(); i++) {
            int digit = numStr[i] - '0';
            drawSimpleDigit(digit, startX + i * digitWidth, y, size);
        }
    }
    
    void drawSimpleDigit(int digit, float x, float y, float size) {
        float w = size * 0.3f;
        float h = size * 0.5f;
        float thick = size * 0.08f;
        
        glColor3f(1.0f, 1.0f, 1.0f);
        
        // Very simple 7-segment display using rectangles
        bool segs[10][7] = {
            {1,1,1,1,1,1,0}, // 0
            {0,1,1,0,0,0,0}, // 1  
            {1,1,0,1,1,0,1}, // 2
            {1,1,1,1,0,0,1}, // 3
            {0,1,1,0,0,1,1}, // 4
            {1,0,1,1,0,1,1}, // 5
            {1,0,1,1,1,1,1}, // 6
            {1,1,1,0,0,0,0}, // 7
            {1,1,1,1,1,1,1}, // 8
            {1,1,1,1,0,1,1}  // 9
        };
        
        if (digit < 0 || digit > 9) return;
        
        glBegin(GL_QUADS);
        
        // Top horizontal (segment 0)
        if (segs[digit][0]) {
            glVertex2f(x-w+thick, y+h-thick);
            glVertex2f(x+w-thick, y+h-thick);
            glVertex2f(x+w-thick, y+h);
            glVertex2f(x-w+thick, y+h);
        }
        
        // Top right vertical (segment 1)
        if (segs[digit][1]) {
            glVertex2f(x+w-thick, y);
            glVertex2f(x+w, y);
            glVertex2f(x+w, y+h-thick);
            glVertex2f(x+w-thick, y+h-thick);
        }
        
        // Bottom right vertical (segment 2)
        if (segs[digit][2]) {
            glVertex2f(x+w-thick, y-h+thick);
            glVertex2f(x+w, y-h+thick);
            glVertex2f(x+w, y);
            glVertex2f(x+w-thick, y);
        }
        
        // Bottom horizontal (segment 3)
        if (segs[digit][3]) {
            glVertex2f(x-w+thick, y-h);
            glVertex2f(x+w-thick, y-h);
            glVertex2f(x+w-thick, y-h+thick);
            glVertex2f(x-w+thick, y-h+thick);
        }
        
        // Bottom left vertical (segment 4)
        if (segs[digit][4]) {
            glVertex2f(x-w, y-h+thick);
            glVertex2f(x-w+thick, y-h+thick);
            glVertex2f(x-w+thick, y);
            glVertex2f(x-w, y);
        }
        
        // Top left vertical (segment 5)
        if (segs[digit][5]) {
            glVertex2f(x-w, y);
            glVertex2f(x-w+thick, y);
            glVertex2f(x-w+thick, y+h-thick);
            glVertex2f(x-w, y+h-thick);
        }
        
        // Middle horizontal (segment 6)
        if (segs[digit][6]) {
            glVertex2f(x-w+thick, y-thick/2);
            glVertex2f(x+w-thick, y-thick/2);
            glVertex2f(x+w-thick, y+thick/2);
            glVertex2f(x-w+thick, y+thick/2);
        }
        
        glEnd();
    }
    
    void drawSimpleText(const std::string& text, float x, float y, float size) {
        float charWidth = size * 0.8f;
        float startX = x - (text.length() - 1) * charWidth * 0.5f;
        
        for (size_t i = 0; i < text.length(); i++) {
            char c = text[i];
            drawSimpleChar(c, startX + i * charWidth, y, size);
        }
    }
    
    void drawSimpleChar(char c, float x, float y, float size) {
        float w = size * 0.3f;
        float h = size * 0.4f;
        float thick = size * 0.1f;
        
        glBegin(GL_QUADS);
        
        switch (c) {
            case 'A':
                // Left vertical
                glVertex2f(x-w, y-h); glVertex2f(x-w+thick, y-h); glVertex2f(x-w+thick, y+h); glVertex2f(x-w, y+h);
                // Right vertical
                glVertex2f(x+w-thick, y-h); glVertex2f(x+w, y-h); glVertex2f(x+w, y+h); glVertex2f(x+w-thick, y+h);
                // Top horizontal
                glVertex2f(x-w, y+h-thick); glVertex2f(x+w, y+h-thick); glVertex2f(x+w, y+h); glVertex2f(x-w, y+h);
                // Middle horizontal
                glVertex2f(x-w+thick, y-thick/2); glVertex2f(x+w-thick, y-thick/2); glVertex2f(x+w-thick, y+thick/2); glVertex2f(x-w+thick, y+thick/2);
                break;
                
            case 'C':
                // Left vertical
                glVertex2f(x-w, y-h); glVertex2f(x-w+thick, y-h); glVertex2f(x-w+thick, y+h); glVertex2f(x-w, y+h);
                // Top horizontal
                glVertex2f(x-w, y+h-thick); glVertex2f(x+w, y+h-thick); glVertex2f(x+w, y+h); glVertex2f(x-w, y+h);
                // Bottom horizontal
                glVertex2f(x-w, y-h); glVertex2f(x+w, y-h); glVertex2f(x+w, y-h+thick); glVertex2f(x-w, y-h+thick);
                break;
                
            case 'E':
                // Left vertical
                glVertex2f(x-w, y-h); glVertex2f(x-w+thick, y-h); glVertex2f(x-w+thick, y+h); glVertex2f(x-w, y+h);
                // Top horizontal
                glVertex2f(x-w, y+h-thick); glVertex2f(x+w, y+h-thick); glVertex2f(x+w, y+h); glVertex2f(x-w, y+h);
                // Middle horizontal
                glVertex2f(x-w+thick, y-thick/2); glVertex2f(x+w*0.7f, y-thick/2); glVertex2f(x+w*0.7f, y+thick/2); glVertex2f(x-w+thick, y+thick/2);
                // Bottom horizontal
                glVertex2f(x-w, y-h); glVertex2f(x+w, y-h); glVertex2f(x+w, y-h+thick); glVertex2f(x-w, y-h+thick);
                break;
                
            case 'I':
                // Top horizontal
                glVertex2f(x-w, y+h-thick); glVertex2f(x+w, y+h-thick); glVertex2f(x+w, y+h); glVertex2f(x-w, y+h);
                // Center vertical
                glVertex2f(x-thick/2, y-h); glVertex2f(x+thick/2, y-h); glVertex2f(x+thick/2, y+h); glVertex2f(x-thick/2, y+h);
                // Bottom horizontal
                glVertex2f(x-w, y-h); glVertex2f(x+w, y-h); glVertex2f(x+w, y-h+thick); glVertex2f(x-w, y-h+thick);
                break;
                
            case 'N':
                // Left vertical
                glVertex2f(x-w, y-h); glVertex2f(x-w+thick, y-h); glVertex2f(x-w+thick, y+h); glVertex2f(x-w, y+h);
                // Right vertical
                glVertex2f(x+w-thick, y-h); glVertex2f(x+w, y-h); glVertex2f(x+w, y+h); glVertex2f(x+w-thick, y+h);
                // Diagonal
                glVertex2f(x-w+thick, y+h-thick); glVertex2f(x, y); glVertex2f(x+thick/2, y); glVertex2f(x-w+thick*1.5f, y+h-thick);
                break;
                
            case 'O':
                // Left vertical
                glVertex2f(x-w, y-h+thick); glVertex2f(x-w+thick, y-h+thick); glVertex2f(x-w+thick, y+h-thick); glVertex2f(x-w, y+h-thick);
                // Right vertical
                glVertex2f(x+w-thick, y-h+thick); glVertex2f(x+w, y-h+thick); glVertex2f(x+w, y+h-thick); glVertex2f(x+w-thick, y+h-thick);
                // Top horizontal
                glVertex2f(x-w+thick, y+h-thick); glVertex2f(x+w-thick, y+h-thick); glVertex2f(x+w-thick, y+h); glVertex2f(x-w+thick, y+h);
                // Bottom horizontal
                glVertex2f(x-w+thick, y-h); glVertex2f(x+w-thick, y-h); glVertex2f(x+w-thick, y-h+thick); glVertex2f(x-w+thick, y-h+thick);
                break;
                
            case 'P':
                // Left vertical
                glVertex2f(x-w, y-h); glVertex2f(x-w+thick, y-h); glVertex2f(x-w+thick, y+h); glVertex2f(x-w, y+h);
                // Top horizontal
                glVertex2f(x-w, y+h-thick); glVertex2f(x+w, y+h-thick); glVertex2f(x+w, y+h); glVertex2f(x-w, y+h);
                // Right vertical (top half)
                glVertex2f(x+w-thick, y); glVertex2f(x+w, y); glVertex2f(x+w, y+h); glVertex2f(x+w-thick, y+h);
                // Middle horizontal
                glVertex2f(x-w+thick, y-thick/2); glVertex2f(x+w, y-thick/2); glVertex2f(x+w, y+thick/2); glVertex2f(x-w+thick, y+thick/2);
                break;
                
            case 'R':
                // Left vertical
                glVertex2f(x-w, y-h); glVertex2f(x-w+thick, y-h); glVertex2f(x-w+thick, y+h); glVertex2f(x-w, y+h);
                // Top horizontal
                glVertex2f(x-w, y+h-thick); glVertex2f(x+w, y+h-thick); glVertex2f(x+w, y+h); glVertex2f(x-w, y+h);
                // Right vertical (top half)
                glVertex2f(x+w-thick, y); glVertex2f(x+w, y); glVertex2f(x+w, y+h); glVertex2f(x+w-thick, y+h);
                // Middle horizontal
                glVertex2f(x-w+thick, y-thick/2); glVertex2f(x+w, y-thick/2); glVertex2f(x+w, y+thick/2); glVertex2f(x-w+thick, y+thick/2);
                // Diagonal
                glVertex2f(x, y-thick/2); glVertex2f(x+thick/2, y-thick/2); glVertex2f(x+w, y-h); glVertex2f(x+w-thick/2, y-h);
                break;
                
            case 'S':
                // Top horizontal
                glVertex2f(x-w, y+h-thick); glVertex2f(x+w, y+h-thick); glVertex2f(x+w, y+h); glVertex2f(x-w, y+h);
                // Left vertical (top half)
                glVertex2f(x-w, y); glVertex2f(x-w+thick, y); glVertex2f(x-w+thick, y+h); glVertex2f(x-w, y+h);
                // Middle horizontal
                glVertex2f(x-w, y-thick/2); glVertex2f(x+w, y-thick/2); glVertex2f(x+w, y+thick/2); glVertex2f(x-w, y+thick/2);
                // Right vertical (bottom half)
                glVertex2f(x+w-thick, y-h); glVertex2f(x+w, y-h); glVertex2f(x+w, y); glVertex2f(x+w-thick, y);
                // Bottom horizontal
                glVertex2f(x-w, y-h); glVertex2f(x+w, y-h); glVertex2f(x+w, y-h+thick); glVertex2f(x-w, y-h+thick);
                break;
                
            case 'T':
                // Top horizontal
                glVertex2f(x-w, y+h-thick); glVertex2f(x+w, y+h-thick); glVertex2f(x+w, y+h); glVertex2f(x-w, y+h);
                // Center vertical
                glVertex2f(x-thick/2, y-h); glVertex2f(x+thick/2, y-h); glVertex2f(x+thick/2, y+h); glVertex2f(x-thick/2, y+h);
                break;
                
            case 'U':
                // Left vertical
                glVertex2f(x-w, y-h+thick); glVertex2f(x-w+thick, y-h+thick); glVertex2f(x-w+thick, y+h); glVertex2f(x-w, y+h);
                // Right vertical
                glVertex2f(x+w-thick, y-h+thick); glVertex2f(x+w, y-h+thick); glVertex2f(x+w, y+h); glVertex2f(x+w-thick, y+h);
                // Bottom horizontal
                glVertex2f(x-w+thick, y-h); glVertex2f(x+w-thick, y-h); glVertex2f(x+w-thick, y-h+thick); glVertex2f(x-w+thick, y-h+thick);
                break;
                
            case ' ':
                // Space - draw nothing
                break;
                
            default:
                // Unknown character - draw a small box
                glVertex2f(x-w/2, y-h/2); glVertex2f(x+w/2, y-h/2); glVertex2f(x+w/2, y+h/2); glVertex2f(x-w/2, y+h/2);
                break;
        }
        
        glEnd();
    }
    
    void drawSpeedSlider() {
        // Speed slider background
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(-0.4f, -0.85f);
        glVertex2f(0.4f, -0.85f);
        glVertex2f(0.4f, -0.8f);
        glVertex2f(-0.4f, -0.8f);
        glEnd();
        
        // Speed slider fill
        glColor3f(0.6f, 0.6f, 1.0f);
        float speedBarWidth = ((ballSpeedMultiplier - 0.2f) / (3.0f - 0.2f)) * 0.8f;
        glBegin(GL_QUADS);
        glVertex2f(-0.4f, -0.85f);
        glVertex2f(-0.4f + speedBarWidth, -0.85f);
        glVertex2f(-0.4f + speedBarWidth, -0.8f);
        glVertex2f(-0.4f, -0.8f);
        glEnd();
        
        // Speed slider handle
        float handleX = -0.4f + speedBarWidth;
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(handleX - 0.02f, -0.87f);
        glVertex2f(handleX + 0.02f, -0.87f);
        glVertex2f(handleX + 0.02f, -0.78f);
        glVertex2f(handleX - 0.02f, -0.78);
        glEnd();
        
        // Speed value display as simple bars
        int speedBars = (int)(ballSpeedMultiplier * 5);
        glColor3f(1.0f, 1.0f, 0.0f);
        for (int i = 0; i < speedBars && i < 15; i++) {
            float x = -0.3f + i * 0.04f;
            glBegin(GL_QUADS);
            glVertex2f(x, -0.92f);
            glVertex2f(x + 0.02f, -0.92f);
            glVertex2f(x + 0.02f, -0.88f);
            glVertex2f(x, -0.88f);
            glEnd();
        }
    }
    
    void handleSpeedSliderInput() {
        // Mouse input for speed slider
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
        // Convert to OpenGL coordinates
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        float glX = (mouseX / windowWidth) * 2.0f - 1.0f;
        float glY = 1.0f - (mouseY / windowHeight) * 2.0f;
        
        // Check if mouse is over speed slider
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
            glY >= -0.87f && glY <= -0.78f && glX >= -0.4f && glX <= 0.4f) {
            
            float sliderPos = (glX + 0.4f) / 0.8f; // Normalize to 0-1
            ballSpeedMultiplier = 0.2f + sliderPos * (3.0f - 0.2f);
            
            // Clamp values
            if (ballSpeedMultiplier < 0.2f) ballSpeedMultiplier = 0.2f;
            if (ballSpeedMultiplier > 3.0f) ballSpeedMultiplier = 3.0f;
        }
        
        // Keyboard fallback
        static bool plusPressed = false, minusPressed = false;
        bool plusKey = glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS;
        bool minusKey = glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS;
        
        if (plusKey && !plusPressed) {
            ballSpeedMultiplier += 0.2f;
            if (ballSpeedMultiplier > 3.0f) ballSpeedMultiplier = 3.0f;
        }
        if (minusKey && !minusPressed) {
            ballSpeedMultiplier -= 0.2f;
            if (ballSpeedMultiplier < 0.2f) ballSpeedMultiplier = 0.2f;
        }
        plusPressed = plusKey;
        minusPressed = minusKey;
    }
    
    void cleanup() {
        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }
    
    ~LidPongGame() {
        cleanup();
    }
};

int main() {
    LidPongGame game;
    
    if (!game.init()) {
        return -1;
    }
    
    game.run();
    
    return 0;
}
