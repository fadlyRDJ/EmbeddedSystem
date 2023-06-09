#include <EEPROM.h>
#include <LiquidCrystal.h>

/* Settings */
constexpr unsigned long kIntroDelay{1000};          // default: 1000
constexpr unsigned long kGameOverDelay{3000};       // default: 3000
constexpr unsigned long kActionTimeDelay{150};      // default: 150
constexpr unsigned long kMoveTimeDelay{350};        // default: 350
constexpr unsigned long kRapidFallLineDelay{20};    // default: 20
constexpr unsigned long kClearedLineScoreBonus{5};  // default: 5

/* Pins */
constexpr int kLeftButtonPin{8};
constexpr int kRightButtonPin{11};
constexpr int kRotateButtonPin{10};
constexpr int kRapidFallButtonPin{9};
constexpr int kLcdRsPin{1};
constexpr int kLcdEnablePin{2};
constexpr int kLcdD4Pin{3};
constexpr int kLcdD5Pin{4};
constexpr int kLcdD6Pin{5};
constexpr int kLcdD7Pin{6};

/* Constants */
constexpr int kFigures[7][4]{{0, 2, 4, 6}, {2, 4, 5, 7}, {3, 5, 4, 6},
                             {3, 5, 4, 7}, {2, 3, 5, 7}, {3, 5, 7, 6},
                             {2, 3, 4, 5}};
constexpr int kFiguresSize{sizeof(kFigures) / sizeof(kFigures[0])};
constexpr int kFigureSize{sizeof(kFigures[0]) / sizeof(kFigures[0][0])};

/**
 * The Board class is used to store and manage the state of the game board.
 */
class Board {
 public:
  /**
   * @param x The x-coordinate of the position
   * @param y The y-coordinate of the position
   *
   * @returns The boolean value of the board at the specified position.
   */
  bool At(const int x, const int y) const;
  /**
   * Sets the value of the board at the specified position to the
   * specified boolean value.
   *
   * @param x The x-coordinate of the position
   * @param y The y-coordinate of the position
   * @param value The new value at specified position
   */
  void Set(const int x, const int y, const bool value);
  /**
   * Checks for full lines on the board and clears them, moving any lines above
   * them down if necessary.
   *
   * @returns The number of lines cleared.
   */
  int ClearLines();
  /**
   * Clears the game board by setting all values in the board array to false.
   */
  void Clear();

  static constexpr int Width() { return kWidth; }
  static constexpr int Height() { return kHeight; }
  static constexpr int BlockWidth() { return kBlockWidth; }
  static constexpr int BlockHeight() { return kBlockHeight; }
  static constexpr int Columns() { return kWidth / kBlockWidth; }
  static constexpr int Rows() { return kHeight / kBlockHeight; }

 private:
  static constexpr int kWidth{16};
  static constexpr int kHeight{20};
  static constexpr int kBlockWidth{8};
  static constexpr int kBlockHeight{5};

  /**
   * Moves all the lines with y lower than the provided value down by one
   * block on the board. This method is used to update the board after clearing
   * lines.
   *
   * @param max_y The y-coordinate value below which lines should be moved down
   */
  void MoveLines(const int max_y);

  bool board_[kWidth][kHeight];
};

bool Board::At(const int x, const int y) const { return board_[x][y]; }

void Board::Set(const int x, const int y, const bool value) {
  board_[x][y] = value;
}

int Board::ClearLines() {
  int cleared_lines{0};

  int y{Board::Height() - 1};
  while (y >= 0) {
    int quantity{0};
    for (int x{0}; x < Board::Width(); ++x) {
      if (At(x, y)) ++quantity;
    }

    if (quantity == 0) {
      break;
    } else if (quantity == Board::Width()) {
      for (int x{0}; x < Board::Width(); ++x) {
        Set(x, y, false);
      }

      MoveLines(y);
      ++cleared_lines;
    } else {
      --y;
    }
  }

  return cleared_lines;
}

void Board::Clear() {
  for (int x{0}; x < Board::Width(); ++x) {
    for (int y{0}; y < Board::Height(); ++y) {
      Set(x, y, false);
    }
  }
}

