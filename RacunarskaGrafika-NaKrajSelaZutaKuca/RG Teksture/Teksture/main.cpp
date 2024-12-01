//Autor: Ognjen Adzic
//Opis: Projekat 2D Na kraj sela zuta kuca

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <thread>
#include <chrono>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cmath>
#define M_PI 3.14159265358979323846
#define CRES 30 


//stb_image.h je header-only biblioteka za ucitavanje tekstura.
//Potrebno je definisati STB_IMAGE_IMPLEMENTATION prije njenog ukljucivanja
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath); //Ucitavanje teksture, izdvojeno u funkciju

void initialize(unsigned int VAO, unsigned int VBO, float* vertices, int verticesCount);
void initializeTexture(unsigned int VAO, unsigned int VBO, float* vertices, int verticesCount, unsigned indexTexture);

void generateCircle(float circle[], float centerX, float centerY, float radius, float aspectRatio, int beg);
void generateImperfectCircle(float circle[], float centerX, float centerY, float radius, float aspectRatio, int beg);
void generateClouds(float circleVerticesCloud[], float aspectRatio, float time);
GLFWcursor* createCustomCursor(const char* imagePath);

int main(void)
{

    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Primarni monitor i video mode
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // Kreiranje fullscreen prozora
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Na kraj sela zuta kuca", monitor, NULL);

    float aspectRatio = static_cast<float>(mode->width) / mode->height;

    if (window == NULL) {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    // Load the custom cursor
    GLFWcursor* boneCursor = createCustomCursor("res/icons8-human-bone-40.png");
    if (boneCursor == NULL) {
        std::cout << "Failed to create custom cursor!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set the custom cursor
    glfwSetCursor(window, boneCursor);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");

    unsigned int VAO[21];
    glGenVertexArrays(21, VAO);
    unsigned int VBO[21];
    glGenBuffers(21, VBO);

    //sky
    float skyVertices[] = {
        -1.0, -1.0,
        1.0, -1.0,
        -1.0, 1.0,
        1.0, 1.0,
    };
    initialize(VAO[0], VBO[0], skyVertices, sizeof(skyVertices));
    // notacija koordinata za teksture je STPQ u GLSL-u (ali se cesto koristi UV za 2D teksture i STR za 3D)
    //ST koordinate u nizu tjemena su koordinate za teksturu i krecu se od 0 do 1, gdje je 0, 0 donji lijevi ugao teksture
    //Npr. drugi red u nizu tjemena ce da mapira boje donjeg lijevog ugla teksture na drugo tjeme
   
    //sun and moon
    float circles[((2 + CRES) * 2) * 2];
    generateCircle(circles, 0.0f, 0.0f, 0.1f, aspectRatio, 0);
    generateCircle(circles, 0.0f, 0.0f, 0.15f, aspectRatio, (2 + CRES) * 2);
    initialize(VAO[1], VBO[1], circles, sizeof(circles));

    //grass
    float grassVertices[] = {
        -1.0, -0.25,
        1.0, -0.25,
        -1.0, -1.0,
        1.0, -1.0,
    };
    initialize(VAO[2], VBO[2], grassVertices, sizeof(grassVertices));

    // index texture
    float indexVertices[] = {
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f, -0.85f,  1.0f, 1.0f,
        0.7f, -1.0f,  0.0f, 0.0f,
        0.7f, -0.85f,  0.0f, 1.0f
    };
    unsigned indexTexture = loadImageToTexture("res/index.png");
    initializeTexture(VAO[3], VBO[3], indexVertices, sizeof(indexVertices), indexTexture);

    // House vertices (walls)
    float houseVertices[] = {
        -0.2f, -0.35f,  // Bottom left
        0.2f, -0.35f,   // Bottom right
        -0.2f, 0.0f,    // Top left (first floor)
        0.2f, 0.0f,     // Top right (first floor)
        -0.2f, 0.25f,   // Top left (second floor)
        0.2f, 0.25f,    // Top right (second floor)
    };
    initialize(VAO[4], VBO[4], houseVertices, sizeof(houseVertices));

    // Roof vertices (triangular roof)
    float roofVertices[] = {
        -0.25f, 0.25f,  // Left corner
        0.25f, 0.25f,   // Right corner
        0.0f, 0.4f,     // Peak
    };
    initialize(VAO[5], VBO[5], roofVertices, sizeof(roofVertices));

    // Divider line (horizontal)
    float dividerLineVertices[] = {
        -0.2f, 0.0f,   // Start of line (left)
        0.2f, 0.0f,    // End of line (right)
    };
    initialize(VAO[6], VBO[6], dividerLineVertices, sizeof(dividerLineVertices));
    
    // Door vertices
    float doorVertices[] = {
        -0.05f, -0.35f,  // Bottom left
        0.05f, -0.35f,   // Bottom right
        -0.05f, -0.2f,   // Top left
        0.05f, -0.2f,    // Top right
    };
    initialize(VAO[7], VBO[7], doorVertices, sizeof(doorVertices));

    // First-floor left window
    float window1Vertices[] = {
        -0.15f, -0.15f, // Bottom left
        -0.05f, -0.15f, // Bottom right
        -0.15f, -0.05f,  // Top left
        -0.05f, -0.05f,  // Top right
    };
    initialize(VAO[8], VBO[8], window1Vertices, sizeof(window1Vertices));

    // First-floor right window
    float window2Vertices[] = {
         0.05f, -0.15f, // Bottom left
         0.15f, -0.15f, // Bottom right
         0.05f, -0.05f,  // Top left
         0.15f, -0.05f,  // Top right
    };
    initialize(VAO[9], VBO[9], window2Vertices, sizeof(window2Vertices));

    // Second-floor left window
    float window3Vertices[] = {
        -0.15f, 0.1f, // Bottom left
        -0.05f, 0.1f, // Bottom right
        -0.15f, 0.2f, // Top left
        -0.05f, 0.2f, // Top right
    };
    initialize(VAO[10], VBO[10], window3Vertices, sizeof(window3Vertices));

    // Second-floor right window
    float window4Vertices[] = {
         0.05f, 0.1f, // Bottom left
         0.15f, 0.1f, // Bottom right
         0.05f, 0.2f, // Top left
         0.15f, 0.2f, // Top right
    };
    initialize(VAO[11], VBO[11], window4Vertices, sizeof(window4Vertices));

    float fenceOffset = -0.45f; // Move fence further down

    // Fence vertical poles (5 independent poles)
    float fencePoles[] = {
        -0.9f, -0.5f + fenceOffset, -0.88f, -0.5f + fenceOffset, -0.9f, -0.1f + fenceOffset, -0.88f, -0.1f + fenceOffset,   // First pole
        -0.5f, -0.5f + fenceOffset, -0.48f, -0.5f + fenceOffset, -0.5f, -0.1f + fenceOffset, -0.48f, -0.1f + fenceOffset,   // Second pole
        -0.1f, -0.5f + fenceOffset, -0.08f, -0.5f + fenceOffset, -0.1f, -0.1f + fenceOffset, -0.08f, -0.1f + fenceOffset,   // Third pole
         0.3f, -0.5f + fenceOffset,  0.32f, -0.5f + fenceOffset,  0.3f, -0.1f + fenceOffset,  0.32f, -0.1f + fenceOffset,   // Fourth pole
         0.7f, -0.5f + fenceOffset,  0.72f, -0.5f + fenceOffset,  0.7f, -0.1f + fenceOffset,  0.72f, -0.1f + fenceOffset,   // Fifth pole
    };
    initialize(VAO[12], VBO[12], fencePoles, sizeof(fencePoles));

    // Fence horizontal bars (3 bars)
    float fenceBars[] = {
        -1.0f, -0.4f + fenceOffset,  1.0f, -0.4f + fenceOffset, -1.0f, -0.38f + fenceOffset,  1.0f, -0.38f + fenceOffset, // Bottom bar
        -1.0f, -0.3f + fenceOffset,  1.0f, -0.3f + fenceOffset, -1.0f, -0.28f + fenceOffset,  1.0f, -0.28f + fenceOffset, // Middle bar
        -1.0f, -0.2f + fenceOffset,  1.0f, -0.2f + fenceOffset, -1.0f, -0.18f + fenceOffset,  1.0f, -0.18f + fenceOffset, // Top bar
    };
    initialize(VAO[13], VBO[13], fenceBars, sizeof(fenceBars));

    // Chimney (a rectangle)
    float chimneyVertices[] = {
        0.1f, 0.30f,  // Bottom left
        0.15f, 0.30f, // Bottom right
        0.1f, 0.45f,  // Top left
        0.15f, 0.45f, // Top right
    };
    initialize(VAO[14], VBO[14], chimneyVertices, sizeof(chimneyVertices));

    // clouds
    float circleVerticesCloud[((2 + CRES) * 2) * 5]{};
    float currentTime = glfwGetTime();
    generateClouds(circleVerticesCloud, 800.0f / 600.0f, currentTime);
    initialize(VAO[15], VBO[15], circleVerticesCloud, sizeof(circleVerticesCloud));

    //dog
    float dogVertices[] = {
        // Positions        // Texture Coords
        -0.2f, -0.45f, 0.0f, 0.0f,  // Bottom-left
         0.2f, -0.45f, 1.0f, 0.0f,  // Bottom-right
        -0.2f, -0.25f, 0.0f, 1.0f,  // Top-left
         0.2f, -0.25f, 1.0f, 1.0f   // Top-right
    };
    unsigned int dogTexture = loadImageToTexture("res/dog_no_bg.png"); // Replace with the path to your dog image
    initializeTexture(VAO[16], VBO[16], dogVertices, sizeof(dogVertices), dogTexture);

    float zVertices[] = {
    -0.05f,  0.05f,  // Top-left
     0.05f,  0.05f,  // Top-right
     0.05f,  0.03f,  // Diagonal top-right
    -0.05f, -0.03f,  // Diagonal bottom-left
    -0.05f, -0.05f,  // Bottom-left
     0.05f, -0.05f   // Bottom-right
    };
    initialize(VAO[17], VBO[17], zVertices, sizeof(zVertices));

    unsigned int stickmanTexture = loadImageToTexture("res/stickman.png");
    // Stickman positions for each window
    float stickmanPositions[4][2] = {
        {-0.1f, -0.10f}, // First-floor left window
        { 0.1f, -0.10f}, // First-floor right window
        {-0.1f,  0.15f}, // Second-floor left window
        { 0.1f,  0.15f}  // Second-floor right window
    };

    // VAOs and VBOs for stickmen
    unsigned int stickmanVAO[4];
    unsigned int stickmanVBO[4];
    glGenVertexArrays(4, stickmanVAO);
    glGenBuffers(4, stickmanVBO);

    // Initialize stickmen
    for (int i = 0; i < 4; i++) {
        float stickmanVertices[] = {
            stickmanPositions[i][0] - 0.05f, stickmanPositions[i][1] - 0.05f, 0.0f, 0.0f,
            stickmanPositions[i][0] + 0.05f, stickmanPositions[i][1] - 0.05f, 1.0f, 0.0f,
            stickmanPositions[i][0] - 0.05f, stickmanPositions[i][1] + 0.05f, 0.0f, 1.0f,
            stickmanPositions[i][0] + 0.05f, stickmanPositions[i][1] + 0.05f, 1.0f, 1.0f,
        };
        initializeTexture(stickmanVAO[i], stickmanVBO[i], stickmanVertices, sizeof(stickmanVertices), stickmanTexture);
    }

    // Tree trunk (rectangle)
    float trunkVertices[] = {
     0.6f, -0.3f,  // Bottom-left
     0.7f, -0.3f,  // Bottom-right
     0.6f, -0.0f,  // Top-left
     0.7f, -0.0f   // Top-right
    };
    initialize(VAO[18], VBO[18], trunkVertices, sizeof(trunkVertices));
    
    /// Tree foliage (circle)
    float foliageVertices[(2 + CRES) * 2]; // Circle vertices
    generateCircle(foliageVertices, 0.65f, 0.2f, 0.25f, aspectRatio, 0); // Center at (0.65, -0.2), radius 0.25
    initialize(VAO[19], VBO[19], foliageVertices, sizeof(foliageVertices));

    // Food vertices (a small square)
    float foodVertices[] = {
        -0.02f, -0.02f,  // Bottom-left
         0.02f, -0.02f,  // Bottom-right
        -0.02f,  0.02f,  // Top-left
         0.02f,  0.02f   // Top-right
    };
    initialize(VAO[20], VBO[20], foodVertices, sizeof(foodVertices));

    float speed = 0.5;
    float r = 1.0;
    //float cloudOffsetX = 0.0f;

    float animationStartTime = -1.0f; // Start time of the animation
    bool animateOnce = false;         // Flag to control animation

    enum AnimationPhase { SunRising, SunSetting };
    AnimationPhase animationPhase = SunSetting; // Start with sun rising

    float sunX = 0.0f, sunY = r; // Sun starts at the top
    float moonX = 0.0f, moonY = -r; // Moon starts at the 

    float dogX = 0.0f; // Initial horizontal position
    float dogY = -0.1f; // Fixed vertical position
    int dogDirection = 1; // 1 for right, -1 for left
    float dogSpeed = 0.001f; // Movement speed
    // Variables for "Z" animation
    const int maxZs = 10;
    float zX[maxZs], zY[maxZs], zOpacity[maxZs], zOscillation[maxZs];
    bool zActive[maxZs];
    float zSpawnInterval = 0.5f;
    float zLastSpawnTime = 0.0f;

    bool isTransparent = false;
    // Animation variables
    bool isWindowAnimating = false;
    int activeWindowIndex = -1; // Index of the window being animated
    float windowAnimationStartTime = 0.0f;  // Record animation start time
    float windowAnimationProgress = 0.0f; // Animation progress (0.0 to 1.0)
    const float animationDuration = 2.5f; // Animation duration in seconds
    //bool stickmanVisible = false;
    bool stickmanVisibleWindow[4] = { false, false, false, false };

    const int targetFPS = 60;
    const double frameTime = 1.0 / targetFPS;

    // Vreme početka petlje
    double previousTime = glfwGetTime();

    float paintProgress = 0.0f;

    // Initialize "Z" states
    for (int i = 0; i < maxZs; i++) zActive[i] = false;

    //dog food
    bool isFoodActive = false;  // Is the food currently active?
    float foodX = 0.0f, foodY = 0.0f;  // Position of the food
    double foodSpawnTime = 0.0;  // Time when the food was spawned
    float waitTime = 3.0f;  // Random wait time before food disappears
    float previousDogX = dogX, previousDogY = dogY; // Initial position before moving to food
    bool isReturningToInitial = false;
    bool isDogAtFood = false;  // Is the dog at the food?


    GLuint sunPosLoc = glGetUniformLocation(unifiedShader, "sunPos");
    GLuint cloudPosLoc = glGetUniformLocation(unifiedShader, "cloudCircleCenter");
    GLuint dogPosLoc = glGetUniformLocation(unifiedShader, "dogPos");
    GLuint dogDirectionLoc = glGetUniformLocation(unifiedShader, "dogDirection");
    GLuint zPosLoc = glGetUniformLocation(unifiedShader, "zPos");
    GLuint zAlphaLoc = glGetUniformLocation(unifiedShader, "zAlpha");
    GLuint transparencyLoc = glGetUniformLocation(unifiedShader, "transparency");
    //GLuint windowColorLoc = glGetUniformLocation(unifiedShader, "windowColor");
    GLuint timeLoc = glGetUniformLocation(unifiedShader, "time");
    GLuint elementMode = glGetUniformLocation(unifiedShader, "mode");
    GLuint isSun = glGetUniformLocation(unifiedShader, "isSun");
    GLuint useTextureLoc = glGetUniformLocation(unifiedShader, "useTexture");
    GLuint paintProgressLoc = glGetUniformLocation(unifiedShader, "paintProgress");
    GLuint foodPosLoc = glGetUniformLocation(unifiedShader, "foodPos");



    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(window))
    {
        double currentTimeFPS = glfwGetTime();
        double elapsedTimeFPS = currentTimeFPS - previousTime;
        if (elapsedTimeFPS < frameTime) {
            std::this_thread::sleep_for(
                std::chrono::duration<double>(frameTime - elapsedTimeFPS));
            continue;
        }
        previousTime = currentTime;
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !animateOnce)
        {
            animateOnce = true;
            animationStartTime = glfwGetTime(); // Record the start time

            if (animationPhase == SunRising)
            {
                animationPhase = SunSetting;
            }
            else
            {
                animationPhase = SunRising;
            }
        }
        static bool keyWasPressed = false; // Tracks the key press state
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            if (!keyWasPressed) {
                isTransparent = !isTransparent; // Toggle transparency state
                keyWasPressed = true; // Mark the key as pressed
            }
        }
        else {
            keyWasPressed = false; // Reset the key press state when the key is released
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            paintProgress = std::min(paintProgress + 0.0005f, 1.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            paintProgress = 0.0f; // Reset painting progress
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // Convert mouse coordinates to normalized device coordinates (NDC)
            float normalizedX = (mouseX / mode->width) * 2.0f - 1.0f;
            float normalizedY = 1.0f - (mouseY / mode->height) * 2.0f;

            float fenceTopBoundary = -0.1f + fenceOffset;

            // Check if click is inside the house and on the grass
            if (!isFoodActive && normalizedY > fenceTopBoundary && normalizedY < -0.25 && sunY < -0.0f) {  // Grass area
                previousDogX = dogX;
                previousDogY = dogY;
                
                foodX = 0.0f;
                foodY = -0.25f;
                isFoodActive = true;
                foodSpawnTime = glfwGetTime();
                waitTime = 3.0f + ((rand() % 2000) / 1000.0f);  // Random wait time (3-5 seconds)
            }
        }
        if (isFoodActive && !isDogAtFood && !isReturningToInitial) {
            float dx = foodX - dogX ;
            float dy = foodY - dogY + 0.3;
            float distance = sqrt(dx * dx + dy * dy);

            float moveSpeed = 0.002f;  // Adjust speed for smooth movement
            float stopThreshold = 0.005f;  // Threshold for stopping close to the food (make this smaller for closer proximity)

            if (distance > stopThreshold) {  // Continue moving until within the threshold distance
                dogX += moveSpeed * dx / distance;
                dogY += moveSpeed * dy / distance;
            }
            else {
                isDogAtFood = true;  // Dog has reached the food
                foodSpawnTime = glfwGetTime();  // Record arrival time
            }
        }
        if (isDogAtFood && glfwGetTime() - foodSpawnTime > waitTime) {
            isDogAtFood = false;            // Reset the "at food" flag
            isFoodActive = false;           // Remove the food
            isReturningToInitial = true;    // Start returning to the initial position
        }
        // Handle the return to the initial position
        if (isReturningToInitial) {
            float dx = previousDogX - dogX;
            float dy = previousDogY - dogY;
            float distance = sqrt(dx * dx + dy * dy);

            float returnSpeed = 0.002f;  // Speed for returning
            if (distance > 0.005f) {
                dogX += returnSpeed * dx / distance;
                dogY += returnSpeed * dy / distance;
            }
            else {
                isReturningToInitial = false; // Stop moving once back at the initial position
            }
        }

        
        if (!isWindowAnimating) {
            isWindowAnimating = true;
            windowAnimationStartTime = glfwGetTime(); // Start animation
            activeWindowIndex = rand() % 4 + 8;      // Randomly select a window (VAOs 8 to 11)
        }

        if (isWindowAnimating) {
            float currentTime = glfwGetTime();
            windowAnimationProgress = (currentTime - windowAnimationStartTime) / animationDuration;

            // Clamp progress to [0.0, 1.0] and check if animation has ended
            if (windowAnimationProgress >= 1.0f) {
                windowAnimationProgress = 1.0f;
                isWindowAnimating = false; // Reset animation state
            }
        }

        if (sunY < -0.0f) {
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                if (dogX + dogSpeed <= 0.8) {
                    dogX += dogSpeed;
                }
                dogDirection = 1;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                if (dogX - dogSpeed >= -0.8) {
                    dogX -= dogSpeed;
                }
                dogDirection = -1;
            }
        }
        
        

        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(unifiedShader);

     
        
        //sky        
        glBindVertexArray(VAO[0]);
        glUniform1i(elementMode, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        

        //sun, moon
        float currentTime = glfwGetTime();
        float elapsedTime = currentTime - animationStartTime;
        glUniform1f(timeLoc, currentTime);

        // Reset animation flag after completing one circle
        if (animateOnce && elapsedTime > (1 * M_PI) / speed)
        {
            animateOnce = false;
        }

        glBindVertexArray(VAO[1]);
        glUniform1i(elementMode, 1);

        if (animateOnce)
        {
            // Calculate elapsed time
            float angle = elapsedTime * speed;

            if (animationPhase == SunRising)
            {
                // Sun starts at the bottom (rising), moon at the top (descending)
                sunX = r * sin(angle);
                sunY = r * cos(angle);
                moonX = r * sin(angle + M_PI);
                moonY = r * cos(angle + M_PI);

                // Draw moon
                glUniform1i(isSun, GL_FALSE);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 32);
                glUniform2f(sunPosLoc, moonX, moonY);

                // Draw sun
                glUniform1i(isSun, GL_TRUE);
                glDrawArrays(GL_TRIANGLE_FAN, 32, 32);
                glUniform2f(sunPosLoc, sunX, sunY);

                // Complete animation when the sun is at the top
                if (elapsedTime > (M_PI / speed))
                {
                    animateOnce = false;
                    sunX = 0.0f; // Sun stays at the top
                    sunY = r;
                    moonX = 0.0f; // Moon stays at the bottom
                    moonY = -r;

                    
                }
            }
            else if (animationPhase == SunSetting)
            {
                // Sun starts at the top (setting), moon at the bottom (rising)
                sunX = r * sin(angle + M_PI);
                sunY = r * cos(angle + M_PI);
                moonX = r * sin(angle);
                moonY = r * cos(angle);

                // Draw moon
                glUniform1i(isSun, GL_FALSE);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 32);
                glUniform2f(sunPosLoc, moonX, moonY);

                // Draw sun
                glUniform1i(isSun, GL_TRUE);
                glDrawArrays(GL_TRIANGLE_FAN, 32, 32);
                glUniform2f(sunPosLoc, sunX, sunY);

                // Complete animation when the sun is at the bottom
                if (elapsedTime > (M_PI / speed))
                {
                    animateOnce = false;
                    sunX = 0.0f; // Sun stays at the bottom
                    sunY = -r;
                    moonX = 0.0f; // Moon stays at the top
                    moonY = r;
                }
            }
        }
        else {
            glUniform1i(isSun, GL_FALSE);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 32);
            glUniform2f(sunPosLoc, moonX, moonY);

            glUniform1i(isSun, GL_TRUE);
            glDrawArrays(GL_TRIANGLE_FAN, 32, 32);
            glUniform2f(sunPosLoc, sunX, sunY);
        }

        //grass
        glBindVertexArray(VAO[2]);
        glUniform1i(elementMode, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //house
        glBindVertexArray(VAO[4]);
        glUniform1i(elementMode, 4); // Mode for the house
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); // Draw walls (two floors)

        //roof
        glBindVertexArray(VAO[5]);
        glUniform1i(elementMode, 5); // Mode for the roof
        glDrawArrays(GL_TRIANGLES, 0, 3); // Draw roof

        // Render divider line
        glBindVertexArray(VAO[6]);
        glUniform1i(elementMode, 6); // Mode for divider line
        glDrawArrays(GL_LINES, 0, 2);

        if (!(isFoodActive && dogX >= -0.05f && dogX <= 0.05f )) {
            glBindVertexArray(VAO[7]);  // Door VAO
            glUniform1i(elementMode, 7);  // Door mode
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // Render the door
        }


        // Render windows with transparency if toggled
        for (int i = 8; i <= 11; i++) { // Loop through window VAOs
            glBindVertexArray(VAO[i]);  // Bind the current window VAO
            glUniform1i(elementMode, 8); // Mode for windows
            glUniform1f(transparencyLoc, isTransparent ? 0.5f : 1.0f); // Set transparency (alpha)
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Render the window
        }

        // Render windows and highlight the active one
        for (int i = 8; i <= 11; i++) {
            glBindVertexArray(VAO[i]);
            glUniform1i(elementMode, 8); // Mode for windows

            if (i == activeWindowIndex && isWindowAnimating) {
                glUniform1f(glGetUniformLocation(unifiedShader, "windowAnimationProgress"), windowAnimationProgress);
            }
            else {
                glUniform1f(glGetUniformLocation(unifiedShader, "windowAnimationProgress"), 0.0f); // Fully yellow
            }

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Render the window
        }

        if (isWindowAnimating && windowAnimationProgress > 0.7f) {
            int stickmanIndex = activeWindowIndex - 8; // Map active window to stickman index
            glBindVertexArray(stickmanVAO[stickmanIndex]);
            glUniform1i(elementMode, 15); // Mode for stickman
            glUniform1i(useTextureLoc, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D, stickmanTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glUniform1i(useTextureLoc, GL_FALSE);
        }
        // Render stickman in the first-floor left window if transparent
        if (isTransparent) {
            glBindVertexArray(stickmanVAO[1]); // Bind the VAO for the first-floor left window
            glUniform1i(elementMode, 15); // Assign a unique mode for stickman
            glUniform1i(useTextureLoc, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D, stickmanTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw stickman texture
            glUniform1i(useTextureLoc, GL_FALSE);
        }
        
        // Render vertical poles (independent)
        for (int i = 0; i < 5; i++) {
            glBindVertexArray(VAO[12]);
            glUniform1i(elementMode, 9); // Mode for vertical poles
            glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4); // Each pole has 4 vertices
        }

        // Render horizontal bars
        for (int i = 0; i < 3; i++) {
            glBindVertexArray(VAO[13]);
            glUniform1i(elementMode, 10); // Mode for horizontal bars
            glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4); // All 3 bars, 4 vertices each
        }
        
        // Render chimney
        glBindVertexArray(VAO[14]);
        glUniform1i(elementMode, 11); // Mode for chimney
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

       

        // Smoke animation
        glBindVertexArray(VAO[15]); // Assuming VAO[15] is used for smoke
        glUniform1i(elementMode, 12);

        float baseChimneyX = 0.12f; // Chimney's X position
        float baseChimneyY = -0.08f;  // Chimney's top Y position

        // Constants for animation
        const int maxPuffs = 10; // Maximum number of active puffs
        const float spawnInterval = 2.0f; // Time between new puffs (in seconds)
        const float moveSpeed = 0.0002f; // Vertical movement speed

        // Animation variables
        static float puffOffsetsY[maxPuffs];   // Vertical positions of each puff
        static bool puffActive[maxPuffs];      // Active state for each puff
        static float lastSpawnTime = 0.0f;     // Time of the last puff spawn

        // Initialize puff positions and states (run only once)
        static bool initialized = false;
        if (!initialized) {
            for (int i = 0; i < maxPuffs; i++) {
                puffOffsetsY[i] = -1.0f; // Start far below the chimney
                puffActive[i] = false;   // All puffs initially inactive
            }
            initialized = true;
        }

        // Update logic
        currentTime = glfwGetTime(); // Get current time in seconds
        if (currentTime - lastSpawnTime >= spawnInterval) {
            lastSpawnTime = currentTime; // Update spawn time

            // Find the first inactive puff and activate it
            for (int i = 0; i < maxPuffs; i++) {
                if (!puffActive[i]) {
                    puffOffsetsY[i] = 0.0f; // Reset position to chimney top
                    puffActive[i] = true;  // Activate puff
                    break; // Only activate one puff at a time
                }
            }
        }

        // Move active puffs
        for (int i = 0; i < maxPuffs; i++) {
            if (puffActive[i]) {
                puffOffsetsY[i] += moveSpeed; // Move puff upward

                if (puffOffsetsY[i] > 1.0f) {
                    puffActive[i] = false; // Deactivate puff when off-screen
                }
            }
        }

        // Render all active puffs
        for (int i = 0; i < maxPuffs; i++) {
            if (puffActive[i]) {
                float puffX = baseChimneyX;              // Fixed X position at chimney
                float puffY = baseChimneyY + puffOffsetsY[i]; // Current Y position

                glUniform2f(cloudPosLoc, puffX, puffY);  // Pass position to shader
                glDrawArrays(GL_TRIANGLE_FAN, 32 * i, 32); // Render puff
            }
        }

        // Set dog position and direction uniforms
        glUniform2f(dogPosLoc, dogX, dogY); // Pass the dog's position
        glUniform1i(dogDirectionLoc, dogDirection); // Pass the dog's direction

        if (isFoodActive) {
            glBindVertexArray(VAO[20]);  // Food VAO
            glUniform1i(elementMode, 18);  // Mode for food
            glUniform2f(foodPosLoc, foodX, foodY);  // Set food position
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // Render the food
        }

        // Render the dog texture
        glBindVertexArray(VAO[16]);
        glUniform1i(elementMode, 13); // Assign a unique mode for the dog texture
        glUniform1i(useTextureLoc, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, dogTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dogTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glUniform1i(useTextureLoc, GL_FALSE);

        // Nighttime logic for rendering "Z"
        if (sunY > 0.0f) {
            float currentTime = glfwGetTime();

            // Spawn new "Z" letters
            if (currentTime - zLastSpawnTime > zSpawnInterval) {
                zLastSpawnTime = currentTime;
                for (int i = 0; i < maxZs; i++) {
                    if (!zActive[i]) {
                        zX[i] = dogX + 0.1;
                        zY[i] = dogY - 0.2;
                        zOpacity[i] = 1.0f;
                        zOscillation[i] = 0.0f;
                        zActive[i] = true;
                        break;
                    }
                }
            }

            // Update "Z" letters
            for (int i = 0; i < maxZs; i++) {
                if (zActive[i]) {
                    zY[i] += 0.0002f;           // Move upward slower
                    zOscillation[i] += 0.001f;  // Oscillate slower
                    zOpacity[i] -= 0.0005f;     // Fade out slower

                    if (zOpacity[i] <= 0.0f || zY[i] > 1.0f) zActive[i] = false;
                }
            }

            // Render "Z" letters
            for (int i = 0; i < maxZs; i++) {
                if (zActive[i]) {
                    float zPosX = zX[i] + 0.02f * sin(zOscillation[i]); // Smaller oscillation
                    float zPosY = zY[i];

                    // Set uniforms for position and opacity
                    glUniform2f(zPosLoc, zPosX, zPosY);
                    glUniform1f(zAlphaLoc, zOpacity[i]);

                    // Render "Z"
                    glBindVertexArray(VAO[17]);
                    glUniform1i(elementMode, 14); // Set mode for "Z"
                    glDrawArrays(GL_LINE_STRIP, 0, 6);
                }
            }
        }


        // Render tree trunk
        glUniform1f(paintProgressLoc, paintProgress);
        glBindVertexArray(VAO[18]);
        glUniform1i(elementMode, 16); // Mode for tree trunk
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw rectangle

        // Render tree foliage
        glBindVertexArray(VAO[19]);
        glUniform1i(elementMode, 17); // Mode for tree foliage
        glDrawArrays(GL_TRIANGLE_FAN, 0, CRES + 2); // Draw circle
        

        // index texture
        glBindVertexArray(VAO[3]);
        glUniform1i(elementMode, 3);
        glUniform1i(useTextureLoc, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, indexTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, indexTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glUniform1i(useTextureLoc, GL_FALSE);
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteTextures(1, &indexTexture);
    glDeleteBuffers(1, VBO);
    glDeleteVertexArrays(1, VAO);
    glDeleteProgram(unifiedShader);

    glfwDestroyCursor(boneCursor);
    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}
void initialize(unsigned int VAO, unsigned int VBO, float* vertices, int verticesCount) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesCount, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}
void initializeTexture(unsigned int VAO, unsigned int VBO, float* vertices, int verticesCount, unsigned indexTexture) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesCount, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, indexTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void generateCircle(float circle[], float centerX, float centerY, float radius, float aspectRatio, int beg) {
    circle[beg] = centerX; // Center X
    circle[beg + 1] = centerY; // Center Y
    for (int i = 0; i <= CRES; i++) {
        float angle = (M_PI / 180) * (i * 360.0 / CRES);
        circle[beg + 2 + 2 * i] = centerX + radius * cos(angle) / aspectRatio; // Xi
        circle[beg + 2 + 2 * i + 1] = centerY + radius * sin(angle); // Yi
    }
}
void generateClouds(float circleVerticesCloud[], float aspectRatio, float time) {
    float cloudRadius = 0.05f; // Adjust radius for smaller "smoke" particles
    const int numCloudCirclesCloud = 5; // Number of overlapping circles for each "smoke puff"
    float smokeCenterX = 0.01f;        // X position of the chimney
    float baseY = 0.55f;                // Starting Y position (chimney top)

    // Calculate dynamic Y offset based on time to animate upwards
    float smokeYOffset = fmod(time * 0.1f, 2.0f); // Smoke wraps around every 2 units

    // Generate "smoke puff" positions
    float cloudCircleCenters[numCloudCirclesCloud][2] = {
        { smokeCenterX, baseY + smokeYOffset },
        { smokeCenterX - 0.02f, baseY + smokeYOffset - 0.02f },
        { smokeCenterX + 0.02f, baseY + smokeYOffset - 0.02f },
        { smokeCenterX - 0.03f, baseY + smokeYOffset - 0.04f },
        { smokeCenterX + 0.03f, baseY + smokeYOffset - 0.04f }
    };

    // Generate vertices for each "smoke puff"
    for (int i = 0; i < numCloudCirclesCloud; ++i) {
        generateImperfectCircle(circleVerticesCloud, cloudCircleCenters[i][0], cloudCircleCenters[i][1], cloudRadius, aspectRatio, (2 + CRES) * 2 * i);
    }
}
void generateImperfectCircle(float circle[], float centerX, float centerY, float radius, float aspectRatio, int beg) {
    circle[beg] = centerX; // Center X
    circle[beg + 1] = centerY; // Center Y
    for (int i = 0; i <= CRES; i++) {
        float angle = (M_PI / 180) * (i * 360.0 / CRES);

        // Generate smoother randomness for perturbation
        float noiseFactor = (rand() % 50) / 1000.0f; // Smaller random variation for smoother edges
        float waveFactor = 0.02f * sin(i * 0.5f);    // Adds a smooth wavy perturbation
        float perturbedRadius = radius + noiseFactor + waveFactor;

        circle[beg + 2 + 2 * i] = centerX + perturbedRadius * cos(angle) / aspectRatio; // Xi
        circle[beg + 2 + 2 * i + 1] = centerY + perturbedRadius * sin(angle);          // Yi
    }
}
// Function to create a GLFW cursor from an image file
GLFWcursor* createCustomCursor(const char* imagePath) {
    int width, height, channels;
    unsigned char* imageData = stbi_load(imagePath, &width, &height, &channels, 4); // Force 4 channels (RGBA)

    if (!imageData) {
        std::cout << "Failed to load cursor image: " << imagePath << std::endl;
        return NULL;
    }

    if (width <= 0 || height <= 0) {
        std::cout << "Invalid cursor image dimensions: " << width << "x" << height << std::endl;
        stbi_image_free(imageData);
        return NULL;
    }

    GLFWimage cursorImage;
    cursorImage.width = width;
    cursorImage.height = height;
    cursorImage.pixels = imageData;

    // Set the hot-spot at the center of the image
    GLFWcursor* cursor = glfwCreateCursor(&cursorImage, 0, 0);
    if (!cursor) {
        std::cout << "Failed to create GLFW cursor!" << std::endl;
    }

    stbi_image_free(imageData); // Free image data after creating the cursor
    return cursor;
}

