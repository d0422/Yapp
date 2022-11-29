#include "game.h"

#include "demoinputcomponent.h"
#include "dot.h"
#include "ghost.h"
#include "ghostgraphicscomponent.h"
#include "ghostinputcomponent.h"
#include "ghostphysicscomponent.h"
#include "keyinputcomponent.h"
#include "pacman.h"
#include "pacmangraphicscomponent.h"
#include "pacmanphysicscomponent.h"

Game::Game(QGraphicsScene* scene) : scene(scene) {
  dotFactory = new DotFactory(scene);
}

void Game::Init() {
  maze = new Maze();
  score = new Score(scene);
  life = 3;
  lifeLabel = new QGraphicsPixmapItem();
  scene->addItem(lifeLabel);
  lifeDisplay();
  foreach (Point dotPos, maze->WhereAreDots()) {
    GameObject* dot = dotFactory->CreateObject("dot", dotPos);
    connect(dot, SIGNAL(Eaten()), score, SLOT(IncreaseDotScore()));
    items.append(dot);
  }

  KeyInputComponent* key = new KeyInputComponent();
  pacman = new Pacman(QString("Pacman"), key, new PacmanPhysicsComponent(),
                      new PacmanGraphicsComponent(*scene));
  blinky = new Ghost(QString("blinky"),
                     new GhostInputComponent(new AggressiveChaseBehavior()),
                     new GhostPhysicsComponent(),
                     new GhostGraphicsComponent(*scene, "blinky"));
  pinky = new Ghost(
      QString("pinky"), new GhostInputComponent(new AmbushChaseBehavior()),
      new GhostPhysicsComponent(), new GhostGraphicsComponent(*scene, "pinky"));
  inky = new Ghost(
      QString("inky"), new GhostInputComponent(new PatrollChaseBehavior()),
      new GhostPhysicsComponent(), new GhostGraphicsComponent(*scene, "inky"));

  clyde = new Ghost(
      QString("clyde"), new GhostInputComponent(new RandomChaseBehavior()),
      new GhostPhysicsComponent(), new GhostGraphicsComponent(*scene, "clyde"));
  connect(pacman, SIGNAL(Eaten()), this, SLOT(lifeDecrease()));
  foreach (Point dotPos, maze->WhereArePellets()) {
    GameObject* pellet = dotFactory->CreateObject("pellet", dotPos);
    connect(pellet, SIGNAL(Eaten()), score, SLOT(IncreasePelletScore()));
    connect(pellet, SIGNAL(Eaten()), blinky, SLOT(PelletEaten()));
    connect(pellet, SIGNAL(Eaten()), pinky, SLOT(PelletEaten()));
    connect(pellet, SIGNAL(Eaten()), inky, SLOT(PelletEaten()));
    connect(pellet, SIGNAL(Eaten()), clyde, SLOT(PelletEaten()));
    items.append(pellet);
  }

  scene->installEventFilter(key);
}

void Game::GameLoop() {
  QTimer* loopTimer = new QTimer(this);
  connect(loopTimer, SIGNAL(timeout()), this, SLOT(Update()));
  loopTimer->start(70);
  // loopTimer->setInterval(100);
}

void Game::Update() {
  Pacman& pacmanObject = static_cast<Pacman&>(*pacman);
  pacman->Update(*maze);
  blinky->Update(*maze);
  clyde->Update(*maze);
  inky->Update(*maze);
  pinky->Update(*maze);
  foreach (GameObject* item, items) {
    item->Update(*maze);
  }
  lastPacmanState = pacmanObject.lifeStatus;
}

void Game::lifeDisplay() {
  lifeLabel->setPixmap(QPixmap(QString(":/res/img/lives_") +
                               QString::number(life) + QString(".png"))
                           .scaledToHeight(40));
  lifeLabel->setPos(0, 31 * 20);
}
void Game::lifeDecrease() {
  if (life == 0) {
    // game end
  } else {
    life -= 1;
    lifeDisplay();
  }
}