void Board::MoveLines(const int max_y) {
  for (int y{max_y}; y > 0; --y) {
    for (int x{0}; x < Board::Width(); ++x) {
      Set(x, y, At(x, y - 1));
    }
  }
}

/**
 * The Tetromino class is responsible for creating and manipulating Tetromino
 * objects, which represent the various shapes used in the game of Tetris. A
 * Tetromino object is composed of four blocks arranged in a specific shape,
 * such as a straight line or a square. The Tetromino class provides functions
 * for moving and rotating these shapes on a game board.
 */
class Tetromino {
 public:
  /**
   * The Direction enum is used to indicate the direction to move the tetromino
   * horizontally. It has two possible values, kLeft and kRight, which
   * respectively represent moving the tetromino to the left or to the right.
   *
   * The Direction enum is used in conjunction with the Move method,
   * which moves the tetromino horizontally based on the specified direction.
   */
  enum class Direction {
    kLeft = -1,
    kRight = 1,
  };

  /**
   * The Tetromino class constructor creates a new Tetromino object of a random
   * figure from the set of pre-defined figures.
   */
  Tetromino();

  /**
   * Checks if the tetromino collides with any other blocks on the game board.
   *
   * @param board The board to check for collisions with the tetromino
   *
   * @returns True if there was a collision, false otherwise.
   */
  bool Collide(const Board& board) const;
  /**
   * Moves the current tetromino left or right by one column, if possible.
   *
   * @param board The board to check for collisions with the tetromino after
   * move in specified direction
   * @param direction The direction to move the tetromino, either
   * `Direction::kLeft`
   *
   * @returns True if the move occured, false otherwise.
   */
  bool Move(const Board& board, const Direction direction);
  /**
   * Rotates the tetromino by 90 degrees, if possible.
   *
   * @param board The board to check for collisions with the tetromino after
   * rotation
   *
   * @returns True if the rotation occured, false otherwise.
   */
  bool Rotate(const Board& board);
  /**
   * Moves the tetromino down by one row, if possible.
   *
   * @param board The board to check for collisions with the tetromino after
   * moving it down
   *
   * @returns True if the tetromino is at the bottom, false otherwise.
   */
  bool MoveDown(const Board& board);
  /**
   * Adds or removes a tetromino from the game board, depending on the value
   * of the passed-in argument.
   *
   * @param board The board to draw or remove the tetromino blocks from
   * @param value The boolean value, where true indicates that the tetromino
   * should be drawn on the board, and false indicates that it should be removed
   * from the board
   */
  void Draw(Board& board, const bool value) const;

 private:
  /**
   * The Block struct represents a single block of a tetromino. It consists of
   * two fields, x and y, which represent the x and y coordinates of the block,
   * respectively.
   */
  struct Block {
    int x;
    int y;
  };

  Block shape_[kFigureSize];
};

Tetromino::Tetromino() {
  constexpr int x{Board::Width() / 2};
  constexpr int y{0};

  const int figure{random(kFiguresSize)};
  for (int i{0}; i < kFigureSize; ++i) {
    shape_[i].x = x + kFigures[figure][i] % 2;
    shape_[i].y = y + kFigures[figure][i] / 2;
  }
}

bool Tetromino::Collide(const Board& board) const {
  for (int i{0}; i < kFigureSize; ++i) {
    const Block& block{shape_[i]};
    if (board.At(block.x, block.y)) return true;
  }
  return false;
}

bool Tetromino::Move(const Board& board, const Direction direction) {
  const int offset{static_cast<int>(direction)};

  for (int i{0}; i < kFigureSize; ++i) {
    const int new_x{shape_[i].x + offset};
    if (new_x < 0 || new_x >= Board::Width()) return false;
    if (board.At(new_x, shape_[i].y)) return false;
  }

  for (int i{0}; i < kFigureSize; ++i) {
    shape_[i].x += offset;
  }
  return true;
}

