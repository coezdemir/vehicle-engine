#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <filesystem>
#include "world/ACar.h"
#include "world/level/ADeathPlane.h"
#include <tuple>
#include "Game.h"

#define GET_STRING(x) #x
#define GET_DIR(x) GET_STRING(x)

/*
Shader* shader;
Mesh* mesh;
Car* car;
Car* car2;
std::vector<std::tuple<btCollisionShape*, btRigidBody*>> physiscsObjs;
std::vector<RaycastVehicle*> crashedCars = std::vector<RaycastVehicle*>();

GLuint vertexBuffer;
GLuint indexBuffer;

std::vector<PhysicsNode*> physicsObjects;
std::vector<AnimNode*> animNodes;

std::vector<AStaticMesh> trackTiles;

GroupNode rootNode = GroupNode(glm::mat4(1.0f));

const int SAMPLE_AMOUNT = 4;
const bool bPrintDeltaT = false;
bool bShowDebugWindow = true;

static std::filesystem::path assetRoot = "./assets";
const float W_WIDTH = 1920.f;
const float W_HEIGHT = 1080.f;
const char* W_TITLE = "GameLab III";

std::filesystem::path resolve(const std::filesystem::path &relativeAssetPath)
{
    return std::filesystem::canonical(GET_DIR(DEBUG_ASSET_ROOT) / (assetRoot / relativeAssetPath).make_preferred());
}

glm::vec3 getTileRotation(TileOrientation tileOrientation)
{
    switch(tileOrientation)
    {
        case TO_UP:
            return {0.f, 0.f, 0.f};
        case TO_RIGHT:
            return {0.f, 90.f, 0.f};
        case TO_DOWN:
            return {0.f, 180.f, 0.f};
        case TO_LEFT:
            return {0.f, 270.f, 0.f};
    }
    return {0.f, 0.f, 0.f};
}

void spawnTile(char* objPath, glm::vec2 position, TileOrientation tileOrientation)
{
    AStaticMesh* currentActor;

    trackTiles.emplace_back(glm::vec3(position.x, -10.f, position.y),
                            getTileRotation(tileOrientation),
                            glm::vec3(50.f, 50.f, 50.f),
                            Physics::getWorld(),
                            objPath);
    currentActor = &trackTiles.back();

    rootNode.add(currentActor->getNodes());
    currentActor->getPhysicsNode()->createPhysicsObject(currentActor->getTransform());
}

void spawnTrack(std::vector<TrackEditorTile>* tiles, std::vector<glm::vec2>* positions)
{
    std::vector<TrackEditorTile>& tilesRef = *tiles;
    std::vector<glm::vec2>& positionsRef = *positions;

    if(!trackTiles.empty())
    {
        for (int i = trackTiles.size() - 1; i >= 0; i--)
        {
            trackTiles[i].destroy();
            trackTiles.pop_back();
        }
    }

    for(int i = 0; i < tilesRef.size(); i++)
    {
        switch(tilesRef[i].type)
        {
            case TET_NONE:
                break;
            case TET_STRAIGHT:
                spawnTile("../../assets/models/Racetrack/default.obj", positionsRef[i], tilesRef[i].orientation);
                break;
            case TET_RIGHT:
                spawnTile("../../assets/models/Racetrack/rcurve.obj", positionsRef[i], tilesRef[i].orientation);
                break;
            case TET_LEFT:
                spawnTile("../../assets/models/Racetrack/lcurve.obj", positionsRef[i], tilesRef[i].orientation);
                break;
            case TET_LOOP:
                spawnTile("../../assets/models/Racetrack/loop.obj", positionsRef[i], tilesRef[i].orientation);
                break;
            case TET_O_VOID:
                break;
            case TET_O_RAMP:
                spawnTile("../../assets/models/Racetrack/ramp1.obj", positionsRef[i], tilesRef[i].orientation);
                break;
            case TET_O_HAMMER:
                break;
            case TET_O_BIGHAMMER:
                break;
        }
    }
}
 */

extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main()
{
    game::Game game = game::Game();
    game.run();
/*
    // Initialize GUI
    GUI::init(window, &spawnTrack);

    // Initialize Physics
    Physics::init();
    Shader debugShader = Shader(resolve("shaders/debugVertexShader.glsl"), resolve("shaders/debugFragmentShader.glsl"));
    BulletDebugDrawer debugDrawer = BulletDebugDrawer(&debugShader);
    Physics::setDebugDrawer(&debugDrawer);
    btDiscreteDynamicsWorld* world = Physics::getWorld();

    shader = new Shader(resolve("shaders/blinn-phong-vertex-shader.glsl"), resolve("shaders/blinn-phong-fragment-shader.glsl"));
    car = new Car(glm::vec3(20.f, 0.f, -10.f), glm::vec3(1.f), glm::vec3(0.5f, 0.5f, 0.5f), world, true);
    car2 = new Car(glm::vec3(20.f, 0.f, 10.f), glm::vec3(1.f), glm::vec3(0.5f, 0.5f, 0.5f), world, false);

    //Adding camera
    ACamera cameraActor = ACamera(glm::vec3(0.f, 5.f, -15.f),
                                          glm::vec3(0.f, 0.f ,0.f),
                                          glm::vec3(1.f), car);

    Arena arena = Arena(glm::vec3(0.f, -5.f, 0.f), glm::vec3(0.f), glm::vec3(1.f, 1.f, 1.f), world);
    ADeathPlane deathPlane = ADeathPlane(glm::vec3(0.f, -50.f, 0.f), glm::vec3(0.f), glm::vec3(500.f, 5.f, 500.f), world);
    ItemBox itemBox = ItemBox(glm::vec3(40.f, -3.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), world);
    ItemBox itemBox2 = ItemBox(glm::vec3(-40.f, -3.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), world);
    ItemBox itemBox3 = ItemBox(glm::vec3(-20.f, -3.f, -40.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), world);
    ItemBox itemBox4 = ItemBox(glm::vec3(20.f, -3.f, 40.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), world);
    ItemBox itemBox5 = ItemBox(glm::vec3(-20.f, -3.f, 40.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), world);
    ItemBox itemBox6 = ItemBox(glm::vec3(20.f, -3.f, -40.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), world);
    AStaticMesh ramps = AStaticMesh(glm::vec3(0.f,  -5.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), world, "../../assets/models/Arena1_Ramps.obj");
    AStaticMesh wall = AStaticMesh(glm::vec3(0.f,  -5.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), world, "../../assets/models/Arena1_Wall.obj");
    wall.setPhysicsNodeShouldUpdate(true);

    GroupNode* planeNode = arena.getNodes();
    GroupNode* itemBoxNode = itemBox.getNodes();
    GroupNode* itemBoxNode2 = itemBox2.getNodes();
    GroupNode* itemBoxNode3 = itemBox3.getNodes();
    GroupNode* itemBoxNode4 = itemBox4.getNodes();
    GroupNode* itemBoxNode5 = itemBox5.getNodes();
    GroupNode* itemBoxNode6 = itemBox6.getNodes();

    GroupNode* ramps0 = ramps.getNodes();

    GroupNode* wall0 = wall.getNodes();
    JumperNode::ConstructInfo jumperInfo = {glm::vec3(0.f, 1.f, 0.f), -10.f, 10.f, 5.f};
    JumperNode jumperNode = JumperNode(jumperInfo);
    wall0->add(&jumperNode);
    animNodes.push_back(&jumperNode);

    GroupNode cubeGroupNode = GroupNode(glm::rotate(glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-25.f, 0.f, 0.f)), glm::vec3(4.f, 4.f, 4.f)), 20.f ,glm::vec3(0.f, 0.f, 1.f)));

    glm::mat4 cubeTransform = glm::mat4(1.f);
    cubeTransform[3] = glm::vec4(20.f, -3.f, 0.f, 1.f);
    GroupNode cubeParent = GroupNode(cubeTransform);
    CubeNode cubeNode = CubeNode();
    cubeParent.add(&cubeNode);
    GroupNode* carNode = car->getNodes();
    cameraActor.getNodes();
    GroupNode* carNode2 = car2->getNodes();
    GroupNode cylinderGroup = GroupNode(glm::translate(glm::mat4(1.f), glm::vec3(-100.f, 0.f, 0.f)));

    glm::mat4 lightTranform = glm::mat4(1.f);
    lightTranform[3] = glm::vec4(0.f, 5.f, 0.f, 1.f);
    GroupNode lightGroup = GroupNode(lightTranform);
    LightNode light = LightNode();
    lightGroup.add(&light);

    rootNode.add(&cubeParent);
    rootNode.add(&lightGroup);
    rootNode.add(carNode);
    rootNode.add(carNode2);
    rootNode.add(planeNode);
    rootNode.add(itemBoxNode);
    rootNode.add(itemBoxNode2);
    rootNode.add(itemBoxNode3);
    rootNode.add(itemBoxNode4);
    rootNode.add(itemBoxNode5);
    rootNode.add(itemBoxNode6);
    rootNode.add(ramps0);
    rootNode.add(wall0);

    RasterVisitor rasterVisitor = RasterVisitor(shader);
    RasterVisitor debugVisitor = RasterVisitor(&debugShader);

    rasterVisitor.init(rootNode);

    float deltaT = 0.f;
    float lastTime = 0.f;
    glfwSetTime(0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // new frame
        GUI::newFrame();

        //Setup windows
        {
            GUI::setupDebugWindow();
            ImGui::ShowDemoWindow();
            GUI::setupTrackEditorWindow();
        }

        if(GUI::getShouldDrawDebug())
        {
            debugVisitor.render(rootNode);
            Physics::debugDrawWorld();
            GUI::render();
        }
        else
        {
            rasterVisitor.render(rootNode);
            GUI::render();
        }

        deltaT = glfwGetTime() - lastTime;
        update(window, deltaT);
        deathPlane.update(deltaT);
        itemBox.update(deltaT);
        itemBox2.update(deltaT);
        itemBox3.update(deltaT);
        itemBox4.update(deltaT);
        itemBox5.update(deltaT);
        itemBox6.update(deltaT);

        if(bPrintDeltaT)
            std::cout << deltaT << std::endl;
        lastTime = glfwGetTime();

        glfwSwapBuffers(window);
    }

    // Cleanup
    GUI::cleanUp();

    delete shader;
    delete mesh;
    delete car;
    delete car2;
    return 0;
    */

};