bool Tetromino::Rotate(const Board& board) {
  // Disables square rotation
  if (abs(shape_[3].x - shape_[0].x) == 1 &&
      abs(shape_[3].y - shape_[0].y) == 1)
    return false;

  const Block& center = shape_[2];
  Block rotated[kFigureSize];

  for (int i{0}; i < kFigureSize; ++i) {
    const int x{shape_[i].y - center.y};
    const int y{shape_[i].x - center.x};
    rotated[i].x = center.x - x;
    rotated[i].y = center.y + y;

    if (rotated[i].x < 0 || rotated[i].x >= Board::Width()) return false;
    if (rotated[i].y < 0 || rotated[i].x >= Board::Height()) return false;
    if (board.At(rotated[i].x, rotated[i].y)) return false;
  }

  for (int i{0}; i < kFigureSize; ++i) {
    shape_[i].x = rotated[i].x;
    shape_[i].y = rotated[i].y;
  }
  return true;
}

bool Tetromino::MoveDown(const Board& board) {
  for (int i{0}; i < kFigureSize; ++i) {
    if (shape_[i].y >= Board::Height() - 1) return true;
    if (board.At(shape_[i].x, shape_[i].y + 1)) return true;
  }

  for (int i{0}; i < kFigureSize; ++i) {
    ++shape_[i].y;
  }
  return false;
}

void Tetromino::Draw(Board& board, const bool value) const {
  for (int i{0}; i < kFigureSize; ++i) {
    const Block& block = shape_[i];
    board.Set(block.x, block.y, value);
  }
}

/**
 * The Display class is responsible for managing the display of the game on an
 * LCD screen. It provides methods for printing various messages, drawing the
 * game board, tetromino shapes, and score information on the screen.
 */
class Display {
 public:
  /**
   * The constructor takes six arguments, which are the pin numbers for the LCD
   * screen. It creates a LiquidCrystal object underneath to interface with the
   * LCD screen.
   *
   * @param rs
   * @param enable
   * @param d4
   * @param d5
   * @param d6
   * @param d7
   */
  Display(int rs, int enable, int d4, int d5, int d6, int d7)
      : display_{rs, enable, d4, d5, d6, d7} {}

  /**
   * Draws the game board on the display, including the current tetromino, if
   * available.
   *
   * @param board The game board
   * @param tetromino The current tetromino (optional)
   **/
  void DrawBoard(Board& board, const Tetromino* const tetromino);
  /**
   * Updates the game score on the display.
   *
   * @param score The new score to display
   */
  void PrintScore(const int score);

  /**
   * Displays an introductory message that includes the game title and author.
   */
  void Intro();
  /**
   * Clears the game display and prints the game score, starting with a score of
   * 0.
   */
  void Start();
  /**
   * Displays the player's current score and high score on the game screen.
   *
   * @param score The current score of the game
   * @param high_score The highest score achieved in all previous games
   */
  void GameOver(const int score, const int high_score);
  /**
   * Displays the "game over" message and provides instructions for restarting
   * the game.
   */
  void Restart();

 private:
  /**
   * Updates the character for specific segment of the board.
   *
   * @param column The number of column on the display
   * @param row The number of row on the display
   * @param board The game board
   *
   * @returns True if any changes were made, false otherwise.
   */
  bool UpdateCharacter(const int column, const int row, const Board& board);

  LiquidCrystal display_;
  uint8_t characters_[Board::Columns()][Board::Rows()][Board::BlockWidth()];
};

void Display::DrawBoard(Board& board, const Tetromino* const tetromino) {
  if (tetromino) tetromino->Draw(board, true);

  for (int column{0}; column < Board::Columns(); ++column) {
    for (int row{0}; row < Board::Rows(); ++row) {
      if (UpdateCharacter(column, row, board)) {
        const int index{column * Board::Rows() + row};
        display_.createChar(index, characters_[column][row]);
        display_.setCursor(row, column);
        display_.write(uint8_t(index));
      }
    }
  }

  if (tetromino) tetromino->Draw(board, false);
}

void Display::PrintScore(const int score) {
  display_.setCursor(9, 1);
  display_.print(min(score, 999));
}

void Display::Intro() {
  uint8_t block_character[] = {0b11111, 0b11111, 0b11111, 0b11111,
                               0b11111, 0b11111, 0b11111, 0b11111};
  display_.createChar(0, block_character);
  display_.begin(16, 2);

  display_.setCursor(0, 0);
  display_.print("TET ");
  display_.write(uint8_t(0));
  display_.print(" Author:");

  display_.setCursor(0, 1);
  display_.print("RIS ");
  display_.write(uint8_t(0));
  display_.print(" Kelompok 3");
}

void Display::Start() {
  for (int column{0}; column < Board::Columns(); ++column) {
    for (int row{0}; row < Board::Rows(); ++row) {
      for (int y{0}; y < Board::BlockWidth(); ++y) {
        characters_[column][row][y] = 0b00000;
      }
    }
  }

  display_.clear();
  display_.setCursor(4, 0);
  display_.print("XXXX Score:");

  display_.setCursor(4, 1);
  display_.print("XXXX");
  PrintScore(0);
}

void Display::GameOver(const int score, const int high_score) {
  uint8_t crown_character[] = {0b00000, 0b00000, 0b00000, 0b10101,
                               0b11111, 0b11111, 0b11111, 0b00000};
  display_.createChar(0, crown_character);
  display_.clear();

  display_.setCursor(0, 0);
  display_.print("Your score: ");
  display_.print(score);
  if (score >= high_score) display_.write(uint8_t(0));

  display_.setCursor(0, 1);
  display_.print("High score: ");
  display_.print(high_score);
  if (score <= high_score) display_.write(uint8_t(0));
}

void Display::Restart() {
  display_.clear();
  display_.setCursor(0, 0);
  display_.print("Game over! Press");
  display_.setCursor(0, 1);
  display_.print("rotate to play.");
}

bool Display::UpdateCharacter(const int column, const int row,
                              const Board& board) {
  const int x{column * Board::BlockWidth()};
  const int y{(Board::Rows() - row - 1) * Board::BlockHeight()};

  uint8_t character[8];
  constexpr uint8_t kPowers[]{1, 2, 4, 8, 16};

  for (int i{0}; i < Board::BlockWidth(); ++i) {
    uint8_t value{0};
    for (int j{0}; j < Board::BlockHeight(); ++j) {
      value += board.At(x + i, y + j) * kPowers[j];
    }
    character[i] = value;
  }

  bool changes{false};
  for (int i{0}; i < Board::BlockWidth(); ++i) {
    if (characters_[column][row][i] != character[i]) {
      characters_[column][row][i] = character[i];
      changes = true;
    }
  }

  return changes;
}

/**
 * Responsible for initializing and managing the state of the game. It also
 * handles user input and updates the game state accordingly.
 */
class Game {
 public:
  /**
   * Deallocates the memory used by the display and tetromino objects.
   */
  ~Game();

  /**
   * Initializes the pins for input, sets up the EEPROM
   * for reading and writing high score data, initializes the display for
   * output, and starts the intro sequence.
   */
  void Setup();
  /**
   * Performs an update on the game state every tick, which includes
   * updating the tetromino position, checking for completed lines,
   * and handling user input.
   */
  void Update();

 private:
  static constexpr int kMemoryHighScoreAddress{0};

  /**
   * Handles a rapid tetromino fall if the user chooses to do so. If the user
   * presses the "drop" key, the method immediately moves the current tetromino
   * to the bottom of the board, then clears any completed lines and updates
   * score.
   *
   *  @returns True if rapid fall occured, false otherwise.
   */
  bool HandleRapidFall();
  /**
   * Checks user input and updates the tetromino if possible.
   *
   * @param time The elapsed time in milliseconds
   *
   * @returns True if resulted in any changes to the board, false otherwise.
   */
  bool HandleUserInput(const unsigned long time);
  /**
   * Moves the current tetromino down by one cell if the delta time
   * since the last move down is greater than a constant value.
   *
   * @param time The elapsed time in milliseconds
   *
   * @returns True if resulted in any changes to the board, false otherwise.
   */
  bool HandleTetrominoMoveDown(const unsigned long time);
  /**
   * Removes the current tetromino from the board and updates the score.
   */
  void RemoveTetromino();

  /**
   * Displays the game's intro sequence, which includes showing the game
   * title and author. Once the intro is complete, the method starts the game.
   */
  void Intro();
  /**
   * Resets the game state and starts the game. This includes resetting
   * the score, deleting the tetromino, and clearing the game board.
   */
  void Start();
  /**
   * Displays the "game over" message and waits for the player to restart the
   * game. If the player has achieved a new high score, the method saves it to
   * the EEPROM memory.
   */
  void GameOver();

  Board board_;
  Display* display_;
  Tetromino* tetromino_;

  int score_{0};
  unsigned long last_action_time_{0};
  unsigned long last_move_time_{0};
};

Game::~Game() {
  delete display_;
  delete tetromino_;
}

void Game::Setup() {
  pinMode(kLeftButtonPin, INPUT_PULLUP);
  pinMode(kRightButtonPin, INPUT_PULLUP);
  pinMode(kRotateButtonPin, INPUT_PULLUP);
  pinMode(kRapidFallButtonPin, INPUT_PULLUP);

  randomSeed(analogRead(0));

  if (!EEPROM.read(kMemoryHighScoreAddress)) {
    EEPROM.write(kMemoryHighScoreAddress, 0);
  }

  display_ = new Display(kLcdRsPin, kLcdEnablePin, kLcdD4Pin, kLcdD5Pin,
                         kLcdD6Pin, kLcdD7Pin);

  Intro();
}

void Game::Update() {
  bool changes{false};

  if (!tetromino_) {
    tetromino_ = new Tetromino();
    changes = true;

    if (tetromino_->Collide(board_)) return GameOver();
  }

  const unsigned long time{millis()};
  if (HandleRapidFall()) return;
  if (HandleUserInput(time)) changes = true;
  if (HandleTetrominoMoveDown(time)) changes = true;

  if (changes) display_->DrawBoard(board_, tetromino_);
}

bool Game::HandleRapidFall() {
  if (digitalRead(kRapidFallButtonPin) == LOW) {
    while (!tetromino_->MoveDown(board_)) {
      display_->DrawBoard(board_, tetromino_);
      delay(kRapidFallLineDelay);
    }

    RemoveTetromino();
    return true;
  }

  return false;
}

bool Game::HandleUserInput(const unsigned long time) {
  const unsigned long last_action_delta{time - last_action_time_};

  if (last_action_delta >= kActionTimeDelay) {
    bool action{false};

    if (digitalRead(kLeftButtonPin) == LOW) {
      action = tetromino_->Move(board_, Tetromino::Direction::kLeft);
    } else if (digitalRead(kRightButtonPin) == LOW) {
      action = tetromino_->Move(board_, Tetromino::Direction::kRight);
    } else if (digitalRead(kRotateButtonPin) == LOW) {
      action = tetromino_->Rotate(board_);
    }

    if (action) {
      last_action_time_ = time;
      return true;
    }
  }

  return false;
}

bool Game::HandleTetrominoMoveDown(const unsigned long time) {
  const unsigned long last_move_delta{time - last_move_time_};

  if (last_move_delta >= kMoveTimeDelay) {
    if (tetromino_->MoveDown(board_)) RemoveTetromino();
    last_move_time_ = time;
    return true;
  }

  return false;
}

void Game::RemoveTetromino() {
  tetromino_->Draw(board_, true);
  delete tetromino_;
  tetromino_ = nullptr;

  ++score_;
  score_ += board_.ClearLines() * kClearedLineScoreBonus;

  display_->PrintScore(score_);
}

void Game::Intro() {
  display_->Intro();
  delay(kIntroDelay);
  Start();
}

void Game::Start() {
  delete tetromino_;
  tetromino_ = nullptr;

  score_ = 0;
  last_action_time_ = millis();
  last_move_time_ = millis();

  board_.Clear();
  display_->Start();
}

void Game::GameOver() {
  const int high_score{EEPROM.read(0)};

  display_->GameOver(score_, high_score);
  if (score_ > high_score) EEPROM.update(0, score_);
  delay(kGameOverDelay);

  display_->Restart();
  while (digitalRead(kRotateButtonPin) == HIGH) {
  }
  Start();
}

Game game;

void setup() { game.Setup(); }

void loop() { game.Update(); }
